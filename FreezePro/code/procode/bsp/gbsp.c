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
  RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_LCD_PREPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_LCD_SUBPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_FSENSOR_PREPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_FSENSOR_SUBPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ADCDMA_PRIORITY_PREEMPTION;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = ADCDMA_PRIORITY_SUBPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = MODBUS_HARD_TIMER_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = MODTIMER_PRIORITY_PREEMPTION;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = MODTIMER_PRIORITY_SUBPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

static void GenericGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//Board Status LED Init
	RCC_APB2PeriphClockCmd( STATUS_LED_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = STATUS_LED1_GPIO_PIN | STATUS_LED2_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(STATUS_LED_GPIO, &GPIO_InitStructure);
	//Board VALUES GPIO Init
	RCC_APB2PeriphClockCmd( FREEZE_VALUE_CTRL_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = FREEZE_VALUE_CTRL1_GPIO_PIN | FREEZE_VALUE_CTRL2_GPIO_PIN | FREEZE_VALUE_CTRL3_GPIO_PIN \
																| FREEZE_VALUE_CTRL4_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(FREEZE_VALVE_CTRL_GPIO, &GPIO_InitStructure);
	FREEZE_VALUE_CTRL1_SHUT;
	FREEZE_VALUE_CTRL2_SHUT;
	FREEZE_VALUE_CTRL3_SHUT;
	FREEZE_VALUE_CTRL4_SHUT;
	//Board footswitch GPIO Init
	RCC_APB2PeriphClockCmd( FOOTSWITCH_INPUT_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = FOOTSWITCH_INPUT_CH_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(FOOTSWITCH_INPUT_GPIO, &GPIO_InitStructure);
	//Board P4 Sensor SS GPIO Init
	RCC_APB2PeriphClockCmd( P4_SENSOR_SS_INPUT_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = P4_SENSOR_SS_INPUT_CH_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(P4_SENSOR_SS_INPUT_GPIO, &GPIO_InitStructure);
}

void BspInit(void)
{
	GenericGpioInit();
	ginitusart();
	gInitAnalogADC();
	Init50usOcTimer();
	gInitIICPeriph();
}
