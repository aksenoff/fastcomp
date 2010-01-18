#ifndef DECODER_H
#define DECODER_H

#include<stdio.h>
#define ESCAPE 256
#define DONE -1

typedef struct {
                unsigned long int low_count;
                unsigned long int high_count;
                unsigned long int scale;
               } SYMBOL;

extern long underflow_bits;    /* The present underflow count in  */
                               /* the arithmetic coder.           */
/*
 * Function prototypes.
 */
void initialize_arithmetic_decoder( FILE *stream );
void remove_symbol_from_stream( FILE *stream, SYMBOL *s );
long get_current_count( SYMBOL *s);

#endif