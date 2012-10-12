#include "model.h"

extern FILE *log_file;

//===================================
//Model member functions definitions:

Model::Model()
    : zc(new ZeroContext), newTree(false), escaped(false), nodeAdded(false), node(0), numsymbols(0) {};

//-----------------------------------

void Model::encode_in_null_table(short ds, SYMBOL* s)
{
    /*newTree = */escaped = false;
    if(ds == DONE) ds = 256; // if EOF, escape from null table
	s->low_count = ds;
	s->high_count = ds + 1;
	s->scale = 257;
 
}

//-----------------------------------

void Model::get_symbol(short ds, SYMBOL *s)
{
	if(escaped)
	{
#ifdef DBG_MODEL
		fprintf(log_file, "get_s: Esc from ord %d\n", order);
#endif
		//newTree = escaped = false;
		if(order == 0) 
		{
#ifdef DBG_MODEL
		fprintf(log_file, "get_s: Enc in nt\n");
#endif
			encode_in_null_table(ds, s);
			return;
		}
		else if(order == 1)
        {
#ifdef DBG_MODEL
		fprintf(log_file, "get_s: Enc in 0 ctxt\n");
#endif
            zc->probTree->encode(ds, s);
            order = 0;
        }
        else
		{
#ifdef DBG_MODEL
		fprintf(log_file, "get_s: Enc in suffix (ord %d)\n", order-1);
#endif
			currentContext = currentContext->suff;
			currentContext->probTree->encode(ds, s);
            --order;
		}
	}
	else 
	{
#ifdef DBG_MODEL
		fprintf(log_file, "get_s: Trying to encode, ord %d\n", order);
#endif
		if(order == 0) zc->probTree->encode(ds, s);
		else currentContext->probTree->encode(ds, s);
	}
	if(!escaped) //can't merge with previous else 'cos encode() may change escaped
	{
#ifdef DBG_MODEL
		fprintf(log_file, "get_s: Success, shifting\n");
#endif
		// we successfully encoded
		BYTE d = static_cast<BYTE>(ds);
        if(node->next) // we encounter symbol in this context for the 3rd time or more
        {
            currentContext = node->next;
            ++order;
#ifdef DBG_MODEL
		fprintf(log_file, "get_s: Goin to ord %d by next\n", order);
#endif
        }
		else // we encounter symbol in this context for the 2nd time
		{
            /*if(order == 0)
            {
                node->next = zc->prefix->addPrefix(d);
            }
            else
            {*/
#ifdef DBG_MODEL
		fprintf(log_file, "get_s: No next ptr\n");
#endif
                //Node *tempNode = node;
                while(node->prev)
                {
                    if(node->prev->next)
                    {
#ifdef DBG_MODEL
		fprintf(log_file, "get_s: Goin prev->next\n");
#endif
                        ctxt = currentContext;
                        currentContext = node->prev->next;
                        ctxt = currentContext->prefix->addPrefix(ctxt->data);
			            ctxt->suff = currentContext;
			            node->next = ctxt;
                        break;
                    }
                    else
                    {
#ifdef DBG_MODEL
		fprintf(log_file, "get_s: Down to ord %d\n", order);
#endif
                        node = node->prev;
                        --order;
                    }
                }
                if(order == 0)
                {
#ifdef DBG_MODEL
		fprintf(log_file, "get_s: Reached 0\n");
#endif
                    node->next = zc->prefix->addPrefix(d);
                }
            //}
		}
    }
}

//------------------------------------

void Model::get_scale(SYMBOL* s)
{
	/*if(escaped) s->scale = 257;
	else t->get_scale(s);
#ifdef DBG
	fprintf(log_file,"get_sc: Scale %d\n",s->scale);
#endif*/
}

//------------------------------------

short Model::get_byte(unsigned short count, SYMBOL *s)
{
	/*if(!escaped) return t->decode(count, s);
     if(count==256) return DONE;
	escaped = false;
	s->low_count = count;
	s->high_count = count + 1;
	t->insert(count);
	return count;*/
    return 0;
}

