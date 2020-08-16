#include "stm32f10x.h"


TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
void TIM2_Init(void);
void TIM4_Init(void);

/*
TIM2 : interrupt 사용 (LED, 기타 플래그 )
TIM3 : 기타 딜레이 함수에 사용
TIM4 : 온도센서 딜레이 추가에 사용(자간딜레이)
*/
void TIM_Config(void)
{
/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM2_Init();
//	TIM4_Init();
}

void TIM2_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	uint16_t PrescalerValue = 0;
	
  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	

	
  /* ---------------------------------------------------------------------------
	TIM2 Configuration: TIM_CounterMode_Down:
	TIM2CLK = 72 MHz, Prescaler = 7199, TIM2 counter clock = 10 KHz, Period = 9
  ----------------------------------------------------------------------------*/
	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 9;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	
		/* Compute the prescaler value */
//	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 10000) - 1;
	PrescalerValue = (uint16_t) ( (SystemCoreClock) / 10000) - 1;
	
	
	/* Prescaler configuration */
  TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);
	
	 /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	
	
  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);
	
 
}


void TIM4_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	uint16_t PrescalerValue = 0;
	
  /* Enable the TIM4 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	
	
  /* ---------------------------------------------------------------------------
	TIM4 Configuration: TIM_CounterMode_Down:
	TIM4 CLK = 72 MHz, Prescaler = 7199, TIM2 counter clock = 10 KHz, Period = 9
  ----------------------------------------------------------------------------*/
	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 9;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) ( (SystemCoreClock) / 10000) - 1;
	
	
	/* Prescaler configuration */
  TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);
	
	
  /* TIM enable counter */
  TIM_Cmd(TIM4, ENABLE);
	
  /* TIM IT enable */
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
}

void TIM_Delay_ms(unsigned int time)
{
	uint16_t PrescalerValue = 0;
	unsigned int i;

	
  /* ---------------------------------------------------------------------------
	TIM3 Configuration: TIM_CounterMode_Down:
	TIM3CLK = 72 MHz, Prescaler = 7199, TIM3 counter clock = 10 KHz, Period = 9
  ----------------------------------------------------------------------------*/
	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 9;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
		/* Compute the prescaler value */
//	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 10000) - 1;
	PrescalerValue = (uint16_t) ( (SystemCoreClock) / 10000) - 1;
	
	
	/* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
	
	
  /* TIM enable counter */
	for(i= 0 ; i < time ; i++)
	{
		TIM_Cmd(TIM3, ENABLE);
		while( TIM_GetFlagStatus(TIM3,TIM_FLAG_Update) == RESET ){} ;
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	}
	TIM_Cmd(TIM3, DISABLE);
}

void TIM_Delay_us(unsigned int time)
{
	uint16_t PrescalerValue = 0;
	unsigned int i;

	
  /* ---------------------------------------------------------------------------
	TIM3 Configuration: TIM_CounterMode_Down:
	TIM3CLK = 72 MHz, Prescaler = 71, TIM3 counter clock = 1 MHz, Period = 0
  ----------------------------------------------------------------------------*/
	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 0;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
		/* Compute the prescaler value */
//	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 10000) - 1;
	PrescalerValue = (uint16_t) ( (SystemCoreClock) / 1000000) - 1;
	
	
	/* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
	
	
  /* TIM enable counter */
	for(i= 0 ; i < time ; i++)
	{
		TIM_Cmd(TIM3, ENABLE);
		while( TIM_GetFlagStatus(TIM3,TIM_FLAG_Update) == RESET ){} ;
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	}
	TIM_Cmd(TIM3, DISABLE);
}