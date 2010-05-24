#ifndef MODEL_H
#define MODEL_H

#include "tree.h"

class Model {
public:
	Tree *t;
	bool escaped, newTree, nodeAdded, rootModified;
	Model();
	bool get_symbol(short, SYMBOL*);
	void get_scale(SYMBOL*);
    short get_byte(unsigned short, SYMBOL*);
	static void encode_in_null_table(BYTE d, SYMBOL* s);
};

#endif