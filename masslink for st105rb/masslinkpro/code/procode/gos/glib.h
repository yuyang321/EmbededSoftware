#ifndef _GLIB_H_
#define _GLIB_H_

	/**************************************************************************************************/
	#include <stdint.h>
	typedef void (*FUNC_VOID)(void *);
	#define NULL	0
	typedef enum _FUNC_ERROR_E_{
		PTR_NULL,
		EXE_FINISH,
		EXE_OTHERS,
	}FuncErrorE;
	/**************************************************************************************************/
	typedef struct _LIST_Node_T_
	{
			struct _LIST_Node_T_ * preNode;
			struct _LIST_Node_T_ * nextNode;
	}LIST_NODE_t;
	
	typedef struct _LIST_T_
	{   
			LIST_NODE_t headNode;
			uint32_t nodeCount;
	}LIST_t;
	
	#define firstNode   headNode.nextNode
	#define lastNode    headNode.preNode
	//
	#define tNodeParent(node, parent, name) (parent *)((uint32_t)node - (uint32_t)&((parent *)0)->name)
	
	extern FuncErrorE gNodeInit (LIST_NODE_t * node);
	extern FuncErrorE gListInit (LIST_t * list);	
	extern uint32_t gListCount (LIST_t * list);
	extern LIST_NODE_t * gListFirst (LIST_t * list);
	extern LIST_NODE_t * gListLast (LIST_t * list);
	extern LIST_NODE_t * gListPre (LIST_t * list, LIST_NODE_t * node);
	extern LIST_NODE_t * gListNext (LIST_t * list, LIST_NODE_t * node);
	extern FuncErrorE gListRemoveAll (LIST_t * list);
	extern FuncErrorE gListAddFirst (LIST_t * list, LIST_NODE_t * node);
	extern FuncErrorE gListAddLast (LIST_t * list, LIST_NODE_t * node);
	extern LIST_NODE_t * gListRemoveFirst (LIST_t * list);
	extern FuncErrorE gListInsertAfter (LIST_t * list, LIST_NODE_t * nodeAfter,  LIST_NODE_t * nodeToInsert);
	extern FuncErrorE gListNodeRemove (LIST_t * list, LIST_NODE_t * node);	
	extern  FuncErrorE gIsNodeInList(LIST_t *list,LIST_NODE_t *node);
	/***************************************************************************************************/
	#define TIMER_DANAMIC_ALLCATION
	typedef struct  _SOFT_TIMER_T_{
    		LIST_NODE_t    linkNode;
    		uint32_t timeout;
    		uint32_t interval;
    		FUNC_VOID callback;
	}SOFT_TIMER_t;
extern volatile uint32_t timerticks;	
	
extern SOFT_TIMER_t * GTimerNodeCreate(FUNC_VOID gcall,uint32_t timeout,uint32_t interval);
extern void GTimerNodeDestroy(SOFT_TIMER_t *ptimernode);
extern FuncErrorE GTimerNodeInit(SOFT_TIMER_t * handle,FUNC_VOID gcall, uint32_t timeout, uint32_t interval);
extern FuncErrorE GTimerStart(SOFT_TIMER_t * handle);
extern FuncErrorE GTimerStop(SOFT_TIMER_t * handle);
extern void GTimerTicks(void);
extern void InitTimerlist(void);
extern void GTimerLoop(void);
	/***************************************************************************************************/
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
	extern FuncErrorE GFIFOPop(FIFO_t* pfifo,uint8_t *pdata);
	extern FuncErrorE GFIFOPeek(FIFO_t* pfifo,uint8_t *pdatapeek);
	extern uint8_t GFIFOIsFull(FIFO_t* pfifo);
	extern uint8_t GFIFOIsEmpty(FIFO_t* pfifo);
	extern uint32_t GFIFOGetUsed(FIFO_t* pfifo);
	extern uint32_t GFIFOGetFree(FIFO_t* pfifo);
  /***************************************************************************************************/
	//System Schedule
	typedef enum _SYS_TASK_LIST_E_{
		HAL_GSMRX_EVENT_E = 1<<0,
		HAL_EXIT_EVENT_E = 1<<1,
		HAL_GSMTX_EVENT_E = 1<<2,
		HAL_FAULT_EVENT_E = 1<<3,
		HAL_TIMER_EVENT_E = 1<<4,
		SYS_APL_EVENT_E = 1<<5,
	}SYS_TASK_LIST_e;

	typedef enum _HAL_FAULT_Code_E_{
	USART_GSM_FIFO_FULL = 1<<0,
	USART_GSM_FIFO_EMPTY = 1<<1,
	}HAL_FAULT_Code_e;

	#define PEND_EVENT(x)				do{ SysTaskMask &= (uint32_t)(~(x)); }while(0)
	#define UNPEND_EVENT(x)   	do{ SysTaskMask |= (uint32_t)(x); }while(0)
	#define SET_EVENT(x)	 			do{ SysTaskFlag |= (uint32_t)(x); }while(0)
	#define RESET_EVENT(x) 			do{ SysTaskFlag &= (uint32_t)(~(x)); }while(0)
	#define SET_FAULTCODE(x)		do{ HalFaultCode |= (uint32_t)(x); }while(0)
	#define RESET_FAULTCODE(x)	do{ HalFaultCode &= (uint32_t)(~(x)); }while(0)
	extern volatile uint32_t SysTaskFlag;
	extern volatile uint32_t SysTaskMask;
	extern volatile uint32_t HalFaultCode;
	extern void SysRun(void);
	/***************************************************************************************************/
	#include <misc.h>
	#define asserterror(x)	__breakpoint(x)

	__STATIC_INLINE uint32_t gentercritical(void)
	{
		uint32_t itflag;
		itflag = __get_PRIMASK();
		__disable_irq();
		return(itflag);
	}
	__STATIC_INLINE void gleavecritical(uint32_t itflag)
	{
		__set_PRIMASK(itflag);
	}
  /***************************************************************************************************/
#endif
