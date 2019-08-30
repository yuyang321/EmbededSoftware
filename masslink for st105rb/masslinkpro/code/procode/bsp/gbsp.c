
#include "bsp.h"

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
}

void gnvicset(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_RAM,0x0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_GSM_PREPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_GSM_SUBPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void GenericGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = STATUS_LED_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(STATUS_LED_GPIO, &GPIO_InitStructure);
}
void BspInit(void)
{
	GenericGpioInit();
	ginitusart();
}
