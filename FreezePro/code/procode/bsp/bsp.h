
#ifndef _BSP_H_
#define _BSP_H_
/*********************************************************************
 			               HardWare Board DESCRIBE
*********************************************************************/
#define ISO_STM32_BOARD							1
#define FREEZE_CONTROL_BOARD_V10	  2
#define HARDWARE_BOARD_VER	FREEZE_CONTROL_BOARD_V10
#define STM32F103RC
/*********************************************************************
 			               FLASH and RAM DESCRIBE
*********************************************************************/
#define FLASH_START_ADDRESS	0x8000000
#define FLASH_BYTE_SIZE		0x10000
#define RAM_START_ADDRESS	0x20000000
#define RAM_BYTE_SIZE		0x5000
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
#define USART_LCD_PREPRIORITY						0
#define USART_LCD_SUBPRIORITY						0

#define USART_FSENSOR_PREPRIORITY				0
#define USART_FSENSOR_SUBPRIORITY				1

#define ADCDMA_PRIORITY_PREEMPTION			1
#define ADCDMA_PRIORITY_SUBPRIORITY			0

#define MODTIMER_PRIORITY_PREEMPTION		2
#define MODTIMER_PRIORITY_SUBPRIORITY		0

#define PRECISE_TIMER_PREPRIORITY				2
#define PRECISE_TIMER_SUBPRIORITY				1

extern void __main(void *);
extern void ghwrccset(void);
extern void gnvicset(void);
extern void BspInit(void);
/*********************************************************************
												USART
*********************************************************************/
	#if(HARDWARE_BOARD_VER == ISO_STM32_BOARD)
		#define USART_CONSOLE			USART1
		#define USART_GSM					USART1
		#define USART_GSM_RX_PIN	GPIO_Pin_10
		#define USART_GSM_TX_PIN	GPIO_Pin_9
		#define USART_GSM_GPIO		GPIOA
	#elif(HARDWARE_BOARD_VER == FREEZE_CONTROL_BOARD_V10)
		#define USART_CONSOLE			USART1
		#define USART_LCD					USART1
		#define USART_LCD_RX_PIN	GPIO_Pin_10
		#define USART_LCD_TX_PIN	GPIO_Pin_9
		#define USART_LCD_GPIO		GPIOA
		#define USART_LCD_DEFAULT_BAUDRATE	38400
		
		#define USART_FSENSOR							USART3		
		#define USART_FSENSOR_RX_PIN			GPIO_Pin_11
		#define USART_FSENSOR_TX_PIN			GPIO_Pin_10
		#define USART_FSENSOR_GPIO				GPIOB
		#define USART_FSENSOR_DEFAULT_BAUDRATE	38400
	#else
	//...........................................
	#endif

	typedef enum _USART_TRANS_MODE_E_{
		UNBLOCK_TRANS_MODE,
		BLOCK_TRANS_MODE,
	}USART_TRANS_MODE_e;

	extern void ginitusart(void);
	extern int gsendchar(int ch,USART_TypeDef * usart);
	extern int sendchar(int ch);
	extern int getkey(void);
	extern void SendUsartString(USART_TypeDef * usart, uint8_t *ptr,uint16_t size,USART_TRANS_MODE_e mode);
	extern void gUsartfifosend(USART_TypeDef * usart);
