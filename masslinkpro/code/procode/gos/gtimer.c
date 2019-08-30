/*********************************************************************************************************
** File name               :   gtimer.c
** Date created		   :   2018-07-01
** Version Latest          :   V1.0.0
** Author Latest           :   core    
** File Description        :   
*********************************************************************************************************
** Development Process**
	(1) gtimer.c (V1.0.0)	core	2018-07-01
	
	 
**********************************************************************************************************/

#include "glib.h"

static LIST_t timerlist;	
volatile uint32_t timerticks = 0;

#ifdef TIMER_DANAMIC_ALLCATION
	#include <stdlib.h>
	SOFT_TIMER_t * GTimerNodeCreate(FUNC_VOID gcall,uint32_t timeout,uint32_t interval)
	{
		SOFT_TIMER_t *ptimernode;
		if( ( ptimernode = (SOFT_TIMER_t *)(malloc(sizeof(SOFT_TIMER_t))) ) == NULL )
			return NULL;
		if(!gNodeInit(&(ptimernode->linkNode)))
			return NULL;
		uint32_t criflag;
		criflag = gentercritical();
		ptimernode->timeout = timeout + timerticks;
		gleavecritical(criflag);
		ptimernode->interval = interval;
		ptimernode->callback = gcall;
		return(ptimernode);
	}
	void GTimerNodeDestroy(SOFT_TIMER_t *ptimernode)
	{
		if(gIsNodeInList(&timerlist,&(ptimernode->linkNode)) == EXE_FINISH )
		{
			GTimerStop(ptimernode);
		}
		free(ptimernode);
		ptimernode = NULL;
	}
#endif

FuncErrorE GTimerNodeInit(SOFT_TIMER_t * handle,FUNC_VOID gcall, uint32_t timeout, uint32_t interval)
{
	if(!handle)
		return(PTR_NULL);
	if(!gNodeInit(&(handle->linkNode)))
		return(PTR_NULL);
	GTimerStop(handle);
	uint32_t criflag;
	criflag = gentercritical();
	handle->timeout = timeout + timerticks;
	gleavecritical(criflag);
	handle->interval = interval;
	handle->callback = gcall;
	return(EXE_FINISH);
}
void InitTimerlist(void)
{
	gListInit(&timerlist);
}

FuncErrorE GTimerStart(SOFT_TIMER_t * handle)
{
	if(!handle)
		return(PTR_NULL);
//	if(!gListCount(&timerlist))
//	{
//		 if(gListAddFirst(&timerlist,&(handle->linkNode)) == EXE_FINISH)
//			 return(EXE_FINISH); 
//	}
//	else
//	{
		if(gIsNodeInList(&timerlist,&(handle->linkNode)) == EXE_OTHERS)
		{
			if(gListAddLast(&timerlist, &(handle->linkNode)) == EXE_FINISH )
				return(EXE_FINISH);
		}
//	}
	return(EXE_OTHERS);
}

FuncErrorE GTimerStop(SOFT_TIMER_t * handle)
{
	if(!handle)
		return(PTR_NULL);
	if(gIsNodeInList(&timerlist,&(handle->linkNode)) != EXE_FINISH )
	{
		return(EXE_OTHERS);
	}
	return(gListNodeRemove(&timerlist, &(handle->linkNode)));
}
void delay_nms(uint32_t nms)
{
	uint32_t curtick;
	uint32_t criflag;
	criflag = gentercritical();
	curtick = timerticks + nms;
	gleavecritical(criflag);
	while(curtick > timerticks);
}
void blockdelay_nus(uint32_t nus)
{
	uint32_t loop;
	for(loop = 0;loop < nus;loop++)
	{
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	}
}
void GTimerLoop(void)
{
	SOFT_TIMER_t *it;
	uint8_t tnodecount = 0;
	for(it = tNodeParent(timerlist.firstNode, SOFT_TIMER_t, linkNode); tnodecount < timerlist.nodeCount; tnodecount++,it = tNodeParent(it->linkNode.nextNode, SOFT_TIMER_t, linkNode) )
	{
		uint32_t timertickcpy;
		uint32_t criflag;
		criflag = gentercritical();
		timertickcpy = timerticks;
		gleavecritical(criflag);
		if(timertickcpy >= it->timeout)
		{
			if( !(it->interval) )
				GTimerStop(it);
			else
				it->timeout = timertickcpy + it->interval;
			if(it->callback)
				it->callback(NULL);
		}
	}
}
