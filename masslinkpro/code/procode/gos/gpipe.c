/*********************************************************************************************************
** File name               :   gpipe.c
** Date created		   :   2018-07-01
** Version Latest          :   V1.0.0
** Author Latest           :   core    
** File Description        :   
*********************************************************************************************************
** Development Process**
	(1) gpipe.c (V1.0.0)	core	2018-07-01
	
	 
**********************************************************************************************************/
#include "glib.h"

#ifdef FIFO_DANAMIC_ALLCATION
#include <stdlib.h>
FIFO_t* GFIFOCreate(uint32_t len)
{
	FIFO_t *fifo;
	if( (fifo = (FIFO_t *)malloc(sizeof(FIFO_t))) == NULL )
		return(NULL);
	if( (fifo->buf = (uint8_t*)malloc(len)) == NULL )
	{
		free(fifo);
		fifo = NULL;
		return NULL;
	}
	fifo->len = len;
	fifo->r = fifo->buf;
	fifo->w = fifo->buf;
	fifo->cnt = 0;
	return fifo;
}
void GFIFODestroy(FIFO_t* pfifo)
{
	free(pfifo->buf);
	free(pfifo);
	pfifo = NULL;
}
#endif

FuncErrorE GFIFOInit(FIFO_t *pfifo,uint8_t *pbuff,uint32_t len)
{
	if(!pfifo)
		return(PTR_NULL);
	pfifo->buf = pbuff;
	pfifo->len = len;
	pfifo->r = pfifo->buf;
	pfifo->w = pfifo->buf;
	pfifo->cnt = 0;
	return(EXE_FINISH);
}

FuncErrorE GFIFOFlush(FIFO_t* pfifo)
{
	if(!pfifo)
		return(PTR_NULL);	
	pfifo->r = pfifo->buf;
	pfifo->w = pfifo->buf;
	pfifo->cnt = 0;
	return(EXE_FINISH);
}

FuncErrorE GFIFOPush(FIFO_t* pfifo, uint8_t element)
{
	if(!pfifo)
		return(PTR_NULL);		
	if (pfifo->cnt == pfifo->len) 
	{
		return EXE_OTHERS;
	}
	*(pfifo->w) = element;
	pfifo->w = (pfifo->w == (pfifo->buf+pfifo->len-1)) ? pfifo->buf :  pfifo->w + 1;
	pfifo->cnt++;
	return EXE_FINISH;
}

FuncErrorE GFIFOPop(FIFO_t* pfifo,uint8_t *pdata)
{
	if(!pfifo)
		return(PTR_NULL);		
	if (pfifo->cnt == 0) 
	{
		return EXE_OTHERS;
	}
	*pdata = *pfifo->r;
	pfifo->r = (pfifo->r == (pfifo->buf+pfifo->len-1)) ? pfifo->buf :  pfifo->r + 1;
	pfifo->cnt--;
	return EXE_FINISH;
}

FuncErrorE GFIFOPeek(FIFO_t* pfifo,uint8_t *pdatapeek)
{
	if(!pfifo)
		return(PTR_NULL);			
	if (pfifo->cnt == 0) 
	{
		return EXE_OTHERS;
	}
	*pdatapeek = *pfifo->r;
	return EXE_FINISH;
}

uint8_t GFIFOIsFull(FIFO_t* pfifo)
{
	if(!pfifo)
		return 0;				
	return pfifo->cnt == pfifo->len;
}

uint8_t GFIFOIsEmpty(FIFO_t* pfifo)
{
	if(!pfifo)
		return 0;
	return pfifo->cnt == 0;
}

uint32_t GFIFOGetUsed(FIFO_t* pfifo)
{
	if(!pfifo)
		return 0;	
	return pfifo->cnt;
}

uint32_t GFIFOGetFree(FIFO_t* pfifo)
{
	if(!pfifo)
		return 0;	
	return pfifo->len - pfifo->cnt;
}

uint32_t GFIFOPopCnt(FIFO_t* pfifo,uint8_t *pdata,uint32_t cnt)
{
	if(!(pfifo && pdata))
		return 0;
	if(cnt > GFIFOGetUsed(pfifo))
		cnt = GFIFOGetUsed(pfifo);
	uint32_t loop;
	for(loop = 0;loop < cnt;loop++)
		GFIFOPop(pfifo,pdata + loop);
	return cnt;
}

static uint8_t glstrncmp(FIFO_t *pfifo,uint8_t *pbuff,uint8_t bufsize)
{
	uint8_t loop;
	if(!(pfifo && pbuff) || bufsize == 0)
		return 0;
	if(pfifo->cnt < bufsize)
		return 0;
	uint8_t *pcmpbuf;
	pcmpbuf = pfifo->w;
	for(loop = 0;loop < bufsize;loop++)
	{
		if((*pcmpbuf) != *(pbuff + loop))
			return 0;
		pcmpbuf = (pcmpbuf == (pfifo->buf + bufsize -1)) ? pfifo->buf : (pcmpbuf + 1);
	}
	return 1;
}

uint8_t gfifostrncmp(FIFO_t *pfifo,uint8_t *cmpbuf,uint8_t length,uint8_t rxdata)
{
	uint8_t flag = 0;
	if(!(pfifo && cmpbuf))
		return 0;
	if( GFIFOPush(pfifo,rxdata) == EXE_OTHERS )
		return 0;
	flag = glstrncmp(pfifo,cmpbuf,length);
	if( GFIFOIsFull(pfifo) )
					GFIFOPop(pfifo,&rxdata);
	return flag;
}
