/*********************************************************************************************************
** File name               :   glist.c
** Date created		   			 :   2018-07-01
** Version Latest          :   V1.0.0
** Author Latest           :   core    
** File Description        :   LIST
*********************************************************************************************************
** Development Process**
	(1) glist.c (V1.0.0)	core	2018-07-01
	
	 
**********************************************************************************************************/
#include "glib.h"

FuncErrorE gNodeInit (LIST_NODE_t * node)
{
    if(!node)
		return(PTR_NULL);
	node->nextNode = node;
    node->preNode = node;
	return(EXE_FINISH);
}

FuncErrorE gListInit (LIST_t * list)
{
    if(!list)
		return(PTR_NULL);
		list->firstNode = &(list->headNode);
    list->lastNode = &(list->headNode);
    list->nodeCount = 0;
	return(EXE_FINISH);
}

uint32_t gListCount (LIST_t * list)
{
	if(!list)
		return 0;
	return list->nodeCount;
}

LIST_NODE_t * gListFirst (LIST_t * list)
{
    LIST_NODE_t * node = (LIST_NODE_t *)0;
	if(!list)
		return NULL;
	if (list->nodeCount != 0) 
	{
		node = list->firstNode;
	}    
    return  node;
}

LIST_NODE_t * gListLast (LIST_t * list)
{
    LIST_NODE_t * node = (LIST_NODE_t *)0;
	if(!list)
		return NULL;
	if (list->nodeCount != 0) 
	{
		node = list->lastNode;
	}    
    return  node;
}

LIST_NODE_t * gListPre (LIST_t * list, LIST_NODE_t * node)
{
	if(!(list && node))
		return NULL;
	if (node->preNode == node) 
	{
		return (LIST_NODE_t *)0;
	} 
	else 
	{
		return node->preNode;
	}
}

LIST_NODE_t * gListNext (LIST_t * list, LIST_NODE_t * node)
{
	if(!(list && node))
		return NULL;
	if (node->nextNode == node) 
	{
		return (LIST_NODE_t *)0;
	}
	else 
	{
		return node->nextNode;
	}
}

FuncErrorE gListRemoveAll (LIST_t * list)
{
    uint32_t count;
    LIST_NODE_t * nextNode;
    if(!list)
		return(PTR_NULL);
    nextNode = list->firstNode;
    for (count = list->nodeCount; count != 0; count-- )
    {
        LIST_NODE_t * currentNode = nextNode;
        nextNode = nextNode->nextNode;
       
       	gNodeInit(currentNode);
    }
    gListInit (list);
	return(EXE_FINISH);
}

FuncErrorE gListAddFirst (LIST_t * list, LIST_NODE_t * node)
{
    if(!(list && node))
		return(PTR_NULL);
		node->preNode = list->firstNode->preNode;
    node->nextNode = list->firstNode;

    list->firstNode->preNode = node;
    list->firstNode = node;
    list->nodeCount++;
	return(EXE_FINISH);
}

FuncErrorE gListAddLast (LIST_t * list, LIST_NODE_t * node)
{
    if(!(list && node))
		return(PTR_NULL);
		node->nextNode = &(list->headNode);
    node->preNode = list->lastNode;

    list->lastNode->nextNode = node;
    list->lastNode = node;
    list->nodeCount++;
	return(EXE_FINISH);
}

LIST_NODE_t * gListRemoveFirst (LIST_t * list)
{
    LIST_NODE_t * node = (LIST_NODE_t *)0;
	if(!list)
		return NULL;
    if( list->nodeCount != 0 )
    {
        node = list->firstNode;

        node->nextNode->preNode = &(list->headNode);
        list->firstNode = node->nextNode;
        list->nodeCount--;
    }
    return  node;
}

FuncErrorE gListInsertAfter (LIST_t * list, LIST_NODE_t * nodeAfter,  LIST_NODE_t * nodeToInsert)
{
    if(!(list && nodeAfter && nodeToInsert))
		return(PTR_NULL);
	nodeToInsert->preNode = nodeAfter;
    nodeToInsert->nextNode = nodeAfter->nextNode;

    nodeAfter->nextNode->preNode = nodeToInsert;
    nodeAfter->nextNode = nodeToInsert;

    list->nodeCount++;
	return(EXE_FINISH);
}
 FuncErrorE gIsNodeInList(LIST_t *list,LIST_NODE_t *node)
{
	LIST_NODE_t *it;
	if(!(list && node))
		return(PTR_NULL);
	uint32_t cntcpy =gListCount(list);
	for(it=(list->firstNode);cntcpy > 0;it = it->nextNode,cntcpy--) 
	{
		if(it == node)
			return(EXE_FINISH);
	}
	return(EXE_OTHERS);
}
FuncErrorE gListNodeRemove (LIST_t * list, LIST_NODE_t * node)
{
    if(!(list && node))
		return(PTR_NULL);
	node->preNode->nextNode = node->nextNode;
    node->nextNode->preNode = node->preNode;
    list->nodeCount--;
		gNodeInit (node);
	return(EXE_FINISH);
}
