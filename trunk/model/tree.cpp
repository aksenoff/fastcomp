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
		model.nodeAdded = false;
	}
	return(((256 - nN) * nN) / (256 * (rootNode->selfCount)) + 1);
}

//--------------------------------------

void Tree::encode(short ds, SYMBOL* s)
{
	if (ds == -1) // we have eof here :)
	{
		model.escaped = true;
		s->low_count = totalCount;
		s->scale = s->high_count = s->low_count + calculate_escape();
		return;
	}
	BYTE d = (BYTE)ds;
	if(!rootNode) 
	{
		rootNode = new Node(d);
		model.escaped = true;
		numNodes++;
		totalCount++;
		s->low_count = 0;
		s->high_count = 1;
		s->scale = 1;
	}
	else 
	{
		Node *node = rootNode;
		unsigned short escape_count = 0, totalRightCount = 0, 
			totalLeftCount = 0, oldLeftCount = 0, selfCount = 0;
		if (totalCount == 0x3FFF) rescale();
		totalRightCount = totalCount;
	    totalCount++;
#ifdef DBG
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
					model.nodeAdded = model.escaped = true;
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
					 model.nodeAdded = model.escaped = true;
					 break;
				 }
		}
		//end of descending
		model.node = node;
		escape_count = calculate_escape();
#ifdef DBG
		fprintf(log_file,"enc: Esc %d\n",escape_count);
#endif
		if(!model.escaped)
		{
			node->selfCount++;  //'cause if escape happened this is done in Node::Node(BYTE)
			//if(node==rootNode) model.rootModified = true;
			s->low_count = totalLeftCount;
			s->high_count = totalLeftCount + selfCount;
			s->scale = totalLeftCount + selfCount + 
						totalRightCount + escape_count;
#ifdef DBG
		fprintf(log_file,"enc: %d %d:%d/%d\n",d, s->low_count,s->high_count,s->scale);
#endif
		}
		else
		{
			s->low_count = totalCount-1;
			s->scale = s->high_count = s->low_count + escape_count;
#ifdef DBG
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
#ifdef DBG
	fprintf(log_file,"get_b: Tree<-%d\n",d);
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
#ifdef DBG
	fprintf(log_file, "get_b: Total %d\n", totalCount);
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
#ifdef DBG
		fprintf(log_file,"get_b: Esc %d:%d/%d\n",s->low_count,s->high_count,s->scale);
#endif
		return ESCAPE;
	}
	else
	{
		unsigned short totalRightCount = 0, totalLeftCount = 0,
			oldLeftCount = 0, selfCount = 0;
#ifdef DBG
		fprintf(log_file, "get_b: srch for %d\n", count);
#endif
		Node* node = rootNode;
		totalCount++;
		totalRightCount = totalCount;
#ifdef DBG
		fprintf(log_file, "get_b: Total %d\n", totalCount);
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
#ifdef DBG
		fprintf(log_file,"get_b: byte %d\n",node->data);
#endif
		s->low_count = totalLeftCount;
		s->high_count = totalLeftCount + selfCount;
#ifdef DBG
		fprintf(log_file,"get_b: s %d:%d/%d\n",s->low_count,s->high_count,s->scale);
#endif
		return node->data;
	}
}

//---------------------------------------

void Tree::rescale()
{
#ifdef DBG
	fprintf(log_file,"Rescaling\n");
#endif
	totalCount = rootNode->rescale();
}

