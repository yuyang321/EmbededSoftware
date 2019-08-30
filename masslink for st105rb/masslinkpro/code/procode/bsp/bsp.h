
#ifndef _BSP_H_
/*********************************************************************
 			               CORE DESCRIBE
*********************************************************************/
#define FLASH_START_ADDRESS	0x8000000
#define FLASH_BYTE_SIZE		0x20000
#define RAM_START_ADDRESS	0x20000000
#define RAM_BYTE_SIZE		0x10000

#define __initial_sp_		(RAM_START_ADDRESS + RAM_BYTE_SIZE )
/*********************************************************************
 			               CPU Peripheral
*********************************************************************/
#include "stm32f10x.h"
/********************************************************************
											cpu init
*********************************************************************/
#define USART_GSM_PREPRIORITY		1
#define USART_GSM_SUBPRIORITY		0

extern void __main(void *);
extern void ghwrccset(void);
extern void gnvicset(void);
extern void BspInit(void);
/*********************************************************************
												USART
*********************************************************************/
	#define USART_CONSOLE			USART1
	
	#define USART_GSM					USART2
	#define USART_GSM_RX_PIN	GPIO_Pin_3
	#define USART_GSM_TX_PIN	GPIO_Pin_2
	#define USART_GSM_GPIO		GPIOA
	#define USART_GSM_DEFAULT_BAUDRATE	9600
	
	extern void ginitusart(void);
	extern int sendchar(int ch);
	extern int getkey(void);
/*********************************************************************
												GPIO
*********************************************************************/
#define STATUS_LED_GPIO			GPIOD
#define STATUS_LED_GPIO_PIN GPIO_Pin_2
#define STATUS_LED_ON				GPIO_SetBits(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN)
#define STATUS_LED_OFF			GPIO_ResetBits(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN)
#define STATUS_LED_TOGGLE		GPIO_WriteBit(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN, !GPIO_ReadOutputDataBit(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN))		
/*********************************************************************/
#endif
