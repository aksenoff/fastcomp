#include "tree.h"
#include "encoder.h"

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
	tree->numNodes++; //! � ������ �����
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

bool Tree::encode(BYTE d, SYMBOL* s)
{
	long escape_count = 0;
	if(escaped || newTree)
	{
		newTree = escaped = false;
		encode_in_null_table(d, s);
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
		node = rootNode = new Node(d); // that's why we need 3rd parameter
		newTree = true;
		nodeAdded = true;
		numNodes++;
	}
	totalCount++;
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
		if(!escaped)node->selfCount++;
		//end of descending
		escape_count = calculate_escape(t);
		if(escaped) {
			s->low_count = t->totalCount-1;
			s->scale = s->high_count = s->low_count + escape_count;
		}
		else {
			s->low_count = totalLeftCount;
			s->high_count = totalLeftCount + selfCount;
			s->scale = totalLeftCount + selfCount + 
						totalRightCount + escape_count;
		}
	}
	else {
		s->low_count = 0;
		s->high_count = 1;
		s->scale = 1;
	}
	return (newTree || escaped);
}

bool get_symbol(BYTE b, SYMBOL *s)
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
}

short get_byte(long count, SYMBOL *s)
{
	long totalRightCount;
	long totalLeftCount;
	long oldLeftCount;
	long selfCount;
	long escape_count = 0;
	Tree::Node* node;
	Tree* tree = t;
	BYTE d;
	if(escaped)
	{
		s->low_count = count;
		s->high_count = count + 1;
		d = (BYTE)count;
		totalRightCount = tree->totalCount;
		if(tree->rootNode) node = tree->rootNode;
		else 
		{
			node = tree->rootNode = new Tree::Node(d);
			newTree = true;
			nodeAdded = true;
			tree->numNodes++;
		}
		tree->totalCount++;
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
		return count;
	}
	if(!(tree->rootNode))
	{
		s->low_count = 0;
		s->high_count = 1;
		escaped = true;
		return ESCAPE;
	}
	else if(count > tree->totalCount-1)
	{
		s->low_count = tree->totalCount;
		s->scale = s->high_count = s->low_count + calculate_escape(t);
		escaped = true;
		return ESCAPE;
	}
	else
	{
		totalRightCount = tree->totalCount;
		node = tree->rootNode;
		tree->totalCount++;
		oldLeftCount = totalLeftCount = node->leftCount;
		selfCount = node->selfCount;
		totalRightCount -= selfCount + totalLeftCount;
		while(totalLeftCount!=count)
		{
			if(totalLeftCount > count)
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
			}
			else if(node->right)
			 {
				 node = node->right;
				 totalRightCount -= node->selfCount + node->leftCount;
				 totalLeftCount += selfCount + node->leftCount;
				 selfCount = node->selfCount;
				 oldLeftCount = node->leftCount;
		     }
		}
		node->selfCount++;
		return node->data;
	}
}