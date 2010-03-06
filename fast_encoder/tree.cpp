#include "tree.h"
extern FILE *log_file;
bool newTree = false, escaped = false, nodeAdded = false;

Tree::Tree(const BYTE d)
	: totalCount(1), numNodes(1) {rootNode = new Node(d);}
Tree::Tree()
	: totalCount(0), numNodes(0), rootNode(0) {};
Tree::Tree(const Tree&) {};
Tree& Tree::operator=(const Tree&) {return *this;}; //!
Tree::~Tree() {};

Tree::Node::Node()
	: selfCount(1), leftCount(0), left(0), right(0) {};
Tree::Node::Node(const Node &n) {};
Tree::Node::Node(const BYTE d)
	: selfCount(1), leftCount(0), data(d), left(0), right(0) {};
Tree::Node& Tree::Node::operator=(const Node&) {return *this;}; //!
Tree::Node::~Node() {};
Tree::Node* Tree::Node::createChild(const BYTE d, Tree *tree)
{
	tree->numNodes++; //! в другом месте
	nodeAdded = true;
	if(data > d) return (left = new Tree::Node(d));
	else return (right = new Tree::Node(d));
};

Tree *t;

void initialize_model()
{
	t = new Tree;
}

long calculate_escape(Tree *t)
{
	short numNodes = t->numNodes-1;
	if(nodeAdded) 
	{
		numNodes--;
		nodeAdded = false;
	}
	return(((256 - numNodes) * numNodes) / (256 * t->rootNode->selfCount) + 1);
}

void encode_in_null_table(BYTE d, SYMBOL* s)
{
	s->low_count = d;
	s->high_count = d+1;
	s->scale = 256;
}

