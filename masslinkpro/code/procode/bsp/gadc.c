#include "bsp.h"
#include "..\user\guser.h"

/***********************************************************************************/
//ADC Arithmetic Val
static uint16_t ADChannelOrginVal[ADC_CHANNEL_NUM];
uint16_t ADChannelValBuf[ADC_CHANNEL_NUM][ADC_BUFF_NUM];
//ADC Final Val
uint16_t ADChannelCurVal[ADC_CHANNEL_NUM];
uint16_t ADChannellastVal[ADC_CHANNEL_NUM];
//ADC Buf count
uint8_t adcbufcnt = 0;
uint8_t adconvertflag = 0;
/***********************************************************************************/

/**
	*Name					:gInitAnalogADC
	*Describe			:Bsp ADC Init
	*Input Arg		:None
	*Output Arg		:None
**/
void gInitAnalogADC(void)
{
	ADC_InitTypeDef	ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	/****************AD Port Init******************/
	RCC_APB2PeriphClockCmd(ADC_PORT_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADC_PORT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(ADC_PORT,&GPIO_InitStructure);
	/*****************DMA Config*******************/
	RCC_AHBPeriphClockCmd(ADC_DMA_RCC,ENABLE);
	DMA_DeInit(ADC_DMA_CHANNEL);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_DR_Adress;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADChannelOrginVal;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_CHANNEL_NUM;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMA_CHANNEL,&DMA_InitStructure);
	DMA_ITConfig(ADC_DMA_CHANNEL,DMA_IT_TC,ENABLE);
	DMA_Cmd(ADC_DMA_CHANNEL,ENABLE);
	/******************************ADC-CORRECT-CONFIG-ADC1*******************************/
	RCC_APB2PeriphClockCmd(ADC_RCC,ENABLE);
	ADC_DeInit(ADC_USED);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//work with multi-channel scan mode
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNEL_NUM;
	ADC_Init(ADC_USED,&ADC_InitStructure);
	/*****************ADC Regular Channel10:13 Config******************/
	ADC_RegularChannelConfig(ADC_USED,ANALOG_CHANNEL_0,ANALOG_CHANNEL_0_INDEX,ADC_SampleTime_41Cycles5 );
	ADC_RegularChannelConfig(ADC_USED,ANALOG_CHANNEL_1,ANALOG_CHANNEL_1_INDEX,ADC_SampleTime_41Cycles5 );
	ADC_RegularChannelConfig(ADC_USED,ANALOG_CHANNEL_2,ANALOG_CHANNEL_2_INDEX,ADC_SampleTime_41Cycles5 );
	ADC_RegularChannelConfig(ADC_USED,ANALOG_CHANNEL_3,ANALOG_CHANNEL_3_INDEX,ADC_SampleTime_41Cycles5 );
	//ADC_ITConfig(ADC_USED,ADC_IT_EOC,ENABLE);
	ADC_Cmd(ADC_USED,ENABLE);
	ADC_DMACmd(ADC_USED,ENABLE);
	/************************************************/
	ADC_ResetCalibration(ADC_USED);
	while(ADC_GetResetCalibrationStatus(ADC_USED));
	ADC_StartCalibration(ADC_USED);
	while(ADC_GetCalibrationStatus(ADC_USED));
	ADC_SoftwareStartConvCmd(ADC_USED,ENABLE);
}
/**
	*Name					:DMA1_Channel1_IRQHandler
	*Describe			:DMA1 IT Handler
	*Input Arg		:None
	*Output Arg		:None
**/
void DMA1_Channel1_IRQHandler(void)
{
	uint8_t loop;
	uint32_t criflag;
	//通道1传输完成中断TC 还有传输过半中断HT 错误中断TE	全局中断GL
	if(DMA_GetITStatus(DMA1_IT_TC1))
	{
		ADC_Cmd(ADC1,DISABLE);
		for(loop = 0;loop < ADC_CHANNEL_NUM;loop++)
		{
			ADChannelValBuf[loop][adcbufcnt] = ADChannelOrginVal[loop];
		}
		criflag = gentercritical();
		adcbufcnt++;
		adconvertflag = 1;
		gleavecritical(criflag);
		DMA_ClearITPendingBit(DMA1_IT_GL1);
	}
}
