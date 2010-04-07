#ifndef ENCODER_H
#define ENCODER_H

#include "..\common_arith\common_arith.h"

extern long underflow_bits;
void initialize_arithmetic_encoder();
void encode_symbol( FILE *stream, SYMBOL *s );
void flush_arithmetic_encoder( FILE *stream );

#endif