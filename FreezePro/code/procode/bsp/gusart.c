#include "bsp.h"
#include "..\user\guser.h"
#include "..\mid\gmodbus.h"
#include "..\mid\LCD\cmd_queue.h"
void ginitusart(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	#if(HARDWARE_BOARD_VER == MASS_LINK_BOARD)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOB,ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	#elif(HARDWARE_BOARD_VER == ISO_STM32_BOARD)
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,ENABLE);
	#elif(HARDWARE_BOARD_VER == FREEZE_CONTROL_BOARD_V10)
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB ,ENABLE);
		RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3 ,ENABLE);
	#endif
	USART_InitTypeDef	usartInitarg = {USART_LCD_DEFAULT_BAUDRATE,USART_WordLength_8b,USART_StopBits_1,\
						USART_Parity_No,USART_Mode_Rx | USART_Mode_Tx,USART_HardwareFlowControl_None};
	//Init LCD USART
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = USART_LCD_TX_PIN;
	GPIO_Init(USART_LCD_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = USART_LCD_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_LCD_GPIO, &GPIO_InitStructure);
	
	USART_Init(USART_LCD, &usartInitarg);
	USART_ITConfig(USART_LCD, USART_IT_RXNE , ENABLE);
  //USART_ITConfig(USART_LCD, USART_IT_ERR , ENABLE);
	USART_Cmd(USART_LCD, ENABLE);
	//Init flow sensor USART
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = USART_FSENSOR_TX_PIN;
	GPIO_Init(USART_FSENSOR_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = USART_FSENSOR_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_FSENSOR_GPIO, &GPIO_InitStructure);
	
	usartInitarg.USART_BaudRate = USART_FSENSOR_DEFAULT_BAUDRATE;
	USART_Init(USART_FSENSOR, &usartInitarg);					
	USART_ITConfig(USART_FSENSOR, USART_IT_RXNE , ENABLE);
	USART_ITConfig(USART_FSENSOR, USART_IT_ERR, ENABLE);
	USART_Cmd(USART_FSENSOR, ENABLE);		
}

void SendUsartString(USART_TypeDef * usart, uint8_t *ptr,uint16_t size,USART_TRANS_MODE_e mode)
{
	uint16_t loop;
	FIFO_t *pfifo;
	uint8_t txdata;
	SYS_TASK_LIST_e eventflag; 
	if( !(usart && ptr) )
		return;
	switch((uint32_t)usart)
	{
		case (uint32_t)USART1:
				pfifo = &LCDTxFifo;
				eventflag = HAL_GSMTX_EVENT_E;
		break;
		case (uint32_t)USART3:
				pfifo = &FsensorTxFifo;
				eventflag = HAL_FSTX_EVENT_E;
		break;
		default:
			return;
	}
	switch(mode)
	{
		case UNBLOCK_TRANS_MODE:
			for(loop = 0 ; loop < size ; loop++ )
				GFIFOPush(pfifo, *(ptr+loop) );
			USART_ITConfig(usart, USART_IT_TC , ENABLE);
			GFIFOPop(pfifo,&txdata);
			USART_SendData(usart, (uint16_t)txdata );
			SET_EVENT(eventflag);
		break;
		case BLOCK_TRANS_MODE:
			for(loop = 0 ; loop < size ; loop++ )
			{
				USART_SendData(usart, *(ptr+loop) );
				uint32_t criflag;
				criflag = gentercritical();
				while(USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET );
				gleavecritical(criflag);
			}
//			if(*(ptr+loop-1)== '\n')
//				blockdelay_nus(2000);
		break;
		default:
			return;
	}
}

