#ifndef COMMON_ARITH_H
#define COMMON_ARITH_H
#include<stdio.h>
#define ESCAPE 256
#define DONE -1
#ifndef DBG
#define DBG
#endif
typedef struct {
                unsigned short low_count;
                unsigned short high_count;
                unsigned short scale;
               } SYMBOL;
#endif