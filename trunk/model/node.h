#ifndef NODE_H
#define NODE_H

#include "..\common_arith\common_arith.h"
#include "context.h"

class Context;

class Node {
public:
	Node(const BYTE);
	unsigned short rescale();
	unsigned short selfCount;
	unsigned short leftCount;
	Node *left, *right;
	BYTE data;
	Context *next;
	Node *prev;
};

#endif