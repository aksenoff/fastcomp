#ifndef MODEL_H
#define MODEL_H

#include "..\common_arith\common_arith.h"
#include "tree.h"
#include "prefix_tree.h"
#include "context.h"
#include "node.h"
#include "zero_context.h"

class PrefixTree;
class Tree;
class Context;
class Node;
class ZeroContext;

class Model {
public:
	Context *currentContext;
	bool escaped, newTree, nodeAdded;
	Model();
	void get_symbol(short, SYMBOL*);
	void get_scale(SYMBOL*);
    short get_byte(unsigned short, SYMBOL*);
	static void encode_in_null_table(BYTE d, SYMBOL* s);
	Context *ctxt;
	Node *node;
	ZeroContext *zc;
};

#endif