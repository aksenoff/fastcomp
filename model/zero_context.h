#ifndef ZERO_CONTEXT_H
#define ZERO_CONTEXT_H

#include "tree.h"
#include "prefix_tree.h"

class Tree;
class PrefixTree;

class ZeroContext {
public:
	Tree *probTree;
	PrefixTree *prefix;
};

#endif