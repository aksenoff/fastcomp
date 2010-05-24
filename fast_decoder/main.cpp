#include "..\model\model.h"
#include "..\bitio\bitio.h"
#include "decoder.h"

FILE *log_file;
Model model;

int main(int argc, char** argv)
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
    initialize_input_bitstream();
    initialize_arithmetic_decoder( compressed_file );
	while(1)
	{
		do { 
			model.get_scale(&s); // symbols defined + num of escapes
			count = get_current_count(&s);
			byte = model.get_byte(count, &s);
			remove_symbol_from_stream( compressed_file, &s );
			if(byte==DONE)
				break;
		} while(byte==ESCAPE);
		if(byte==DONE)
			break;
#ifdef DBG
		fprintf(log_file,"main: Wr %d\n",byte);
#endif
		putc( (BYTE) byte, text_file );
	}
	return 0;
}