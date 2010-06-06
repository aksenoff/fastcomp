#include "model.h"

extern FILE *log_file;

//===================================
//Model member functions definitions:

Model::Model()
    : zc(new ZeroContext), newTree(false), escaped(false), nodeAdded(false) {};

//-----------------------------------

void Model::encode_in_null_table(short ds, SYMBOL* s)
{
	if(ds == -1) ds = 256;
	s->low_count = ds;
	s->high_count = ds + 1;
	s->scale = 257;
}

//-----------------------------------

void Model::get_symbol(short ds, SYMBOL *s)
{
	/*BYTE d = (BYTE)ds;
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
	}
	else t->encode(d, s);*/
	//BYTE d = BYTE(ds);
	if(escaped)
	{
		newTree = escaped = false;
		if(currentContext == 0) 
		{
			encode_in_null_table(ds, s);
			return;
		}
		else
		{
			currentContext = currentContext->suff;
			currentContext->probTree->encode(ds, s);
		}
	}
	else 
	{
		if(currentContext == 0) zc->probTree->encode(ds, s);
		else currentContext->probTree->encode(ds, s);
	}
	if(!escaped) //can't merge with previous else 'cos encode() may change escaped
	{
		// we successfully encoded not in -1st context
		BYTE d = (BYTE)ds;
		if(node->next) currentContext = node->next;
		else
		{
			ctxt = node->prev->next;
			currentContext = ctxt->prefix->addPrefix(d); // not implemented!
			currentContext->suff = ctxt;
			node->next = currentContext;
		}
	}
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
	/*if(!escaped) return t->decode(count, s);
	escaped = false;
	s->low_count = count;
	s->high_count = count + 1;
	t->insert(count);
	return count;*/
}

