#ifndef COMMON_ARITH_H
#define COMMON_ARITH_H

#include<cstdio>

#define ESCAPE 256
#define DONE -1

#ifndef DBG
#define DBG
#endif

using namespace std;

typedef unsigned char BYTE;

typedef struct {
                unsigned short low_count;
                unsigned short high_count;
                unsigned short scale;
               } SYMBOL;

#endif