bool Tree::encode(short ds, SYMBOL* s)
{
	BYTE d = (BYTE)ds;
	long escape_count = 0;
	if(escaped || newTree)
	{
		newTree = escaped = false;
#ifdef DBG
		fprintf(log_file,"encode: Encoding %d in null table\n",d);
#endif
		encode_in_null_table(d, s);
#ifdef DBG
		fprintf(log_file,"encode: %d encoded as %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
#endif
		return false;
	}
	Node *node;
	long totalRightCount;
	long totalLeftCount;
	long oldLeftCount;
	long selfCount;
	totalRightCount = totalCount;
	if(rootNode) node = rootNode;
	else 
	{
#ifdef DBG
		fprintf(log_file,"encode: Creating new tree with root node containing %d\n",d);
#endif
		node = rootNode = new Node(d); // that's why we need 3rd parameter
		newTree = true;
		nodeAdded = true;
		numNodes++;
	}
	totalCount++;
#ifdef DBG
		fprintf(log_file,"encode: Total count increased to %d\n",totalCount);
#endif
	if(!newTree) {
		//descending
#ifdef DBG
		fprintf(log_file,"encode: Traversing the tree in search for %d\n",d);
#endif
		oldLeftCount = totalLeftCount = node->leftCount;
		selfCount = node->selfCount;
		totalRightCount -= selfCount + totalLeftCount;
#ifdef DBG
		fprintf(log_file,"encode: Currently %d is on the left, %d is on the right, and current node has %d\n",totalLeftCount,totalRightCount,selfCount);
#endif
		while(node->data != d)
		{
#ifdef DBG
		fprintf(log_file,"encode: Current node has %d instead of %d\n",node->data,d);
#endif
			if(node->data > d)
			{
				node->leftCount++;
				if(node->left) 
				{
#ifdef DBG
		fprintf(log_file,"encode: Gone left\n");
#endif
					node = node->left;
					totalRightCount += selfCount + oldLeftCount - node->selfCount - node->leftCount;
					totalLeftCount += node->leftCount - oldLeftCount;
					selfCount = node->selfCount;
					oldLeftCount = node->leftCount;
#ifdef DBG
		fprintf(log_file,"encode: Currently %d is on the left, %d is on the right, and current node has %d\n",totalLeftCount,totalRightCount,selfCount);
#endif
				}
				else {
#ifdef DBG
		fprintf(log_file,"encode: No left child. Creating one\n");
#endif
					node->createChild(d, this);
					escaped = true;
					break;
				}				
			}
			else if(node->right)
				 {
#ifdef DBG
		fprintf(log_file,"encode: Gone right\n");
#endif
					 node = node->right;
					 totalRightCount -= node->selfCount + node->leftCount;
					 totalLeftCount += selfCount + node->leftCount;
					 selfCount = node->selfCount;
					 oldLeftCount = node->leftCount;
#ifdef DBG
		fprintf(log_file,"encode: Currently %d is on the left, %d is on the right, and current node has %d\n",totalLeftCount,totalRightCount,selfCount);
#endif
				 }
				 else {
#ifdef DBG
		fprintf(log_file,"encode: No right child. Creating one\n");
#endif
					 node->createChild(d, this);
					 escaped = true;
					 break;
				 }
		}
		//end of descending
		escape_count = calculate_escape(t);
#ifdef DBG
		fprintf(log_file,"encode: Escape count calculated and equals %d\n",escape_count);
#endif
		if(!escaped)node->selfCount++;
		if(escaped) {
			s->low_count = t->totalCount-1;
			s->scale = s->high_count = s->low_count + escape_count;
#ifdef DBG
		fprintf(log_file,"encode: %d encoded as %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
#endif
		}
		else {
			s->low_count = totalLeftCount;
			s->high_count = totalLeftCount + selfCount;
			s->scale = totalLeftCount + selfCount + 
						totalRightCount + escape_count;
#ifdef DBG
		fprintf(log_file,"encode: %d encoded as %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
#endif
		}
	}
	else {
		s->low_count = 0;
		s->high_count = 1;
		s->scale = 1;
#ifdef DBG
		fprintf(log_file,"encode: %d encoded as %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
#endif
	}
	return (newTree || escaped);
}

bool get_symbol(short b, SYMBOL *s)
{
	return t->encode(b, s);
}

void get_scale(SYMBOL* s)
{	
	if(escaped) 
	{
		s->scale = 256;
	}
	else if(t->rootNode)
			s->scale = t->totalCount + calculate_escape(t);
		 else s->scale = 1;
#ifdef DBG
		fprintf(log_file,"get_scale: Scale counted and equals %d\n",s->scale);
#endif
}

long get_byte(unsigned long count, SYMBOL *s)
{
	unsigned long totalRightCount;
	unsigned long totalLeftCount;
	unsigned long oldLeftCount;
	unsigned long selfCount;
	//unsigned long escape_count = 0;
	Tree::Node* node;
	Tree* tree = t;
	BYTE d;
	if(escaped)
	{
		s->low_count = count;
		s->high_count = count + 1;
		d = (BYTE)count;
#ifdef DBG
		fprintf(log_file,"get_byte: We've escaped, but still need to update the tree with %d\n",d);
#endif
		totalRightCount = tree->totalCount;
		if(tree->rootNode) node = tree->rootNode;
		else 
		{
#ifdef DBG
		fprintf(log_file,"get_byte: Creating new tree with %d in its root\n",d);
#endif
			node = tree->rootNode = new Tree::Node(d);
			newTree = true;
			nodeAdded = true;
			tree->numNodes++;
		}
		tree->totalCount++;
#ifdef DBG
		fprintf(log_file,"get_byte: Total count increased to %d\n",tree->totalCount);
#endif
		if(!newTree)
		{
#ifdef DBG
		fprintf(log_file,"get_byte: Traversing the tree in search for %d\n",d);
#endif
			//descending
			oldLeftCount = totalLeftCount = node->leftCount;
			selfCount = node->selfCount;
			totalRightCount -= selfCount + totalLeftCount;
#ifdef DBG
		fprintf(log_file,"get_byte: Currently %d is on the left, %d is on the right, and current node has %d\n",totalLeftCount,totalRightCount,selfCount);
#endif
			while(node->data != d)
			{
#ifdef DBG
		fprintf(log_file,"get_byte: Current node has %d instead of %d\n",node->data,d);
#endif
				if(node->data > d)
				{
					node->leftCount++;
					if(node->left) 
					{
#ifdef DBG
		fprintf(log_file,"get_byte: Gone left\n");
#endif
						node = node->left;
						totalRightCount += selfCount + oldLeftCount - node->selfCount - node->leftCount;
						totalLeftCount += node->leftCount - oldLeftCount;
						selfCount = node->selfCount;
						oldLeftCount = node->leftCount;
#ifdef DBG
		fprintf(log_file,"get_byte: Currently %d is on the left, %d is on the right, and current node has %d\n",totalLeftCount,totalRightCount,selfCount);
#endif
					}
					else {
#ifdef DBG
		fprintf(log_file,"get_byte: No left child. Creating one\n");
#endif
						node->createChild(d, tree);
						escaped = true;
						break;
					}				
				}
				else if(node->right)
					 {
#ifdef DBG
		fprintf(log_file,"get_byte: Gone right\n");
#endif
						 node = node->right;
						 totalRightCount -= node->selfCount + node->leftCount;
						 totalLeftCount += selfCount + node->leftCount;
						 selfCount = node->selfCount;
						 oldLeftCount = node->leftCount;
#ifdef DBG
		fprintf(log_file,"get_byte: Currently %d is on the left, %d is on the right, and current node has %d\n",totalLeftCount,totalRightCount,selfCount);
#endif
					 }
					 else {
#ifdef DBG
		fprintf(log_file,"get_byte: No right child. Creating one\n");
#endif
						 node->createChild(d, tree);
						 escaped = true;
						 break;
					 }
			}
			if(!escaped)node->selfCount++;
			//done descending
		}
		newTree = nodeAdded = escaped = false;
		return count;
	}
	if(!(tree->rootNode))
	{
		s->low_count = 0;
		s->high_count = 1;
		escaped = true;
#ifdef DBG
		fprintf(log_file,"get_byte: No tree. Escaping with 0:1/1\n");
#endif
		return ESCAPE;
	}
	else if(count >= tree->totalCount)
	{
		s->low_count = tree->totalCount;
		s->scale = s->high_count = s->low_count + calculate_escape(t);
		escaped = true;
#ifdef DBG
		fprintf(log_file,"get_byte: Byte not present in tree. Escaping with %d:%d/%d\n",s->low_count,s->high_count,s->scale);
#endif
		return ESCAPE;
	}
	else
	{
#ifdef DBG
		fprintf(log_file,"get_byte: Tree has byte, but we need to find it. Target count is %d\n", count);
#endif
		totalRightCount = tree->totalCount;
		node = tree->rootNode;
		tree->totalCount++;
#ifdef DBG
		fprintf(log_file,"get_byte: Total count increased to %d\n",tree->totalCount);
#endif
		oldLeftCount = totalLeftCount = node->leftCount;
		selfCount = node->selfCount;
		totalRightCount -= selfCount + totalLeftCount;
#ifdef DBG
		fprintf(log_file,"get_byte: Currently %d is on the left, %d is on the right, and current node has %d\n",totalLeftCount,totalRightCount,selfCount);
#endif
		while(!((totalLeftCount <= count) && ((totalLeftCount+selfCount) > count)))
		{
			if(totalLeftCount > count)
			{
				node->leftCount++;
#ifdef DBG
		fprintf(log_file,"get_byte: We have to move left\n");
#endif
				node = node->left;
				totalRightCount += selfCount + oldLeftCount - node->selfCount - node->leftCount;
				totalLeftCount += node->leftCount - oldLeftCount;
				selfCount = node->selfCount;
				oldLeftCount = node->leftCount;
#ifdef DBG
		fprintf(log_file,"get_byte: Currently %d is on the left, %d is on the right, and current node has %d\n",totalLeftCount,totalRightCount,selfCount);
#endif				
			}
			else {
#ifdef DBG
		fprintf(log_file,"get_byte: We have to move right\n");
#endif
				 node = node->right;
				 totalRightCount -= node->selfCount + node->leftCount;
				 totalLeftCount += selfCount + node->leftCount;
				 selfCount = node->selfCount;
				 oldLeftCount = node->leftCount;
#ifdef DBG
		fprintf(log_file,"get_byte: Currently %d is on the left, %d is on the right, and current node has %d\n",totalLeftCount,totalRightCount,selfCount);
#endif
		     }
		}
		node->selfCount++;
#ifdef DBG
		fprintf(log_file,"get_byte: Target byte is %d\n",node->data);
#endif
		s->low_count = totalLeftCount;
		s->high_count = totalLeftCount + selfCount;
#ifdef DBG
		fprintf(log_file,"get_byte: s is %d:%d/%d\n",s->low_count,s->high_count,s->scale);
#endif
		return node->data;
	}
}