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
	//setvbuf( text_file, NULL, _IOFBF, 4096 );
	SYMBOL s;
	short d;
	do
	{
		d = getc(text_file);
        ++model.numsymbols;
#ifdef DBG
		fprintf(log_file,"main: Cmp %d\n",d);
#endif
        do { 
			model.get_symbol(d, &s);
			encode_symbol(compressed_file, &s);
		} while(model.escaped);
#ifdef DBG
        std::fflush(log_file);
        std::fflush(compressed_file);
#endif
	}
    while(d != DONE);
	flush_arithmetic_encoder( compressed_file );
    flush_output_bitstream( compressed_file );
	return 0;
}