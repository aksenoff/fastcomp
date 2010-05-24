#ifndef DECODER_H
#define DECODER_H

#include "..\common_arith\common_arith.h"

extern long underflow_bits;
void initialize_arithmetic_decoder( FILE *stream );
void remove_symbol_from_stream( FILE *stream, SYMBOL *s );
unsigned short get_current_count( SYMBOL *s);

#endif