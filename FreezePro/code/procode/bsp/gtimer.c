#include "bsp.h"
#include "guser.h"	
#include "..\mid\gmodbus.h"

void InitADC1TrigTimer(void)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef TIME_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(ADC1_TRIG_TIMER_RCC,ENABLE);
	TIM_Cmd(ADC1_TRIG_TIMER,DISABLE);
  /*           TIME base configuration            */
  TIME_TimeBaseStructure.TIM_Period = ADC1_TRIG_TIMER_PERIOD;
  TIME_TimeBaseStructure.TIM_Prescaler = ADC1_TRIG_TIMER_PRESCALER - 1;
  TIME_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIME_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(ADC1_TRIG_TIMER,&TIME_TimeBaseStructure);
  /*   PWM1 Mode configuration:   */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = TIME_TimeBaseStructure.TIM_Period / 2;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC2Init(ADC1_TRIG_TIMER,&TIM_OCInitStructure);
}

void Init50usOcTimer(void)
{
  TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIME_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(MODBUS_HARD_TIMER_RCC,ENABLE);
	TIM_Cmd(MODBUS_HARD_TIMER,DISABLE);
	/*           TIME base configuration            */
  TIME_TimeBaseStructure.TIM_Period = MODBUS_HARD_TIMER_PERIOD;	//1s
  TIME_TimeBaseStructure.TIM_Prescaler = MODBUS_HARD_TIMER_PRESCALER - 1;
  TIME_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIME_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIME_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(MODBUS_HARD_TIMER,&TIME_TimeBaseStructure);
	/* CC Common Config	*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	/*   CC1 Config   */
	TIM_OCInitStructure.TIM_Pulse = 100;
	TIM_OC1Init(MODBUS_HARD_TIMER,&TIM_OCInitStructure);
	TIM_OC1PreloadConfig(MODBUS_HARD_TIMER,TIM_OCPreload_Disable);
	TIM_ClearITPendingBit(MODBUS_HARD_TIMER, TIM_IT_CC1);
	/*   CC2 Config   */
	TIM_OCInitStructure.TIM_Pulse = 300;
	TIM_OC2Init(MODBUS_HARD_TIMER,&TIM_OCInitStructure);
	TIM_OC2PreloadConfig(MODBUS_HARD_TIMER,TIM_OCPreload_Disable);
	TIM_ClearITPendingBit(MODBUS_HARD_TIMER, TIM_IT_CC2);
	/*   CC3 Config   */
	TIM_OCInitStructure.TIM_Pulse = 600;
	TIM_OC3Init(MODBUS_HARD_TIMER,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(MODBUS_HARD_TIMER,TIM_OCPreload_Disable);
	TIM_ClearITPendingBit(MODBUS_HARD_TIMER, TIM_IT_CC3);
	
	TIM_ARRPreloadConfig(MODBUS_HARD_TIMER,ENABLE);
	TIM_Cmd(MODBUS_HARD_TIMER,ENABLE);
}
	
void MODTIMER_IRQHandler(void)
{
	if (TIM_GetITStatus(MODBUS_HARD_TIMER, TIM_IT_CC1))
	{
			TIM_ClearITPendingBit(MODBUS_HARD_TIMER, TIM_IT_CC1);
			TIM_ITConfig(MODBUS_HARD_TIMER,TIM_IT_CC1,DISABLE);
			mbflag.mbrframeflag = RESET;
	}
	if(TIM_GetITStatus(MODBUS_HARD_TIMER, TIM_IT_CC2))
	{
			TIM_ClearITPendingBit(MODBUS_HARD_TIMER, TIM_IT_CC2);
			TIM_ITConfig(MODBUS_HARD_TIMER,TIM_IT_CC2,DISABLE);
			mbflag.mbtframeflag = RESET;
	}
	if(TIM_GetITStatus(MODBUS_HARD_TIMER, TIM_IT_CC3))
	{
			TIM_ClearITPendingBit(MODBUS_HARD_TIMER, TIM_IT_CC3);
			TIM_ITConfig(MODBUS_HARD_TIMER,TIM_IT_CC3,DISABLE);
			mbflag.mbacktimeoutcheckflag = RESET;
	}
}

#if(1 == 0)
	#define PWM_PORT
	#define PWM_PINA
	#define PWM_PORT_RCC
	static void InitPwmTimerGpio(void)
	{
			GPIO_InitTypeDef GPIO_InitStructure;
			/*Init PWM PIN*/
			RCC_ARPB2PeriphClockCmd(PWM_PORT_RCC,ENABLE);
			GPIO_InitStructure.GPIO_Pin = PWM_PINA;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(PWM_PORT,&GPIO_InitStructure);
	}
	
	#define PWM_TIMER TIM3
	#define PWM_TIMER_RCC
	#define PWM_TIMER_PERIOD
	#define PWM_DUTY
	static void InitPwmTimer(void)
	{
		TIM_OCInitTypeDef TIM_OCInitStructure;
		TIM_TimeBaseInitTypeDef TIME_TimeBaseStructure;
		RCC_APB1PeriphClockCmd(PWM_TIMER_RCC,ENABLE);
		/*           TIME base configuration            */
		TIME_TimeBaseStructure.TIM_Period = PWM_TIMER_PERIOD;
		TIME_TimeBaseStructure.TIM_Prescaler = 0;
		TIME_TimeBaseStructure.TIM_ClockDivision = 0;
		TIME_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(PWM_TIMER,&TIME_TimeBaseStructure);
		/*   PWM1 Mode configuration:Channel2(PA1 out)   */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = PWM_DUTY;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC2Init(PWM_TIMER,&TIM_OCInitStructure);
		TIM_OC2PreloadConfig(PWM_TIMER,TIM_OCPreload_Enable);
		TIM_ARRPreloadConfig(PWM_TIMER,ENABLE);

		TIME_Cmd(PWM_TIMER,ENABLE);
	}

	static void InitSynTimer(void)
	{
			TIM_TimeBaseInitTypeDef TIME_TimeBaseStructure;
			RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

			TIME_TimeBaseStructure.TIM_Period = 1000-1;
			TIME_TimeBaseStructure.TIM_Prescaler = 72-1;
			TIME_TimeBaseStructure.TIM_ClockDivision = 0;
			TIME_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
			TIME_TimeBaseStructure.TIM_RepetitionCounter = 0;
			TIM_TimeBaseInit(TIM1,&TIME_TimeBaseStructure);
			//TIM_ARRPreloadConfig(TIM1,ENABLE);
			TIM_ClearFlag(TIM1,TIM_FLAG_Update);
			TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
			//TIM_Cmd(TIM1,ENABLE);
	}
	//ISR

#endif
