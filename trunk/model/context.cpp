#include "context.h"

Context::Context(const BYTE d)
	: data(d), left(0), right(0), prefix(new PrefixTree) {};