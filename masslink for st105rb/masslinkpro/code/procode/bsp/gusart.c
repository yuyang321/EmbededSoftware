#include "bsp.h"
#include "..\user\guser.h"
void ginitusart(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = USART_GSM_TX_PIN;
	GPIO_Init(USART_GSM_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = USART_GSM_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_GSM_GPIO, &GPIO_InitStructure);
	
	USART_InitTypeDef	usartInitarg = {115200,USART_WordLength_8b,USART_StopBits_1,\
	USART_Parity_No,USART_Mode_Rx | USART_Mode_Tx,USART_HardwareFlowControl_None};
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	USART_Init(USART_GSM, &usartInitarg);
	USART_ITConfig(USART_GSM, USART_IT_RXNE , ENABLE);
  USART_Cmd(USART_GSM, ENABLE);
}

int sendchar(int ch)
{
	while(USART_GetFlagStatus(USART_CONSOLE,USART_FLAG_TXE) == RESET);
	USART_SendData(USART_CONSOLE,ch);
	while(USART_GetFlagStatus(USART_CONSOLE,USART_FLAG_TC) == RESET);
	return(ch);
}

int getkey(void)
{
	while(USART_GetFlagStatus(USART_CONSOLE,USART_FLAG_RXNE) == RESET);
	return(USART_ReceiveData(USART_CONSOLE));
}

//USART IT_HANDLE
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART_GSM,USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART_GSM,USART_IT_RXNE);
		if(GFIFOIsFull(&GsmRxFifo))
		{
			//fifo full
			SET_EVENT(HAL_FAULT_EVENT_E);
			SET_FAULTCODE(USART_GSM_FIFO_FULL);
			return;
		}	
		GFIFOPush(&GsmRxFifo, (uint8_t)USART_ReceiveData(USART_GSM) );
		//event delivery
		SET_EVENT(HAL_GSMRX_EVENT_E);
	}
	else if(USART_GetITStatus(USART_GSM,USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART_GSM,USART_IT_TC);
		if(GFIFOIsEmpty(&GsmTxFifo))
		{
			//fifo empty
			SET_EVENT(HAL_FAULT_EVENT_E);
			//SET_FAULTCODE(USART_GSM_FIFO_FULL);
			return;
		}
		uint8_t txdata;
		GFIFOPop(&GsmTxFifo,&txdata);
		USART_SendData(USART_GSM, (uint16_t)txdata );
		//event delivery
		SET_EVENT(HAL_GSMTX_EVENT_E);
	}
}
