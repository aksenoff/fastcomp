#ifndef TREE_H
#define TREE_H
/*
A tree holding probabilities for bytes encountered in input stream

*/
#include "..\common_arith\common_arith.h"

class Tree {
public: 
	Tree();
	bool encode(BYTE, SYMBOL*);
	void rescale();
	void get_scale(SYMBOL*);
    short decode(unsigned short, SYMBOL*);
	void insert(unsigned short);
	unsigned short calculate_escape();

	class Node {
	public:
		Node(const BYTE);
		unsigned short rescale();
		unsigned short selfCount;
		unsigned short leftCount;
		Node *left, *right;
		BYTE data;
	};

	Node *rootNode;
	unsigned short totalCount; // total number of symbols encoded
	unsigned short numNodes; // number of different symbols encoded
};

#endif