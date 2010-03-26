#ifndef DECODER_H
#define DECODER_H

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

extern long underflow_bits;    /* The present underflow count in  */
                               /* the arithmetic coder.           */
/*
 * Function prototypes.
 */
void initialize_arithmetic_decoder( FILE *stream );
void remove_symbol_from_stream( FILE *stream, SYMBOL *s );
unsigned short get_current_count( SYMBOL *s);

#endif