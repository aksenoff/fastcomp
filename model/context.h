#ifndef CONTEXT_H
#define CONTEXT_H

#include "..\common_arith\common_arith.h"
#include "tree.h"
#include "prefix_tree.h"

class Tree;

class Context {
public:
	Context(const BYTE);
	Context *left, *right;
	BYTE data;
	Tree *probTree;
	PrefixTree *prefix;
	Context *suff;
};

#endif