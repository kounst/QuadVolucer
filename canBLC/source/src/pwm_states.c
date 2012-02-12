// Header: PWM_states of 6-Step Commutation
// File Name: pwm_states.c
// Author: Konstantin Reichmeyer
// Date:


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "pwm_states.h"
#include "main.h"
#include "Com_TIM.h"
#include "pwm_TIM.h"


extern uint8_t BEMF_active;
uint16_t edge;
uint8_t new_current = 0;

volatile uint8_t state = 1;

/* 6-Step commutation scheme */
/*  |  1  |  2  |  3  |  4  |  5  |  6  |
 * -+-----+-----+-----+-----+-----+-----+
 * A| --- |  /  | pwm | pwm |  \  | --- |
 * -+-----+-----+-----+-----+-----+-----+
 * B| pwm | pwm |  \  | --- | --- |  /  |
 * -+-----+-----+-----+-----+-----+-----+
 * C|  \  | --- | --- |  /  | pwm | pwm |
 * -+-----+-----+-----+-----+-----+-----+
 *  |     |     |     |     |     |     |
 *  0°    60°   120°  180°  240°  300°  360°  
*/


/* ---Timer channel configuration functions--- */
void set_____high    (u16 channel)
{
  TIM_SelectOCxM(TIM1, channel, TIM_ForcedAction_Active );
  TIM_CCxCmd(TIM1, channel, TIM_CCx_Enable);
  TIM_CCxNCmd(TIM1, channel, TIM_CCxN_Enable);
}

void set_____off     (u16 channel)
{
  TIM_CCxCmd(TIM1, channel, TIM_CCx_Disable);
  TIM_CCxNCmd(TIM1, channel, TIM_CCxN_Disable);
}

void set_pwm_low     (u16 channel)
{
  TIM_SelectOCxM(TIM1,channel, TIM_OCMode_PWM1 );
  TIM_CCxCmd(TIM1, channel, TIM_CCx_Disable);
  TIM_CCxNCmd(TIM1, channel, TIM_CCxN_Enable);
}

void set_pwm_high   (u16 channel)
{
  TIM_SelectOCxM(TIM1,channel, TIM_OCMode_PWM1 );
  TIM_CCxCmd(TIM1, channel, TIM_CCx_Enable);
  TIM_CCxNCmd(TIM1, channel, TIM_CCxN_Enable);      //active freewheeling
  //instead of using the parasitc diode of the MOSFET for freewheeling its being turned on
}

void set_____low    (u16 channel)
{
  TIM_SelectOCxM(TIM1, channel, TIM_ForcedAction_InActive );
  TIM_CCxCmd(TIM1, channel, TIM_CCx_Enable);
  TIM_CCxNCmd(TIM1, channel, TIM_CCxN_Enable); 
}


/* ----------------------------------------------------------- */
/* State in between of state 6 and 1 */
void start_state(void)
{
  set_____low(TIM_Channel_1);
  set_pwm_high(TIM_Channel_2);
  set_pwm_high(TIM_Channel_3);
}

void start_state2(void)
{
  set_____low(TIM_Channel_1);
  set_____off(TIM_Channel_2);
  set_pwm_high(TIM_Channel_3);
}


void test_high(void)
{
  set_____high(TIM_Channel_1);
  set_____high(TIM_Channel_2);
  set_____high(TIM_Channel_3);
}

void test_low(void)
{
  set_____low(TIM_Channel_1);
  set_____low(TIM_Channel_2);
  set_____low(TIM_Channel_3);
}

void test_pwm_h(void)
{
  set_pwm_high(TIM_Channel_1);
  set_pwm_high(TIM_Channel_2);
  set_pwm_high(TIM_Channel_3);
}

void test_pwm_l(void)
{
  set_pwm_low(TIM_Channel_1);
  set_pwm_low(TIM_Channel_2);
  set_pwm_low(TIM_Channel_3);
}



/* 6-Step commutation channel configuration */
void pwm_states(void)
{

  switch(state)
  {
    case 1:

      //GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5)));

      set_____low(TIM_Channel_1);
      set_pwm_high(TIM_Channel_2);
      set_____off(TIM_Channel_3);

      //Set ADC channel for BEMF zero detection
      //ADC channel for step6!!
      edge = state%2;

      ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);
      ADC_InjectedChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_13Cycles5);
      ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
      

      state++;

      break;

    case 2:

      //GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5)));

      set_____off(TIM_Channel_1);
      set_pwm_high(TIM_Channel_2);
      set_____low(TIM_Channel_3);

      //Set ADC channel for BEMF zero detection
      //ADC channel for step1!!
      edge = state%2;

      ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);
      ADC_InjectedChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_13Cycles5);
      ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);

      state++;

      break;

    case 3:

      set_pwm_high(TIM_Channel_1);
      set_____off(TIM_Channel_2);
      set_____low(TIM_Channel_3);

      //Set ADC channel for BEMF zero detection
      //ADC channel for step2!!
      edge = state%2;

      ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);
      ADC_InjectedChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_13Cycles5);
      ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);

      state++;

      break;

    case 4:


      set_pwm_high(TIM_Channel_1);
      set_____low(TIM_Channel_2);
      set_____off(TIM_Channel_3);

      //Set ADC channel for BEMF zero detection
      //ADC channel for step3!!
      edge = state%2;

      ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);
      ADC_InjectedChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_13Cycles5);
      ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
            
      state++;

      break;

    case 5:

      set_____off(TIM_Channel_1);
      set_____low(TIM_Channel_2);
      set_pwm_high(TIM_Channel_3);

      //Set ADC channel for BEMF zero detection
      //ADC channel for step4!!
      edge = state%2;

      ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);
      ADC_InjectedChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_13Cycles5);
      ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);

      state++;

      break;

    case 6:

      set_____low(TIM_Channel_1);
      set_____off(TIM_Channel_2);
      set_pwm_high(TIM_Channel_3);

      //Set ADC channel for BEMF zero detection
      //ADC channel for step5!!
      edge = state%2;

      ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);
      ADC_InjectedChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_13Cycles5);
      ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);

      state = 1;

      break;

    default:
      Com_disable();
      BEMF_active = 0;
      setPW(0);
      
      break;
  }
}
