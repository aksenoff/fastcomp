#ifndef TREE_H
#define TREE_H
/*
A tree holding probabilities for bytes encountered in input stream

*/
#include "decoder.h"
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
		unsigned short selfCount;
		unsigned short leftCount;
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
		unsigned short totalRightCount;
		unsigned short totalLeftCount;
		unsigned short oldLeftCount;
		unsigned short selfCount;
	};

	Node *rootNode;
	unsigned short totalCount; // total number of symbols encoded
	unsigned short numNodes; // number of different symbols encoded
};

void initialize_model();

bool get_symbol(BYTE, SYMBOL*);

void get_scale(SYMBOL*);

short get_byte(unsigned short, SYMBOL*);

#endif