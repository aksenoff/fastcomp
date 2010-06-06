#ifndef TREE_H
#define TREE_H
/*
A tree holding probabilities for bytes encountered in input stream

*/
#include "..\common_arith\common_arith.h"
#include "node.h"
#include "model.h"

class Node;
class Model;

extern FILE *log_file;
extern Model model;

class Tree {
public: 
	Tree();
	void encode(short, SYMBOL*);
	void rescale();
	void get_scale(SYMBOL*);
    short decode(unsigned short, SYMBOL*);
	void insert(unsigned short);
	unsigned short calculate_escape();
	Node *rootNode;
	unsigned short totalCount; // total number of symbols encoded
	unsigned short numNodes; // number of different symbols encoded
};

#endif