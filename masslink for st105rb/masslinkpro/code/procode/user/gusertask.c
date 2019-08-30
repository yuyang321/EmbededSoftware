#include "bsp.h"
#include "guser.h"

static SOFT_TIMER_t sledtimer;
static void gsledtoggle(void *arg)
{
	STATUS_LED_TOGGLE;
}

static uint8_t GsmRxBuf[USART_GSM_RxBUFF_SIZE];
static uint8_t GsmTxBuf[USART_GSM_TxBUFF_SIZE];
FIFO_t GsmRxFifo;
FIFO_t GsmTxFifo;

void GosInit(void)
{
	//timer init
	InitTimerlist();
	GTimerNodeInit(&sledtimer,gsledtoggle, 100, 100);
	GTimerStart(&sledtimer);
	//pipe init
	GFIFOInit(&GsmRxFifo,GsmRxBuf,sizeof(GsmRxBuf));
	GFIFOInit(&GsmTxFifo,GsmTxBuf,sizeof(GsmTxBuf));
	
}

void HAL_GSMRX_EVENT(void *arg)
{
	uint8_t rxdata;
	GFIFOPop(&GsmRxFifo,&rxdata);
	
}
void HAL_EXIT_EVENT(void *arg)
{

}
void HAL_GSMTX_EVENT(void *arg)
{
	
}
void HAL_FAULT_EVENT(void *arg)
{
	switch(HalFaultCode)
	{
		case USART_GSM_FIFO_FULL:
		break;
		case USART_GSM_FIFO_EMPTY:
		break;
		default:;
	}
}
void HAL_TIMER_EVENT(void *arg)
{
	GTimerLoop();
}

void SYS_APL_EVENT(void *arg)
{
//	switch()
//	{
//		
//	}
}
