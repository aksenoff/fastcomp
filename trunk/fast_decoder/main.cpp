#include <cstdio>
#include "tree.h"
#include "decoder.h"
#include "bitio.h"
#include <cstdlib>
#include <cstring>

using namespace std;

int main()
{
	FILE *text_file;
    FILE *compressed_file;
	compressed_file = fopen( "test.cmp", "rb" );
    text_file = fopen( "test.unc", "wb" );
	setvbuf( text_file, NULL, _IOFBF, 4096 ); //??
	SYMBOL s;
	long count;
	short byte;
	initialize_model();
    initialize_input_bitstream();
    initialize_arithmetic_decoder( compressed_file );
	while(1)
	{
		do { 
			get_scale(&s); // найти кол-во символов вместе с эскейпами
			count = get_current_count(&s);
			byte = get_byte(count, &s);
			remove_symbol_from_stream( compressed_file, &s );
		} while(byte==ESCAPE);
		if(byte==DONE) break;
		putc( (BYTE) byte, text_file );
	}
	return 0;
}