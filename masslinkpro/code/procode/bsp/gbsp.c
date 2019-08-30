
#include "bsp.h"
#include "guser.h"

void ghwrccset(void)
{
	RCC_DeInit();
	RCC_LSICmd(ENABLE);
	RCC_HSICmd(DISABLE);
	RCC_HSEConfig(RCC_HSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	FLASH_SetLatency(FLASH_Latency_2);
	#if defined(STM32F103RC) || defined(STM32F103RB)
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
	#elif	defined(STM32F105RB)
		RCC_PLLConfig(RCC_PLLSource_PREDIV1,RCC_PLLMul_9);
	#endif
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	
	RCC_ClocksTypeDef RCC_ClocksVar;
	RCC_GetClocksFreq(&RCC_ClocksVar);
	SystemCoreClock = RCC_ClocksVar.HCLK_Frequency;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(SystemCoreClock/1000);
	//NVIC_SetPriority (SysTick_IRQn, 0);
}

void gnvicset(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_RAM,0x0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_GSM_PREPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_GSM_SUBPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXIT_CH0_7_PREPRIORITY;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXIT_CH0_7_SUBPRIORITY;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ADCDMA_PRIORITY_PREEMPTION;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = ADCDMA_PRIORITY_SUBPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void GenericGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//Board Status LED Init
	RCC_APB2PeriphClockCmd( STATUS_LED_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = STATUS_LED_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(STATUS_LED_GPIO, &GPIO_InitStructure);
	//GSM PWON Init
	RCC_APB2PeriphClockCmd( GSM_PWON_GPIO_RCC	, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GSM_PWON_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GSM_PWON_GPIO, &GPIO_InitStructure);
	//GSM RST 
	RCC_APB2PeriphClockCmd( GSM_RST_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GSM_RST_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GSM_RST_GPIO, &GPIO_InitStructure);
	//GSM Status:pull down for 64ms every second
//	RCC_APB2PeriphClockCmd( GSM_STATUS_GPIO_RCC, ENABLE);
//	GPIO_InitStructure.GPIO_Pin = GSM_STATUS_GPIO_PIN;	
//	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;       
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//	GPIO_Init(GSM_STATUS_GPIO, &GPIO_InitStructure);
	//Board Level Input Init
	RCC_APB2PeriphClockCmd( EXT_INPUT_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = EXT_INPUT_CH0_PIN | EXT_INPUT_CH1_PIN | EXT_INPUT_CH2_PIN | EXT_INPUT_CH3_PIN |\
																EXT_INPUT_CH4_PIN | EXT_INPUT_CH5_PIN | EXT_INPUT_CH6_PIN | EXT_INPUT_CH7_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(EXT_INPUT_GPIO, &GPIO_InitStructure);	
}
//static void gExitInit(void) __attribute__((used));
//static void gExitInit(void) 
//{
//	//EXIT Example
//	GPIO_InitTypeDef GPIO_InitStructure;
//	EXTI_InitTypeDef   EXTI_InitStructure;
//	RCC_APB2PeriphClockCmd(EXT_INPUT_GPIO_RCC, ENABLE);
//	
//	GPIO_InitStructure.GPIO_Pin = EXT_INPUT_CH0_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(EXT_INPUT_GPIO, &GPIO_InitStructure);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
//	
//	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

//}
void BspInit(void)
{
	GenericGpioInit();
	ginitusart();
	gInitAnalogADC();
}
