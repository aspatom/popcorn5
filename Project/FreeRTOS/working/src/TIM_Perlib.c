#include "stm32f4_discovery.h"
#include "TIM_Perlib.h"


// TImer Interupt   1us 
void TIM_IT_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
		/* Enable the TIM5 gloabal Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	
	
	
  	/* TIM5 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	/* Time base configuration */

  	TIM_TimeBaseStructure.TIM_Period = 283;					// 1us
  	TIM_TimeBaseStructure.TIM_Prescaler = 3;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	 /* TIM IT enable */
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	
	/* TIM2 enable counter */
	TIM_Cmd(TIM5, ENABLE);
}



// TImer Interupt   10ms 
void TIM2_IT_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
		/* Enable the TIM2 gloabal Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	
	
	
  	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	/* Time base configuration */

  	TIM_TimeBaseStructure.TIM_Period = 5599;					// 10ms
  	TIM_TimeBaseStructure.TIM_Prescaler = 150;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	 /* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}