/*********************************************************************
												GPIO
*********************************************************************/
#if(HARDWARE_BOARD_VER == ISO_STM32_BOARD)
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
#elif(HARDWARE_BOARD_VER == FREEZE_CONTROL_BOARD_V10)
	#define STATUS_LED_GPIO					GPIOB
	#define STATUS_LED_GPIO_RCC			RCC_APB2Periph_GPIOB

	#define STATUS_LED1_GPIO_PIN 		GPIO_Pin_6
	#define STATUS_LED1_ON					GPIO_SetBits(STATUS_LED_GPIO, STATUS_LED1_GPIO_PIN)
	#define STATUS_LED1_OFF					GPIO_ResetBits(STATUS_LED_GPIO, STATUS_LED1_GPIO_PIN)
	#define STATUS_LED1_TOGGLE			GPIO_WriteBit(STATUS_LED_GPIO, STATUS_LED1_GPIO_PIN, !GPIO_ReadOutputDataBit(STATUS_LED_GPIO, STATUS_LED1_GPIO_PIN))

	#define STATUS_LED2_GPIO_PIN 		GPIO_Pin_7
	#define STATUS_LED2_ON					GPIO_SetBits(STATUS_LED_GPIO, STATUS_LED2_GPIO_PIN)
	#define STATUS_LED2_OFF					GPIO_ResetBits(STATUS_LED_GPIO, STATUS_LED2_GPIO_PIN)
	#define STATUS_LED2_TOGGLE			GPIO_WriteBit(STATUS_LED_GPIO, STATUS_LED2_GPIO_PIN, !GPIO_ReadOutputDataBit(STATUS_LED_GPIO, STATUS_LED2_GPIO_PIN))

	#define FREEZE_VALVE_CTRL_GPIO						GPIOB
	#define FREEZE_VALUE_CTRL_GPIO_RCC				RCC_APB2Periph_GPIOB

	#define FREEZE_VALUE_CTRL1_GPIO						GPIOB
	#define FREEZE_VALUE_CTRL1_GPIO_RCC				RCC_APB2Periph_GPIOB
	#define FREEZE_VALUE_CTRL1_GPIO_PIN				GPIO_Pin_15
	#define FREEZE_VALUE_CTRL1_START					GPIO_ResetBits(FREEZE_VALUE_CTRL1_GPIO, FREEZE_VALUE_CTRL1_GPIO_PIN)
	#define FREEZE_VALUE_CTRL1_SHUT						GPIO_SetBits(FREEZE_VALUE_CTRL1_GPIO, FREEZE_VALUE_CTRL1_GPIO_PIN)

	#define FREEZE_VALUE_CTRL2_GPIO						GPIOB
	#define FREEZE_VALUE_CTRL2_GPIO_RCC				RCC_APB2Periph_GPIOB
	#define FREEZE_VALUE_CTRL2_GPIO_PIN				GPIO_Pin_14
	#define FREEZE_VALUE_CTRL2_START					GPIO_ResetBits(FREEZE_VALUE_CTRL2_GPIO, FREEZE_VALUE_CTRL2_GPIO_PIN)
	#define FREEZE_VALUE_CTRL2_SHUT						GPIO_SetBits(FREEZE_VALUE_CTRL2_GPIO, FREEZE_VALUE_CTRL2_GPIO_PIN)

	#define FREEZE_VAULE_CTRL3_GPIO						GPIOB
	#define FREEZE_VALUE_CTRL3_GPIO_RCC				RCC_APB2Periph_GPIOB
	#define FREEZE_VALUE_CTRL3_GPIO_PIN				GPIO_Pin_13
	#define FREEZE_VALUE_CTRL3_START					GPIO_ResetBits(FREEZE_VAULE_CTRL3_GPIO, FREEZE_VALUE_CTRL3_GPIO_PIN)
	#define FREEZE_VALUE_CTRL3_SHUT						GPIO_SetBits(FREEZE_VAULE_CTRL3_GPIO, FREEZE_VALUE_CTRL3_GPIO_PIN)
	
	#define FREEZE_VALUE_CTRL4_GPIO						GPIOB
	#define FREEZE_VALUE_CTRL4_GPIO_RCC				RCC_APB2Periph_GPIOB
	#define FREEZE_VALUE_CTRL4_GPIO_PIN				GPIO_Pin_12
	#define FREEZE_VALUE_CTRL4_START					GPIO_ResetBits(FREEZE_VALUE_CTRL4_GPIO, FREEZE_VALUE_CTRL4_GPIO_PIN)
	#define FREEZE_VALUE_CTRL4_SHUT						GPIO_SetBits(FREEZE_VALUE_CTRL4_GPIO, FREEZE_VALUE_CTRL4_GPIO_PIN)

	#define FOOTSWITCH_INPUT_GPIO					GPIOB
	#define FOOTSWITCH_INPUT_GPIO_RCC			RCC_APB2Periph_GPIOB
	#define FOOTSWITCH_INPUT_CH_PIN			GPIO_Pin_0
	#define GET_FOOTSWITCH_STATUS()				GPIO_ReadInputDataBit(FOOTSWITCH_INPUT_GPIO, FOOTSWITCH_INPUT_CH_PIN)
	
	#define P4_SENSOR_SS_INPUT_GPIO					GPIOB
	#define P4_SENSOR_SS_INPUT_GPIO_RCC			RCC_APB2Periph_GPIOB
	#define P4_SENSOR_SS_INPUT_CH_PIN			GPIO_Pin_2
	#define GET_P4_SENSOR_SS_STATUS()				GPIO_ReadInputDataBit(P4_SENSOR_SS_INPUT_GPIO, P4_SENSOR_SS_INPUT_CH_PIN)
	
	typedef enum FOOTSWITCH_STATUS_E{
		FOOTSWITCH_STATUS_PRESSED = 0,
		FOOTSWITCH_STATUS_UNPRESSED = 1,
	}FOOTSWITCH_STATUS_e;

#else
//...........................................
#endif
/****************************************************************************
												ADC
****************************************************************************/
#define ADC1_OVERSAMPLE_RATE_HZ			10000			//10KHz					
#define ADC1_SAMPLE_RATE_HZ					500			//500Hz	
#define ADC_UREF_VAL  							3300 //5000mv Amplify 1000 times
	
#define ADC_USED					  ADC1
#define ADC_DR_Adress      (ADC1_BASE+0x4C)
#define ADC_PORT_RCC			 (RCC_APB2Periph_GPIOA)
#define ADC_PORT					 (GPIOA)
//#define ADC_PORT_PIN			 (GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6)
	#define ADC_PORT_PIN			 (GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_6)
