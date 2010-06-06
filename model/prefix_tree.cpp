#include "prefix_tree.h"

PrefixTree::PrefixTree()
	: rootNode(0) {};

Context* PrefixTree::addPrefix(BYTE d)
{
	if(!rootNode) return rootNode = new Context(d);
	else 
	{
		Context *node = rootNode;
		while(node->data != d)
		{
			if(node->data > d)
			{
				if(node->left) node = node->left;
				else return node->left = new Context(d);
			}
			else
			{
				if(node->right) node = node->right;
				else return node->right = new Context(d);
			}
		}
	}
}