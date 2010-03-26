#include <cstdio>
#include "tree.h"
#include "decoder.h"
#include "bitio.h"
#include <cstdlib>
#include <cstring>

using namespace std;
FILE *log_file;

int main()
{
	FILE *text_file;
    FILE *compressed_file;
	compressed_file = fopen( "test.cmp", "rb" );
    text_file = fopen( "test.unc", "wb" );
	log_file = fopen("log_unc.txt","wt");
	setvbuf( text_file, NULL, _IOFBF, 4096 ); //??
	SYMBOL s;
	unsigned short count;
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
			if(byte==DONE)
				break;
		} while(byte==ESCAPE);
		if(byte==DONE)
			break;
#ifdef DBG
		fprintf(log_file,"main: Writing %d\n",byte);
#endif
		putc( (BYTE) byte, text_file );
	}
	return 0;
}