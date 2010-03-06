#include <stdio.h>
#include "decoder.h"
#include "bitio.h"

extern FILE* log_file;

static unsigned short int code;  /* The present input code value       */
static unsigned short int low;   /* Start of the current code range    */
static unsigned short int high;  /* End of the current code range      */
long underflow_bits;             /* Number of underflow bits pending   */
/*
 * When decoding, this routine is called to figure out which symbol
 * is presently waiting to be decoded.  This routine expects to get
 * the current model scale in the s->scale parameter, and it returns
 * a count that corresponds to the present floating point code:
 *
 *  code = count / s->scale
 */
unsigned long get_current_count( SYMBOL *s)
{
    unsigned long range;
    long count;
	
    range = (unsigned long) ( high - low ) + 1;
	count = (long)
            ((((long) ( code - low ) + 1 ) * s->scale-1 ) / range );
#ifdef DBG
		fprintf(log_file,"get_current_count: Current count is %d\n",count);
#endif
	return( count );
}
/*
 * This routine is called to initialize the state of the arithmetic
 * decoder.  This involves initializing the high and low registers
 * to their conventional starting values, plus reading the first
 * 16 bits from the input stream into the code value.
 */
void initialize_arithmetic_decoder(FILE *stream )
{
    int i;

    code = 0;
    for ( i = 0 ; i < 16 ; i++ )
    {
        code <<= 1;
		code += input_bit( stream );
    }
    low = 0;
    high = 0xffff;
#ifdef DBG
		fprintf(log_file,"initialize_arithmetic_decoder: Current code is %d\n",code);
#endif
}

/*
 * Just figuring out what the present symbol is doesn't remove
 * it from the input bit stream.  After the character has been
 * decoded, this routine has to be called to remove it from the
 * input stream.
 */
void remove_symbol_from_stream( FILE *stream, SYMBOL *s )
{
    long range;

/*
 * First, the range is expanded to account for the symbol removal.
 */
    range = (long)( high - low ) + 1;
    high = low + (unsigned short int)
                 (( range * s->high_count ) / s->scale - 1 );
    low = low + (unsigned short int)
                 (( range * s->low_count ) / s->scale );
#ifdef DBG
		fprintf(log_file,"remove_symbol_from_stream: Low <- %d High <- %d\n",low,high);
#endif
/*
 * Next, any possible bits are shipped out.
 */
    for ( ; ; )
    {
/*
 * If the MSDigits match, the bits will be shifted out.
 */
        if ( ( high & 0x8000 ) == ( low & 0x8000 ) )
        {
        }
/*
 * Else, if underflow is threatining, shift out the 2nd MSDigit.
 */
        else if ((low & 0x4000) == 0x4000  && (high & 0x4000) == 0 )
        {
            code ^= 0x4000;
            low   &= 0x3fff;
            high  |= 0x4000;
#ifdef DBG
		fprintf(log_file,"remove_symbol_from_stream: Underflow suspected. Low <- %d High <- %d\n",low,high);
#endif
        }
/*
 * Otherwise, nothing can be shifted out, so I return.
 */
        else
            return;
        low <<= 1;
        high <<= 1;
        high |= 1;
#ifdef DBG
		fprintf(log_file,"remove_symbol_from_stream: Low and high are shifted left. Low: %d High: %d\n",low,high);
#endif
        code <<= 1;
        code += input_bit( stream );
#ifdef DBG
		fprintf(log_file,"remove_symbol_from_stream: New bit is shifted into code. Code: %d\n",code);
#endif
    }
}

