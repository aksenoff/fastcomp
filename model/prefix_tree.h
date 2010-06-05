#ifndef PREFIX_TREE_H
#define PREFIX_TREE_H

#include "..\common_arith\common_arith.h"

class PrefixTree {
public:
	PrefixTree();

	class Node {
	public:
		Node(const BYTE);
		unsigned short rescale();
		unsigned short selfCount;
		unsigned short leftCount;
		Node *left, *right;
		BYTE data;
	};

	Node *rootnode;
	unsigned short totalCount;
};

#endif