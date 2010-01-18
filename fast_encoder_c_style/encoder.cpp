#include <cstdio>
#include "encoder.h"
#include "bitio.h"

using namespace std;

static unsigned short int code; 
static unsigned short int low; 
static unsigned short int high;
long underflow_bits; 

void initialize_arithmetic_encoder()
{
    low = 0;
    high = 0xffff;
    underflow_bits = 0;
}

void encode_symbol( FILE *stream, SYMBOL *s )
{
    long range;
    range = (long) ( high-low ) + 1;
    high = low + (unsigned short int )
                 (( range * s->high_count ) / s->scale - 1 );
    low = low + (unsigned short int )
                 (( range * s->low_count ) / s->scale );
    for ( ; ; )
    {
        if ( ( high & 0x8000 ) == ( low & 0x8000 ) )
        {
            output_bit( stream, high & 0x8000 );
            while ( underflow_bits > 0 )
            {
                output_bit( stream, ~high & 0x8000 );
                underflow_bits--;
            }
        }
        else if ( ( low & 0x4000 ) && !( high & 0x4000 ))
        {
            underflow_bits += 1;
            low &= 0x3fff;
            high |= 0x4000;
        }
        else
            return ;
        low <<= 1;
        high <<= 1;
        high |= 1;
    }
}

void flush_arithmetic_encoder( FILE *stream )
{
    output_bit( stream, low & 0x4000 );
    underflow_bits++;
    while ( underflow_bits-- > 0 )
        output_bit( stream, ~low & 0x4000 );
}