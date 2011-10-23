// Header: Timer2 Configuration for 6-Step Commutation
// File Name: Com_TIM.c
// Author: Konstantin Reichmeyer
// Date:


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "Com_TIM.h"


void Com_TIM_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  /* TIM2 time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM2 prescaler configuration */
  TIM_PrescalerConfig(TIM2, 7, TIM_PSCReloadMode_Immediate);

  /* TIM2 Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1000;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* TIM2 IT enable */
  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* TIM2 disable counter */
  TIM_Cmd(TIM2, DISABLE);


}


void Com_enable(void)
{ 
  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
}


void Com_disable(void)
{ 
  /* TIM2 disable counter */
  TIM_Cmd(TIM2, DISABLE);
}
