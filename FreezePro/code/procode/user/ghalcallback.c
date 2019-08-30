#include "..\gos\glib.h"
#include "bsp.h"
#include "guser.h"
#include "..\mid\gSensorAlg.h"
#include "..\mid\LCD\cmd_queue.h"
/****************************************************************************************************************
*****************************************************************************************************************
****************************************************************************************************************/
/**
	system event handle:	
		HAL_LCDRX_EVENT
		HAL_ADCSAMPLE_EVENT
		HAL_LCDTX_EVENT
		HAL_FAULT_EVENT
		HAL_TIMER_EVENT
**/
void HAL_GSMTX_EVENT(void *arg)
{
//	uint8_t txdata;
//	GFIFOPop(&LCDTxFifo,&txdata);
//	USART_SendData(USART_LCD, (uint16_t)txdata );
	__NOP();
}

void HAL_FAULT_EVENT(void *arg)
{
	switch(HalFaultCode)
	{
		case USART_GSM_FIFO_FULL:
		break;
		case USART_GSM_FIFO_EMPTY:
			USART_ITConfig(USART_LCD, USART_IT_TC , DISABLE);
		break;
		default:;
	}
}

void HAL_TIMER_EVENT(void *arg)
{
	GTimerLoop();
}

//USART RX Handle
void HAL_GSMRX_EVENT(void *arg)
{
	uint8_t rxdata;   
	while(GFIFOPop(&LCDRxFifo,&rxdata) == EXE_FINISH)
	{
		//SendUsartString(USART_FSENSOR, &rxdata,1,UNBLOCK_TRANS_MODE);
		//queue_push(rxdata);
	}
}

void HAL_FSRX_EVENT(void *arg)
{
	uint8_t rxdata;
	while(GFIFOPop(&FsensorRxFifo,&rxdata) == EXE_FINISH)
	{
		
	}
}

void HAL_FSTX_EVENT(void *arg)
{
//	uint8_t txdata;
//	GFIFOPop(&FsensorTxFifo,&txdata);
//	USART_SendData(USART_FSENSOR, (uint16_t)txdata );
	__NOP();
}

//SYS_PRESS_INFOR_s PressInfor ={.Pre4Value = 0};
//ADC Sample Analyze
void HAL_ADCSAMPLE_EVENT(void *arg)
{
	 uint8_t loop;
	static ADC_ANALOG_VAR_INIT_STATE_e adcinitstate = ADC_ANALOG_VAR_INIT_STATE_START;
	switch(adcinitstate)
	{
		case ADC_ANALOG_VAR_INIT_STATE_START:
				adcinitstate = ADC_ANALOG_VAR_INIT_STATE_WORK;
				for(loop = 0;loop < ADC_CHANNEL_NUM;loop++)
				{
					gLPfiltMidVal((uint16_t *)ADChannelValBuf[loop],ADC_BUFF_NUM,ADChannellastVal+loop);
					*(ADChannelCurVal+loop) = *(ADChannellastVal+loop);
				}
		break;
		case ADC_ANALOG_VAR_INIT_STATE_WORK:
				//Filt Arithmetic
				for(loop = 0;loop < ADC_CHANNEL_NUM;loop++)
					gLPfiltOneOrder(ADChannelCurVal+loop,ADChannellastVal+loop,ADChannelValBuf[loop],ADC_BUFF_NUM,ADC_FILT_ARG_VAL);
				//converse press value
				gCalPresaVal(&(sysvar.PressInfor));
		break;
		default:
		break;
	}
	sysvar.PressInfor.Pre4Value = gGetP4Val();
}
