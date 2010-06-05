#include "node.h"

//=====================================
//Tree::Node member functions definitions:

Node::Node(const BYTE d)
	: selfCount(1), leftCount(0), data(d), left(0), right(0) {};

//---------------------------------------

unsigned short Node::rescale()
{
	unsigned short tempRightCount = 0;
	if(selfCount != 1) selfCount >>= 1;
	if(left) leftCount = left->rescale();
	if(right) tempRightCount = right->rescale();
	return (selfCount + leftCount + tempRightCount);
}