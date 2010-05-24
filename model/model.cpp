#include "model.h"

extern FILE *log_file;

//===================================
//Model member functions definitions:

Model::Model()
    : t(new Tree), newTree(false), escaped(false), nodeAdded(false), rootModified(false) {};

//-----------------------------------

void Model::encode_in_null_table(BYTE d, SYMBOL* s)
{
	s->low_count = d;
	s->high_count = d + 1;
	s->scale = 256;
}

//-----------------------------------

bool Model::get_symbol(short ds, SYMBOL *s)
{
	BYTE d = (BYTE)ds;
	if(escaped)
	{
		newTree = escaped = false;
#ifdef DBG
		fprintf(log_file,"enc: Enc %d in nt\n",d);
#endif
		encode_in_null_table(d, s);
#ifdef DBG
		fprintf(log_file,"enc: %d %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
#endif
		return false;
	}
	return t->encode(d, s);
}

//------------------------------------

void Model::get_scale(SYMBOL* s)
{
	if(escaped) s->scale = 256;
	else t->get_scale(s);
#ifdef DBG
	fprintf(log_file,"get_sc: Scale %d\n",s->scale);
#endif
}

//------------------------------------

short Model::get_byte(unsigned short count, SYMBOL *s)
{
	if(!escaped) return t->decode(count, s);
	escaped = false;
	s->low_count = count;
	s->high_count = count + 1;
	t->insert(count);
	return count;
}

