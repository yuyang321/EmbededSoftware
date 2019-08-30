
#ifndef _BSP_H_
#define _BSP_H_
/*********************************************************************
 			               HardWare Board DESCRIBE
*********************************************************************/
#define MASS_LINK_BOARD		0
#define ISO_STM32_BOARD		1
#define HARDWARE_BOARD_VER	MASS_LINK_BOARD
/*********************************************************************
 			               FLASH and RAM DESCRIBE
*********************************************************************/
#define FLASH_START_ADDRESS	0x8000000
#define FLASH_BYTE_SIZE		0x40000
#define RAM_START_ADDRESS	0x20000000
#define RAM_BYTE_SIZE		0xC000
//Set Msp Address
#define __initial_sp_		(RAM_START_ADDRESS + RAM_BYTE_SIZE )
/*********************************************************************
 			               CPU Peripheral Header
*********************************************************************/
#include 	"stm32f10x.h"
//NVIC header
#include <misc.h>
/***************************************************************************************************/
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
/********************************************************************
											cpu init
*********************************************************************/
#define USART_GSM_PREPRIORITY						0
#define USART_GSM_SUBPRIORITY						0

#define ADCDMA_PRIORITY_PREEMPTION			2	
#define ADCDMA_PRIORITY_SUBPRIORITY			1

#define EXIT_CH0_7_PREPRIORITY					1
#define EXIT_CH0_7_SUBPRIORITY  				0

extern void __main(void *);
extern void ghwrccset(void);
extern void gnvicset(void);
extern void BspInit(void);
/*********************************************************************
												USART
*********************************************************************/
	#if(HARDWARE_BOARD_VER == MASS_LINK_BOARD)
		#define USART_CONSOLE			USART1
		#define USART_GSM					USART1
		#define USART_GSM_RX_PIN	GPIO_Pin_7
		#define USART_GSM_TX_PIN	GPIO_Pin_6
		#define USART_GSM_GPIO		GPIOB
	#elif(HARDWARE_BOARD_VER == ISO_STM32_BOARD)
		#define USART_CONSOLE			USART1
		#define USART_GSM					USART1
		#define USART_GSM_RX_PIN	GPIO_Pin_10
		#define USART_GSM_TX_PIN	GPIO_Pin_9
		#define USART_GSM_GPIO		GPIOA
	#else
	//........................................... 
	#endif
	
	#define USART_GSM_DEFAULT_BAUDRATE	115200
	
	typedef enum _USART_TRANS_MODE_E_{
		UNBLOCK_TRANS_MODE,
		BLOCK_TRANS_MODE,
	}USART_TRANS_MODE_e;
		
	extern void ginitusart(void);
	extern int sendchar(int ch);
	extern int getkey(void);
	extern void SendUsartString(uint8_t *ptr,uint16_t size,USART_TRANS_MODE_e mode);
/*********************************************************************
												GPIO
*********************************************************************/
#if(HARDWARE_BOARD_VER == MASS_LINK_BOARD)
	#define STATUS_LED_GPIO					GPIOC
	#define STATUS_LED_GPIO_RCC			RCC_APB2Periph_GPIOC
	#define STATUS_LED_GPIO_PIN 		GPIO_Pin_14
	#define STATUS_LED_ON						GPIO_SetBits(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN)
	#define STATUS_LED_OFF					GPIO_ResetBits(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN)
	#define STATUS_LED_TOGGLE				GPIO_WriteBit(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN, !GPIO_ReadOutputDataBit(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN))		

	#define GSM_PWON_GPIO						GPIOB		
	#define GSM_PWON_GPIO_RCC				RCC_APB2Periph_GPIOB
	#define GSM_PWON_GPIO_PIN				GPIO_Pin_0
	#define GSM_PWON_PULL_DOWN			GPIO_ResetBits(GSM_PWON_GPIO, GSM_PWON_GPIO_PIN)
	#define GSM_PWON_PULL_UP				GPIO_SetBits(GSM_PWON_GPIO, GSM_PWON_GPIO_PIN)
	
	#define GSM_RST_GPIO						GPIOB
	#define GSM_RST_GPIO_RCC				RCC_APB2Periph_GPIOB
	#define GSM_RST_GPIO_PIN				GPIO_Pin_2
	#define GSM_ENABLE_RESET				GPIO_ResetBits(GSM_RST_GPIO, GSM_RST_GPIO_PIN)
	#define GSM_DISABLE_RESET				GPIO_SetBits(GSM_RST_GPIO, GSM_RST_GPIO_PIN)

	//#define GSM_STATUS_GPIO					GPIOB
	//#define GSM_STATUS_GPIO_RCC			RCC_APB2Periph_GPIOB
	//#define GSM_STATUS_GPIO_PIN			GPIO_Pin_0
	//#define GSM_GET_STATUS					GPIO_ReadOutputDataBit(GSM_STATUS_GPIO, GSM_STATUS_GPIO_PIN)

	#define EXT_INPUT_GPIO					GPIOA
	#define EXT_INPUT_GPIO_RCC			RCC_APB2Periph_GPIOA
	#define EXT_INPUT_CH0_PIN				GPIO_Pin_0
	#define EXT_INPUT_CH1_PIN				GPIO_Pin_1
	#define EXT_INPUT_CH2_PIN				GPIO_Pin_2
	#define EXT_INPUT_CH3_PIN				GPIO_Pin_3
	#define EXT_INPUT_CH4_PIN				GPIO_Pin_4
	#define EXT_INPUT_CH5_PIN				GPIO_Pin_5
	#define EXT_INPUT_CH6_PIN				GPIO_Pin_6
	#define EXT_INPUT_CH7_PIN				GPIO_Pin_7
	#define GET_CHX_STATUS(x)				GPIO_ReadInputDataBit(EXT_INPUT_GPIO, (uint16_t)(1 << x))
