#include <cstdio>
#include "tree.h"
#include "encoder.h"
#include "bitio.h"
#include <cstdlib>
#include <cstring>

using namespace std;

int main(int argc, char** argv)
{
	FILE *text_file;
    FILE *compressed_file;
	bool escaped;
	initialize_model();
	initialize_output_bitstream();
    initialize_arithmetic_encoder();
	//std::ifstream is(argv[1]);
	text_file = fopen( "test.inp", "rb" );
    compressed_file = fopen( "test.cmp", "wb" );
	setvbuf( text_file, NULL, _IOFBF, 4096 );
	SYMBOL s;
	short d;
	while(1)
	{
		//is.get(d);
		d = getc(text_file);
		if(d==EOF) break;
		do { 
			escaped = get_symbol((BYTE)d, &s);
			encode_symbol(compressed_file, &s);
		} while(escaped);
	}
	flush_arithmetic_encoder( compressed_file );
    flush_output_bitstream( compressed_file );
	return 0;
}