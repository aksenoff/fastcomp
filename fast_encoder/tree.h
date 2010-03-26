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
	bool encode(short, SYMBOL*);
	void rescale();

	class Node {
	public:
		//interface declarations
		Node();
		Node(const Node &n);
		Node(const BYTE);
		Node& operator=(const Node&);
		~Node();
		Node* createChild(const BYTE, Tree*);
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

void initialize_model();
bool get_symbol(short, SYMBOL *);

#endif