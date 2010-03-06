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
		unsigned long selfCount;
		unsigned long leftCount;
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
		unsigned long totalRightCount;
		unsigned long totalLeftCount;
		unsigned long oldLeftCount;
		unsigned long selfCount;
	};

	Node *rootNode;
	unsigned long totalCount; // total number of symbols encoded
	unsigned short numNodes; // number of different symbols encoded
};

void initialize_model();

bool get_symbol(BYTE, SYMBOL*);

void get_scale(SYMBOL*);

long get_byte(unsigned long, SYMBOL*);

#endif