#include "tree.h"

//=====================================
//Tree member functions definitions:

Tree::Tree()
	: totalCount(0), numNodes(0), rootNode(0) {};

//-------------------------------------

unsigned short Tree::calculate_escape()
{
	unsigned short nN = numNodes-1;
	if(model.nodeAdded) 
	{
		nN--;
		//model.nodeAdded = false;
	}
	return(((256 - nN) * nN) / (256 * (rootNode->selfCount)) + 1);
}

//--------------------------------------

void Tree::encode(short ds, SYMBOL* s)
{
	if (ds == DONE) // we have eof here :)
	{
		model.escaped = true;
        if(!rootNode) 
	    {
		    s->low_count = 0;
		    s->high_count = 1;
		    s->scale = 1;
            return;
	    }
        else
        {
		    s->low_count = totalCount;
		    s->scale = s->high_count = s->low_count + calculate_escape();
		    return;
        }
	}
	BYTE d = static_cast<BYTE>(ds);
	if(!rootNode) 
	{
		rootNode = new Node(d);
		numNodes++;
		totalCount++;
		s->low_count = 0;
		s->high_count = 1;
		s->scale = 1;
        if(model.escaped) model.node->prev = rootNode;
        model.node = rootNode;
		model.nodeAdded = model.escaped =  true;
	}
	else 
	{
        model.nodeAdded = false;
        Node *node = rootNode;
		unsigned short escape_count = 0, totalRightCount = 0, 
			totalLeftCount = 0, oldLeftCount = 0, selfCount = 0;
		if (totalCount == 0x3FFF) rescale();
		totalRightCount = totalCount;
	    totalCount++;
#ifdef DBG_PROB
		fprintf(log_file, "enc: Total %d\n", totalCount);
#endif
		//descending
		oldLeftCount = totalLeftCount = node->leftCount;
		selfCount = node->selfCount;
		totalRightCount -= selfCount + totalLeftCount;
		while(node->data != d)
		{
			if(node->data > d)
			{
				node->leftCount++;
				if(node->left) 
				{
					node = node->left;
					totalRightCount += selfCount + oldLeftCount - node->selfCount - node->leftCount;
					totalLeftCount += node->leftCount - oldLeftCount;
					selfCount = node->selfCount;
					oldLeftCount = node->leftCount;
				}
				else 
				{
					node->left = new Node(d);
					numNodes++;
                    node = node->left;
					model.nodeAdded = true;
					break;
				}				
			}
			else if(node->right)
				 {
					 node = node->right;
					 totalRightCount -= node->selfCount + node->leftCount;
					 totalLeftCount += selfCount + node->leftCount;
					 selfCount = node->selfCount;
					 oldLeftCount = node->leftCount;
				 }
				 else 
				 {
					 node->right = new Node(d);
					 numNodes++;
                     node = node->right;
					 model.nodeAdded = true;
					 break;
				 }
		}
		//end of descending
        if(model.escaped) model.node->prev = node;  // we fellback from a higher order context
        model.node = node;
		escape_count = calculate_escape();
#ifdef DBG_PROB
		fprintf(log_file,"enc: Esc %d\n",escape_count);
#endif
		if(!model.nodeAdded)
		{
            model.escaped = false;
			node->selfCount++;  //'cause if escape happened this is done in Node::Node(BYTE)
			//if(node==rootNode) model.rootModified = true;
			s->low_count = totalLeftCount;
			s->high_count = totalLeftCount + selfCount;
			s->scale = totalLeftCount + selfCount + 
						totalRightCount + escape_count;
#ifdef DBG_PROB
		fprintf(log_file,"enc: %d %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
#endif
		}
		else
		{
			model.escaped = true;
            s->low_count = totalCount-1;
			s->scale = s->high_count = s->low_count + escape_count;
#ifdef DBG_PROB
		fprintf(log_file,"enc: %d %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
#endif
		}
	}
}

//----------------------------------------

void Tree::get_scale(SYMBOL* s)
{	
	if(rootNode)
		s->scale = totalCount + calculate_escape();
	else s->scale = 1;
}

//------------------------------------------

void Tree::insert(unsigned short count)
{
	BYTE d = (BYTE)count;
#ifdef DBG_PROB
	fprintf(log_file,"insert: Tree<-%d\n",d);
#endif
	if(!rootNode) 
	{
		rootNode = new Node(d);
		numNodes++;
		totalCount++;
	}
	else 
	{	
		unsigned short totalRightCount = 0, totalLeftCount = 0,
					oldLeftCount = 0, selfCount = 0;
		Node* node = rootNode;
	    totalRightCount = totalCount;
	    totalCount++;		
#ifdef DBG_PROB
	fprintf(log_file, "insert: Total %d\n", totalCount);
#endif
		//descending
		oldLeftCount = totalLeftCount = node->leftCount;
		selfCount = node->selfCount;
		totalRightCount -= selfCount + totalLeftCount;
		while(node->data != d)
		{
			if(node->data > d)
			{
				node->leftCount++;
				if(node->left) 
				{
					node = node->left;
					totalRightCount += selfCount + oldLeftCount - node->selfCount - node->leftCount;
					totalLeftCount += node->leftCount - oldLeftCount;
					selfCount = node->selfCount;
					oldLeftCount = node->leftCount;
				}
				else 
				{
				    node->left = new Node(d);
				    numNodes++;
				    break;
				}				
			}
			else if(node->right)
				 {
					 node = node->right;
					 totalRightCount -= node->selfCount + node->leftCount;
					 totalLeftCount += selfCount + node->leftCount;
					 selfCount = node->selfCount;
					 oldLeftCount = node->leftCount;
				 }
				 else 
				 {
					 node->right = new Node(d);
				     numNodes++;
					 break;
				 }
		}
		//done descending
		if (totalCount == 0x3FFF) rescale();
	}
}

//-----------------------------------------

short Tree::decode(unsigned short count, SYMBOL *s)
{
	if(!rootNode)
	{
		s->low_count = 0;
		s->high_count = 1;
		model.escaped = true;
		return ESCAPE;
	}
	else if(count >= totalCount)
	{
		s->low_count = totalCount;
		s->scale = s->high_count = s->low_count + calculate_escape();
		model.escaped = true;
#ifdef DBG_PROB
		fprintf(log_file,"dec: Esc %d:%d/%d\n",s->low_count,s->high_count,s->scale);
#endif
		return ESCAPE;
	}
	else
	{
		unsigned short totalRightCount = 0, totalLeftCount = 0,
			oldLeftCount = 0, selfCount = 0;
#ifdef DBG_PROB
		fprintf(log_file, "dec: srch for %d\n", count);
#endif
		Node* node = rootNode;
		totalCount++;
		totalRightCount = totalCount;
#ifdef DBG_PROB
		fprintf(log_file, "dec: Total %d\n", totalCount);
#endif
		oldLeftCount = totalLeftCount = node->leftCount;
		selfCount = node->selfCount;
		totalRightCount -= selfCount + totalLeftCount;
		while(!((totalLeftCount <= count) && ((totalLeftCount+selfCount) > count)))
		{
			if(totalLeftCount > count)
			{
				node->leftCount++;
				node = node->left;
				totalRightCount += selfCount + oldLeftCount - node->selfCount - node->leftCount;
				totalLeftCount += node->leftCount - oldLeftCount;
				selfCount = node->selfCount;
				oldLeftCount = node->leftCount;
			}
			else {
				 node = node->right;
				 totalRightCount -= node->selfCount + node->leftCount;
				 totalLeftCount += selfCount + node->leftCount;
				 selfCount = node->selfCount;
				 oldLeftCount = node->leftCount;
		     }
		}
		node->selfCount++;
		if (totalCount == 0x3FFF) rescale();
#ifdef DBG_PROB
		fprintf(log_file,"dec: byte %d\n",node->data);
#endif
		s->low_count = totalLeftCount;
		s->high_count = totalLeftCount + selfCount;
#ifdef DBG_PROB
		fprintf(log_file,"dec: s %d:%d/%d\n",s->low_count,s->high_count,s->scale);
#endif
		return node->data;
	}
}

//---------------------------------------

void Tree::rescale()
{
#ifdef DBG_PROB
	fprintf(log_file,"Rescaling\n");
#endif
	totalCount = rootNode->rescale();
}

