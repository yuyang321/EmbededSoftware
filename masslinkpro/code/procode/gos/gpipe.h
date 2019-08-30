#ifndef __GPIPE_H__
#define __GPIPE_H__
	
#include <stdint.h>
	
#define FIFO_DANAMIC_ALLCATION

typedef struct _FIFO_T_
{
	uint8_t *buf;
	uint32_t len;
	uint8_t *r;
	uint8_t *w;
	uint32_t cnt;
}FIFO_t;

extern FIFO_t* GFIFOCreate(uint32_t len);
extern void GFIFODestroy(FIFO_t* pfifo);
extern FuncErrorE GFIFOInit(FIFO_t *pfifo,uint8_t *pbuff,uint32_t len);
extern FuncErrorE GFIFOFlush(FIFO_t* pfifo);
extern FuncErrorE GFIFOPush(FIFO_t* pfifo, uint8_t element);
extern FuncError GFIFOPop(FIFO_t* pfifo,uint8_t *pdata);
extern FuncError GFIFOPeek(FIFO_t* pfifo,uint8_t *pdatapeek);
extern uint8_t GFIFOIsFull(FIFO_t* pfifo);
extern uint8_t GFIFOIsEmpty(FIFO_t* pfifo);
extern uint32_t GFIFOGetUsed(FIFO_t* pfifo);
extern uint32_t GFIFOGetFree(FIFO_t* pfifo);

#endif
