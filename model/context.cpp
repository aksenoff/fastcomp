#include "context.h"

Context::Context(const BYTE d)
	: selfCount(1), leftCount(0), data(d), left(0), right(0) {};