#elif(HARDWARE_BOARD_VER == ISO_STM32_BOARD)
	#define STATUS_LED_GPIO					GPIOF
	#define STATUS_LED_GPIO_RCC			RCC_APB2Periph_GPIOF
	#define STATUS_LED_GPIO_PIN 		GPIO_Pin_7
	#define STATUS_LED_ON						GPIO_SetBits(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN)
	#define STATUS_LED_OFF					GPIO_ResetBits(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN)
	#define STATUS_LED_TOGGLE				GPIO_WriteBit(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN, !GPIO_ReadOutputDataBit(STATUS_LED_GPIO, STATUS_LED_GPIO_PIN))		

	#define GSM_PWON_GPIO						GPIOB		
	#define GSM_PWON_GPIO_RCC				RCC_APB2Periph_GPIOB
	#define GSM_PWON_GPIO_PIN				GPIO_Pin_0
	#define GSM_PWON_PULL_DOWN			GPIO_ResetBits(GSM_PWON_GPIO, GSM_PWON_GPIO_PIN)
	#define GSM_PWON_PULL_UP				GPIO_SetBits(GSM_PWON_GPIO, GSM_PWON_GPIO_PIN)
	
	#define GSM_RST_GPIO						GPIOB
	#define GSM_RST_GPIO_RCC				RCC_APB2Periph_GPIOB
	#define GSM_RST_GPIO_PIN				GPIO_Pin_2
	#define GSM_ENABLE_RESET				GPIO_ResetBits(GSM_RST_GPIO, GSM_RST_GPIO_PIN)
	#define GSM_DISABLE_RESET				GPIO_SetBits(GSM_RST_GPIO, GSM_RST_GPIO_PIN)

	//#define GSM_STATUS_GPIO					GPIOB
	//#define GSM_STATUS_GPIO_RCC			RCC_APB2Periph_GPIOB
	//#define GSM_STATUS_GPIO_PIN			GPIO_Pin_0
	//#define GSM_GET_STATUS					GPIO_ReadOutputDataBit(GSM_STATUS_GPIO, GSM_STATUS_GPIO_PIN)

	#define EXT_INPUT_GPIO					GPIOA
	#define EXT_INPUT_GPIO_RCC			RCC_APB2Periph_GPIOA
	#define EXT_INPUT_CH0_PIN				GPIO_Pin_0
	#define EXT_INPUT_CH1_PIN				GPIO_Pin_1
	#define EXT_INPUT_CH2_PIN				GPIO_Pin_2
	#define EXT_INPUT_CH3_PIN				GPIO_Pin_3
	#define EXT_INPUT_CH4_PIN				GPIO_Pin_4
	#define EXT_INPUT_CH5_PIN				GPIO_Pin_5
	#define EXT_INPUT_CH6_PIN				GPIO_Pin_6
	#define EXT_INPUT_CH7_PIN				GPIO_Pin_7
	#define GET_CHX_STATUS(x)				GPIO_ReadInputDataBit(EXT_INPUT_GPIO, (uint16_t)(1 << x))
#else
//........................................... 
#endif

#define EXTI_FILT_TIME	5				//ms
/****************************************************************************
												ADC
****************************************************************************/
#define ADC_USED					  ADC1
#define ADC_DR_Adress      (ADC1_BASE+0x4C)
#define ADC_PORT_RCC			 (RCC_APB2Periph_GPIOC)
#define ADC_PORT					 (GPIOC)
#define ADC_PORT_PIN			 (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)
#define ADC_RCC						 (RCC_APB2Periph_ADC1)
#define ADC_DMA_RCC				 (RCC_AHBPeriph_DMA1)
#define ADC_DMA_CHANNEL    (DMA1_Channel1)
#define ADC_CHANNEL_NUM			4
#define ADC_BUFF_NUM				5

typedef enum ANALOG_CHANNEL_VAL_E{
	ANALOG_CHANNEL_0 = ADC_Channel_10,
	ANALOG_CHANNEL_1 = ADC_Channel_11,
	ANALOG_CHANNEL_2 = ADC_Channel_12,
	ANALOG_CHANNEL_3 = ADC_Channel_13, 
}ANALOG_CHANNEL_VAL_e;
typedef enum ANALOG_REGULAR_GROUP_INDEX_E{
	ANALOG_CHANNEL_0_INDEX = 1,
	ANALOG_CHANNEL_1_INDEX,
	ANALOG_CHANNEL_2_INDEX,
	ANALOG_CHANNEL_3_INDEX,
}ANALOG_REGULAR_GROUP_INDEX_e;

extern uint8_t adcbufcnt;
extern uint8_t adconvertflag;
extern uint16_t ADChannelValBuf[ADC_CHANNEL_NUM][ADC_BUFF_NUM];
extern uint16_t ADChannelCurVal[ADC_CHANNEL_NUM];
extern uint16_t ADChannellastVal[ADC_CHANNEL_NUM];

extern void gInitAnalogADC(void);

#endif
