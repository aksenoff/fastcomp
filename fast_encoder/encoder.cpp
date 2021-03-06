#include <cstdio>
#include "encoder.h"
#include "..\bitio\bitio.h"

using namespace std;

static unsigned short code; 
static unsigned short low; 
static unsigned short high;
long underflow_bits; 
extern FILE *log_file;

void initialize_arithmetic_encoder()
{
    low = 0;
    high = 0xffff;
    underflow_bits = 0;
}

void encode_symbol( FILE *stream, SYMBOL *s )
{
    unsigned long range;
    range = (unsigned long) ( high-low ) + 1;
    high = low + (unsigned short)
                 (( range * s->high_count ) / s->scale - 1 );
    low = low + (unsigned short)
                 (( range * s->low_count ) / s->scale );
#ifdef DBG_ENC
	fprintf(log_file,"enc_s: L %d H %d\n",low,high);
#endif
    for ( ; ; )
    {
        if ( ( high & 0x8000 ) == ( low & 0x8000 ) )
        {
            output_bit( stream, high & 0x8000 );
#ifdef DBG_ENC
		fprintf(log_file,"enc_s: Shifting out %d\n",high>>15);
#endif
            while ( underflow_bits > 0 )
            {
                output_bit( stream, ~high & 0x8000 );
#ifdef DBG_ENC
		fprintf(log_file,"enc_s: Shifting out u.f. %d\n",(~high & 0x8000)>>15);
#endif
                underflow_bits--;
            }
        }
        else if ( ( low & 0x4000 ) && !( high & 0x4000 ))
        {
            underflow_bits += 1;
            low &= 0x3fff;
            high |= 0x4000;
#ifdef DBG_ENC
		fprintf(log_file,"enc_s: Uflow? L %d H %d\n",low,high);
#endif
        }
        else
            return ;
        low <<= 1;
        high <<= 1;
        high |= 1;
#ifdef DBG_ENC
		fprintf(log_file,"enc_s: SHL L %d H %d\n",low,high);
#endif
    }
}

void flush_arithmetic_encoder( FILE *stream )
{
    output_bit( stream, low & 0x4000 );
#ifdef DBG_ENC
		fprintf(log_file,"flush_a_e: Shifting out %d\n", low & 0x4000>>14);
#endif
    underflow_bits++;
    while ( underflow_bits-- > 0 )
	{
#ifdef DBG_ENC
		fprintf(log_file,"flush_a_e: Shifting out u.f. %d\n",~low & 0x4000 >>14);
#endif
        output_bit( stream, ~low & 0x4000 );
	}
}