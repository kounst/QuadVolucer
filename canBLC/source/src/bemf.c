// Header: BEMF zero crossing detection 
// File Name: bemf.c
// Author: Konstantin Reichmeyer
// Date:



/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "bemf.h"


volatile uint16_t angle=4; // 30°/angle = 15°
uint16_t com_period;
uint8_t BEMF_active = 0;
uint16_t slopecount =0;
extern uint16_t curr_count;

void next_commutation (uint16_t edge)
{
  uint16_t new_bemf_sample;

  uint16_t last_com;
  
 
  static uint8_t last_cond_rise;
  static uint8_t last_cond_fall; 

  new_bemf_sample = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
  //GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_7)));

  if(edge == 0)  // 0 is falling edge
  {
    if(new_bemf_sample == 0 && last_cond_fall > 1)
    {
      slopecount++;

      last_com = TIM_GetCapture1(TIM2);

      com_period = (curr_count - last_com);
      
      ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);

      if(BEMF_active)
        TIM_SetCompare1(TIM2,curr_count + com_period / angle);

      last_cond_fall = 0;

    }
    else
    {
      last_cond_fall++;
    }
  }
  else          // rising edge
  {
    if(new_bemf_sample > 25 && last_cond_rise > 1)
    {
      slopecount++;

      last_com = TIM_GetCapture1(TIM2);
      //com_period = 2*(curr_count - last_com);
      com_period = (curr_count - last_com);

      ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);

      if(BEMF_active)
        TIM_SetCompare1(TIM2,curr_count + com_period / angle);

      last_cond_rise = 0;

    }
    else
    {
      last_cond_rise++;
    }
  }
}


