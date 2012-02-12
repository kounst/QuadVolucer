// Header: 
// File Name: startup.c
// Author: Konstantin Reichmeyer
// Date: 27.04.10


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "pwm_states.h"
#include "bemf.h"
#include "Com_TIM.h"
#include "startup.h"
#include "pwm_TIM.h"

extern uint8_t  BEMF_active;
extern uint8_t  state;
extern uint16_t minpwm;
extern uint16_t  slopecount;
extern uint16_t period;
extern uint16_t speed;

uint16_t forced_period = 30000;

/* forces motor into a known position */
void positioning  (void)
{ 
  uint16_t pwm=POSITIONING_FORCE; 
  uint8_t i;

  start_state2();
  
  TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE);
  
  Com_enable();

  TIM_GenerateEvent(TIM1, TIM_EventSource_COM);

  for(i = 25; i>0; i--)
  {
    if(speed < minpwm)
      break;
    setPW(pwm);
    pwm+=2;
    Delay(30);
  }




  pwm=POSITIONING_FORCE;

  start_state();
  
  TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE);
  
  Com_enable();

  TIM_GenerateEvent(TIM1, TIM_EventSource_COM);

  for(i = 25; i>0; i--)
  {
    if(speed < minpwm)
      break;
    setPW(pwm);
    pwm+=2;
    Delay(30);
  }
}

/* starts the motor */
uint8_t startup(uint8_t mode)
{
  TIM_CtrlPWMOutputs(TIM1, ENABLE);

  positioning();
  //Delay(500);
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_13Cycles5);
  ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
  ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);

  //if(mode)
    forced_start_ramp();
  //else
  //  forced_start_push();

  //Delay(50);

//  //if((abs(period - 12500)) < 3000)
//  {
    //Delay(20);
    //setPW(minpwm);
    //setPW(0);
    if(speed > minpwm)
    {
      BEMF_active = 1;
      Delay(1000);
    }
    else
    {
      return 1;
    }
//  }
//  //else
//  {
//  //  return 1;
//  }
//
//  setPW(100);
//  Delay(500);

  return 0;
}

/* forces a single commutation step relying on the BEMF to kick in immediately */
void forced_start_push(void)
{
  uint16_t pwm = START_PUSH_FORCE;
  state = 1;
  
  setPW(pwm);
  slopecount = 0;

  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

  TIM_GenerateEvent(TIM1, TIM_EventSource_COM);

}

/* forces commutation upon the motor until the BEMF signal is big enough to be recoginized */
void forced_start_ramp(void)
{
  uint16_t pwm = START_RAMP_FORCE;
  int i;
  state = 1;
  forced_period = RAMP_INITIAL_PERIOD;
  
  setPW(pwm);
  slopecount = 0;

  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
  for(i = 427; i > 0; i--)
  {
    Delay(2);
    if(speed < minpwm)
      break;
    //forced_period -= 200;
    forced_period *= 0.995;
    
  }
  TIM_GenerateEvent(TIM1, TIM_EventSource_COM);

}



