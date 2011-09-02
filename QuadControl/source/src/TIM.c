// Header:
// File Name: 
// Author:
// Date:



/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "TIM.h"

void TIM3_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;


  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 5;//(uint16_t) (SystemCoreClock / 24000000) - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1500;       //2kHz
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);

//  /* PWM1 Mode configuration: Channel3 */
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OCInitStructure.TIM_Pulse = 4500;
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
//
//  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
//
//  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
//
//  TIM_ARRPreloadConfig(TIM3, ENABLE);

  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}
