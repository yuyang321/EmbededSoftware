
#ifndef _GUSER_H_
#define _GUSER_H_

	#include "..\gos\glib.h"
/**
	USART GSM  MACRO/Variable
**/
#define USART_GSM_RxBUFF_SIZE		100
#define USART_GSM_TxBUFF_SIZE		100
extern FIFO_t GsmRxFifo;
extern FIFO_t GsmTxFifo;
/**
	External  Function
**/
	extern int main(void);
	extern void GosInit(void);
	extern void HAL_GSMRX_EVENT(void *arg);
	extern void HAL_EXIT_EVENT(void *arg);
	extern void HAL_GSMTX_EVENT(void *arg);
	extern void HAL_TIMER_EVENT(void *arg);
	extern void SYS_APL_EVENT(void *arg);
	extern void HAL_FAULT_EVENT(void *arg);
#endif