#define ADC_RCC						 (RCC_APB2Periph_ADC1)
#define ADC_DMA_RCC				 (RCC_AHBPeriph_DMA1)
#define ADC_DMA_CHANNEL    (DMA1_Channel1)
#define ADC_CHANNEL_NUM			3
#define ADC_BUFF_NUM				(ADC1_OVERSAMPLE_RATE_HZ / ADC1_SAMPLE_RATE_HZ)

typedef enum PRESS_SENSOR_CHANNEL_VAL_E{
	PRESS_SENSOR_CHANNEL_3 = ADC_Channel_1,
	PRESS_SENSOR_CHANNEL_2 = ADC_Channel_2,
	PRESS_SENSOR_CHANNEL_1 = ADC_Channel_3,
	PRESS_SENSOR_CHANNEL_22 = ADC_Channel_6,
	VREFINT_CHANNEL = ADC_Channel_17,
}PRESS_SENSOR_CHANNEL_VAL_e;

typedef enum PRESS_SENSOR_REGULAR_GROUP_INDEX_E{
	PRESS_SENSOR_CHANNEL_1_INDEX = 1,
	PRESS_SENSOR_CHANNEL_22_INDEX ,
	//PRESS_SENSOR_CHANNEL_2_INDEX,
	PRESS_SENSOR_CHANNEL_3_INDEX,
	VREFINT_CHANNEL_INDEX,
}PRESS_SENSOR_REGULAR_GROUP_INDEX_e;

extern __IO uint8_t adcbufcnt;
extern __IO uint8_t adconvertflag;
extern __IO uint16_t ADChannelValBuf[ADC_CHANNEL_NUM][ADC_BUFF_NUM];
extern uint16_t ADChannelCurVal[ADC_CHANNEL_NUM];
extern uint16_t ADChannellastVal[ADC_CHANNEL_NUM];

extern void gInitAnalogADC(void);
/****************************************************************************
												IIC
****************************************************************************/
#define CPU_IIC_ADDRESS               (1)

#define IIC1_GPIO_RCC     						(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO) 
#define IIC1_GPIO_PIN     						(GPIO_Pin_8 | GPIO_Pin_9)
#define IIC1_GPIO         						GPIOB
#define IIC1_PERIPH_RCC   						RCC_APB1Periph_I2C1
#define IIC1_SPEED    								(400000)
#define IIC_NUM_USED_BY_P4_DAC				I2C1

#define ADJUST_VALUE_DAC_IIC_ADDRESS  (0xC0)
#define MCP4725_UREF_VAL  						3300 //5000mv Amplify 1000 times
#define ADC_UREFINT_VAL  							1200

//P4 Output 12 Digit
#define P4_SENSOR_IIC_ADDRESS					(0x28 << 1)

typedef enum IIC_DEVICE_STYLE_E{
  IIC_DEVICE_STYLE_MEMORY = 0,
  IIC_DEVICE_STYLE_MCP4725,
}IIC_DEVICE_STYLE_e;

extern void gInitIICPeriph(void);
extern void gSetMCP4725Voltage(uint16_t Vol);
extern void gSetMCP4725Dig(uint16_t dig);
extern uint16_t gGetP4Val(void);
/****************************************************************************
												TIM
****************************************************************************/
#define ADC1_TRIG_TIMER_PRESCALER		(uint16_t)(72)		
#define TIM2_PLCK1_CLK							(uint32_t)(72000000)								

#define	ADC1_TRIG_TIMER							TIM2
#define ADC1_TRIG_TIMER_PERIOD			(uint16_t)(TIM2_PLCK1_CLK / ADC1_OVERSAMPLE_RATE_HZ / ADC1_TRIG_TIMER_PRESCALER)			//x:us,>1us				
#define ADC1_TRIG_TIMER_RCC					RCC_APB1Periph_TIM2	

extern void InitADC1TrigTimer(void);

#define	MODBUS_HARD_TIMER							TIM3 
#define MODBUS_HARD_TIMER_RCC					RCC_APB1Periph_TIM3
#define MODBUS_HARD_TIMER_PRESCALER		(uint16_t)(3600)																									//50us
#define MODBUS_HARD_TIMER_PERIOD			(uint16_t)(TIM2_PLCK1_CLK / 1 / MODBUS_HARD_TIMER_PRESCALER)			//1Hz

#define MODBUS_HARD_TIMER_IRQ					TIM3_IRQn
#define MODTIMER_IRQHandler						TIM3_IRQHandler

extern void Init50usOcTimer(void);
/****************************************************************************
												HAL Callback
*****************************************************************************/
extern void HAL_GSMRX_EVENT(void *arg);
extern void HAL_GSMTX_EVENT(void *arg);
extern void HAL_TIMER_EVENT(void *arg);
extern void HAL_FAULT_EVENT(void *arg);
extern void HAL_ADCSAMPLE_EVENT(void *arg);
extern void HAL_FSRX_EVENT(void *arg);
extern void HAL_FSTX_EVENT(void *arg);

#endif