void gUsartfifosend(USART_TypeDef * usart)
{
	SYS_TASK_LIST_e eventflag; 
	FIFO_t *pfifo;
	uint8_t txdata;
	if( !usart )
		return;
	switch((uint32_t)usart)
	{
		case (uint32_t)USART1:
				pfifo = &LCDTxFifo;
				eventflag = HAL_GSMTX_EVENT_E;
		break;
		case (uint32_t)USART3:
				pfifo = &FsensorTxFifo;
				eventflag = HAL_FSTX_EVENT_E;
		break;
		default:
			return;
	}
	USART_ITConfig(usart, USART_IT_TC , ENABLE);
	GFIFOPop(pfifo,&txdata);
	USART_SendData(usart, (uint16_t)txdata );
	SET_EVENT(eventflag);
}

int gsendchar(int ch,USART_TypeDef * usart)
{
	while(USART_GetFlagStatus(usart,USART_FLAG_TXE) == RESET);
	USART_SendData(usart,ch);
	while(USART_GetFlagStatus(usart,USART_FLAG_TC) == RESET);
	return(ch);
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
void USART1_IRQHandler(void)
{
	uint8_t txdata;
	uint8_t rxdata;
	if(USART_GetFlagStatus(USART_LCD, USART_FLAG_ORE) != RESET)
	{  
			USART_ReceiveData(USART_LCD); 
			USART_ClearITPendingBit(USART_LCD, USART_IT_RXNE);
	}  
	if(USART_GetITStatus(USART_LCD,USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART_LCD,USART_IT_RXNE);
		if(GFIFOIsFull(&LCDRxFifo))
		{
			//fifo full
			SET_EVENT(HAL_FAULT_EVENT_E);
			SET_FAULTCODE(USART_GSM_FIFO_FULL);
			return;
		}	
		rxdata = USART_ReceiveData(USART_LCD);
		GFIFOPush(&LCDRxFifo, rxdata );
		queue_push(rxdata);
		//event delivery
		SET_EVENT(HAL_GSMRX_EVENT_E);
	}
	else if(USART_GetITStatus(USART_LCD,USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART_LCD,USART_IT_TC);
		if(GFIFOIsEmpty(&LCDTxFifo))
		{
			//fifo empty
			SET_EVENT(HAL_FAULT_EVENT_E);
			SET_FAULTCODE(USART_GSM_FIFO_EMPTY);
			USART_ITConfig(USART_LCD, USART_IT_TC , DISABLE);
			return;
		}
		//event delivery
		GFIFOPop(&LCDTxFifo,&txdata);
		USART_SendData(USART_LCD, (uint16_t)txdata );
		SET_EVENT(HAL_GSMTX_EVENT_E);
	}
}

void USART3_IRQHandler(void)
{
	uint8_t txdata;
	uint8_t rxdata;
	if(USART_GetITStatus(USART_FSENSOR,USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART_FSENSOR,USART_IT_RXNE);
		if(GFIFOIsFull(&FsensorRxFifo))
		{
			//fifo full
			SET_EVENT(HAL_FAULT_EVENT_E);
			SET_FAULTCODE(USART_FSENSOR_FIFO_FULL);
			return;
		}	
		rxdata = USART_ReceiveData(USART_FSENSOR);
		GFIFOPush(&FsensorRxFifo, rxdata );
		gstartmodtimer(MODTIMER_STYLE_RECFRAME);
		//event delivery
		//SET_EVENT(HAL_FSRX_EVENT_E);
	}
	else if(USART_GetITStatus(USART_FSENSOR,USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART_FSENSOR,USART_IT_TC);
		if(GFIFOIsEmpty(&FsensorTxFifo))
		{
			//fifo empty
			SET_EVENT(HAL_FAULT_EVENT_E);
			SET_FAULTCODE(USART_FSENSOR_FIFO_EMPTY);
			USART_ITConfig(USART_FSENSOR, USART_IT_TC , DISABLE);
			gstartmodtimer(MODTIMER_STYLE_TXFRAME);
			return;
		}
		//event delivery
		GFIFOPop(&FsensorTxFifo,&txdata);
		USART_SendData(USART_FSENSOR, (uint16_t)txdata );
		SET_EVENT(HAL_FSTX_EVENT_E);
	}
}
