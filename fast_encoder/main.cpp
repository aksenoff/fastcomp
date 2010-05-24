#include "..\model\model.h"
#include "..\bitio\bitio.h"
#include "encoder.h"

FILE *log_file;
Model model;

int main(int argc, char** argv)
{
	FILE *text_file;
    FILE *compressed_file;
	initialize_output_bitstream();
    initialize_arithmetic_encoder();
	text_file = fopen( "test.inp", "rb" );
    compressed_file = fopen( "test.cmp", "wb" );
	log_file = fopen("log_c.txt","wt");
	setvbuf( text_file, NULL, _IOFBF, 4096 );
	SYMBOL s;
	short d;
	while(1)
	{
		d = getc(text_file);
		do { 
#ifdef DBG
		fprintf(log_file,"main: Cmp %d\n",d);
#endif
			model.escaped = model.get_symbol(d, &s);
			encode_symbol(compressed_file, &s);
			if(d==DONE) break;
		} while(model.escaped);
		if(d==DONE) 
			break;
	}
	flush_arithmetic_encoder( compressed_file );
    flush_output_bitstream( compressed_file );
	return 0;
}