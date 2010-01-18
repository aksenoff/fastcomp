#ifndef ENCODER_H
#define ENCODER_H

#include<stdio.h>
#define ESCAPE 256
#define DONE -1

typedef struct {
                unsigned long int low_count;
                unsigned long int high_count;
                unsigned long int scale;
               } SYMBOL;

extern long underflow_bits;

void initialize_arithmetic_encoder();
void encode_symbol( FILE *stream, SYMBOL *s );
void flush_arithmetic_encoder( FILE *stream );

#endif