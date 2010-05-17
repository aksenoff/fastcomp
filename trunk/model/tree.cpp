#include "tree.h"
extern FILE *log_file;
bool newTree = false, escaped = false, nodeAdded = false, rootModified = false;

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

unsigned short calculate_escape(Tree *t)
{
	unsigned short numNodes = t->numNodes-1;
	if(nodeAdded) 
	{
		numNodes--;
		nodeAdded = false;
	}
	return(((256 - numNodes) * numNodes) / (256 * (t->rootNode->selfCount)) + 1);
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
	unsigned short escape_count = 0;
	if(escaped || newTree)
	{
		newTree = escaped = false;
#ifdef DBG
		fprintf(log_file,"enc: Enc %d in nt\n",d);
#endif
		encode_in_null_table(d, s);
#ifdef DBG
		fprintf(log_file,"enc: %d %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
#endif
		return false;
	}
	Node *node;
	unsigned short totalRightCount;
	unsigned short totalLeftCount;
	unsigned short oldLeftCount;
	unsigned short selfCount;
	if(rootNode) node = rootNode;
	else 
	{
		node = rootNode = new Node(d); // that's why we need 3rd parameter
		newTree = true;
		nodeAdded = true;
		numNodes++;
	}
	if (t->totalCount == 0x3FFF) t->rescale();
	totalRightCount = totalCount;
	totalCount++;	
#ifdef DBG
		fprintf(log_file,"enc: Total %d\n",totalCount);
#endif
	if(!newTree) {
		//descending
		oldLeftCount = totalLeftCount = node->leftCount;
		selfCount = node->selfCount;
		totalRightCount -= selfCount + totalLeftCount;
		while(node->data != d)
		{
			if(node->data > d)
			{
				node->leftCount++;
				if(node->left) 
				{
					node = node->left;
					totalRightCount += selfCount + oldLeftCount - node->selfCount - node->leftCount;
					totalLeftCount += node->leftCount - oldLeftCount;
					selfCount = node->selfCount;
					oldLeftCount = node->leftCount;
				}
				else {
					node->createChild(d, this);
					escaped = true;
					break;
				}				
			}
			else if(node->right)
				 {
					 node = node->right;
					 totalRightCount -= node->selfCount + node->leftCount;
					 totalLeftCount += selfCount + node->leftCount;
					 selfCount = node->selfCount;
					 oldLeftCount = node->leftCount;
				 }
				 else {
					 node->createChild(d, this);
					 escaped = true;
					 break;
				 }
		}
		//end of descending
		escape_count = calculate_escape(t);
#ifdef DBG
		fprintf(log_file,"enc: Esc %d\n",escape_count);
#endif
		if(!escaped)
		{
			node->selfCount++;
			if(node==rootNode)rootModified = true;
		}
		if(escaped) {
			s->low_count = t->totalCount-1;
			s->scale = s->high_count = s->low_count + escape_count;
#ifdef DBG
		fprintf(log_file,"enc: %d %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
#endif
		}
		else {
			s->low_count = totalLeftCount;
			s->high_count = totalLeftCount + selfCount;
			s->scale = totalLeftCount + selfCount + 
						totalRightCount + escape_count;
#ifdef DBG
		fprintf(log_file,"enc: %d %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
#endif
		}
	}
	else {
		s->low_count = 0;
		s->high_count = 1;
		s->scale = 1;
#ifdef DBG
		fprintf(log_file,"enc: %d %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
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
	fprintf(log_file,"get_sc: Scale %d\n",s->scale);
#endif
}

short get_byte(unsigned short count, SYMBOL *s)
{
	unsigned short totalRightCount;
	unsigned short totalLeftCount;
	unsigned short oldLeftCount;
	unsigned short selfCount;
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
		fprintf(log_file,"get_b: Tree<-%d\n",d);
#endif
		if(tree->rootNode) node = tree->rootNode;
		else 
		{
			node = tree->rootNode = new Tree::Node(d);
			newTree = true;
			nodeAdded = true;
			tree->numNodes++;
		}
		totalRightCount = tree->totalCount;
		tree->totalCount++;		
#ifdef DBG
		fprintf(log_file,"get_b: Total %d\n",tree->totalCount);
#endif
		if(!newTree)
		{
			//descending
			oldLeftCount = totalLeftCount = node->leftCount;
			selfCount = node->selfCount;
			totalRightCount -= selfCount + totalLeftCount;
			while(node->data != d)
			{
				if(node->data > d)
				{
					node->leftCount++;
					if(node->left) 
					{
						node = node->left;
						totalRightCount += selfCount + oldLeftCount - node->selfCount - node->leftCount;
						totalLeftCount += node->leftCount - oldLeftCount;
						selfCount = node->selfCount;
						oldLeftCount = node->leftCount;
					}
					else {
						node->createChild(d, tree);
						escaped = true;
						break;
					}				
				}
				else if(node->right)
					 {
						 node = node->right;
						 totalRightCount -= node->selfCount + node->leftCount;
						 totalLeftCount += selfCount + node->leftCount;
						 selfCount = node->selfCount;
						 oldLeftCount = node->leftCount;
					 }
					 else {
						 node->createChild(d, tree);
						 escaped = true;
						 break;
					 }
			}
			if(!escaped)node->selfCount++;
			//done descending
		}
		newTree = nodeAdded = escaped = false;
		if (tree->totalCount == 0x3FFF) tree->rescale();
		return count;
	}
	if(!(tree->rootNode))
	{
		s->low_count = 0;
		s->high_count = 1;
		escaped = true;
		return ESCAPE;
	}
	else if(count >= tree->totalCount)
	{
		s->low_count = tree->totalCount;
		s->scale = s->high_count = s->low_count + calculate_escape(t);
		escaped = true;
#ifdef DBG
		fprintf(log_file,"get_b: Esc %d:%d/%d\n",s->low_count,s->high_count,s->scale);
#endif
		return ESCAPE;
	}
	else
	{
#ifdef DBG
		fprintf(log_file,"get_b: srch for %d\n", count);
#endif
		node = tree->rootNode;
		tree->totalCount++;
		totalRightCount = tree->totalCount;
#ifdef DBG
		fprintf(log_file,"get_b: Total %d\n",tree->totalCount);
#endif
		oldLeftCount = totalLeftCount = node->leftCount;
		selfCount = node->selfCount;
		totalRightCount -= selfCount + totalLeftCount;
		while(!((totalLeftCount <= count) && ((totalLeftCount+selfCount) > count)))
		{
			if(totalLeftCount > count)
			{
				node->leftCount++;
				node = node->left;
				totalRightCount += selfCount + oldLeftCount - node->selfCount - node->leftCount;
				totalLeftCount += node->leftCount - oldLeftCount;
				selfCount = node->selfCount;
				oldLeftCount = node->leftCount;
			}
			else {
				 node = node->right;
				 totalRightCount -= node->selfCount + node->leftCount;
				 totalLeftCount += selfCount + node->leftCount;
				 selfCount = node->selfCount;
				 oldLeftCount = node->leftCount;
		     }
		}
		node->selfCount++;
		if (tree->totalCount == 0x3FFF) tree->rescale();
#ifdef DBG
		fprintf(log_file,"get_byte: byte %d\n",node->data);
#endif
		s->low_count = totalLeftCount;
		s->high_count = totalLeftCount + selfCount;
#ifdef DBG
		fprintf(log_file,"get_byte: s %d:%d/%d\n",s->low_count,s->high_count,s->scale);
#endif
		return node->data;
	}
}

void Tree::rescale()
{
	totalCount = rootNode->rescale();
}

unsigned short Tree::Node::rescale()
{
#ifdef DBG
		fprintf(log_file,"Rescaling\n");
#endif
	unsigned short tempRightCount = 0;
	if(selfCount != 1) selfCount >>= 1;
	if(left)
		leftCount = left->rescale();
	if(right)
		tempRightCount = right->rescale();
	return (selfCount + leftCount + tempRightCount);
}