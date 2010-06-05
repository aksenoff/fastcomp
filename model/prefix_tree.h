#ifndef PREFIX_TREE_H
#define PREFIX_TREE_H

#include "..\common_arith\common_arith.h"
#include "context.h"

class Context;

class PrefixTree {
public:
	PrefixTree();
	Context *rootNode;
	unsigned short totalCount;
};

#endif