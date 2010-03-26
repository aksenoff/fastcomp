#ifndef ENCODER_H
#define ENCODER_H

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

extern long underflow_bits;

void initialize_arithmetic_encoder();
void encode_symbol( FILE *stream, SYMBOL *s );
void flush_arithmetic_encoder( FILE *stream );

#endif