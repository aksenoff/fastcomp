#ifndef TREE_H
#define TREE_H
/*
A tree holding probabilities for bytes encountered in input stream

*/
#include "encoder.h"
typedef unsigned char BYTE;


class Tree {
public:
	//interface declarations	 
	Tree();
	Tree(const Tree&);
	Tree(const BYTE);
	Tree& operator=(const Tree&);
	~Tree();

	class Node {
	public:
		//interface declarations
		Node();
		Node(const Node &n);
		Node(const BYTE);
		Node& operator=(const Node&);
		~Node();
		Node* createChild(const BYTE, Tree*);
		long selfCount;
		long leftCount;
		Node *left, *right;
		BYTE data;
	};

	class iterator {
	public:
		//interface declarations
		iterator();
		iterator(Tree*, const BYTE);
		iterator(const iterator&);
		~iterator();
		void operator[](const BYTE);
		Node *node;
		Tree *tree;
		long totalRightCount;
		long totalLeftCount;
		long oldLeftCount;
		long selfCount;
	};

	Node *rootNode;
	long totalCount; // total number of symbols encoded
	short numNodes; // number of different symbols encoded
};

void initialize_model();

bool get_symbol(BYTE d, SYMBOL* s);

#endif