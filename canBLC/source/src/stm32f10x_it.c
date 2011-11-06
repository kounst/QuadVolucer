// Header: This file provides templates for all exceptions handler and peripherals
//         interrupt service routine.
// File Name: stm32f10x_it.c
// Author: Konstantin Reichmeyer
// Date:



/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "pwm_states.h"
#include "bemf.h"
#include "main.h"
#include "Com_TIM.h"
#include "pwm_TIM.h"
#include "CAN.h"


/* Private variables ---------------------------------------------------------*/
extern uint16_t edge;
extern uint16_t forced_period;
extern uint8_t BEMF_active;
extern uint16_t current;
extern uint16_t period;



/* CAN related variables */
extern uint16_t address;
extern uint16_t minpwm;
extern uint16_t maxpwm;
extern uint8_t  angle;
extern uint16_t speed;
extern uint16_t rpm;
uint16_t curr_count;

uint8_t canlog = 1;
uint16_t cantimer;             
uint16_t timeout;              //CAN speed frame timeout in ms
volatile uint8_t newConfig = 0;
extern uint16_t slopecount;

CanRxMsg RxMessage, RxConfigMessage;
CanTxMsg TxMessage;

extern uint32_t idle_song;


enum mode
{
  off,
  start,
  stop,
  running
};
extern enum mode op_mode;



/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  static uint8_t prescaler = CANLOGprescaler;

  #ifdef current_protection
  if(op_mode == off)
  {
    current = 0;
  }
  else
  {
    current = 0.95 * current + ADC_GetConversionValue(ADC2);
  }
  if(current > 12000 && maxpwm > 100)
  {
    maxpwm -= 100;
    setPW(speed);
  }
  if(current < 8000 && maxpwm < 1024)
  {
    maxpwm += 100;
    setPW(speed);
  }
  #endif

  if(canlog)
  {
    if(prescaler == 0)
    {
      prescaler = 100;
      //current =  ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_1);
      //current = 0.95 * current + ADC_GetConversionValue(ADC2);
      TxMessage.Data[0] = (current >> 8);
      TxMessage.Data[1] =  current;
      TxMessage.Data[2] =  maxpwm>>8;
      TxMessage.Data[3] =  maxpwm;
      TxMessage.Data[4] = (rpm>>8);              // RPM high byte
      TxMessage.Data[5] = rpm;                   // RPM low byte
      CAN_Transmit(CAN1, &TxMessage);
    }
    prescaler--;


  }

  TimingDelay_Decrement();  //Decrement ms counter of Delay() function

  cantimer--;
  if(!cantimer)           //turn off motor if CAN speed frame has timed out
    speed = 0;

  if(idle_song-1)
    idle_song--;
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/


/**
  * @brief  This function handles ADC1 and ADC2 global interrupts requests.
  * @param  None
  * @retval None
  * Interrupt is triggered when ADC conversion is done
  */
void ADC1_2_IRQHandler(void)
{
  if( ADC_GetITStatus(ADC1, ADC_IT_JEOC) != RESET)
  {
    /* Clear ADC1 JEOC pending interrupt bit */
    ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
  
    next_commutation(edge); //checks if BEMF zero crossing has occured
  }

  if( ADC_GetITStatus(ADC2, ADC_IT_EOC) != RESET)
  {
    /* Clear ADC1 EOC pending interrupt bit */
    ADC_ClearITPendingBit(ADC2, ADC_IT_EOC);

    //current = ADC_GetConversionValue(ADC2);
//    if(current < new_current)
//      current++;
//    else
//      current--;

    //GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5)));
  }
}



/**
  * @brief  This function handles TIM1 Trigger and commutation interrupts
  *   requests.
  * @param  None
  * @retval None
  * Interrupt triggers pwm_state change to the by pwm_states() configured state
  */
void TIM1_TRG_COM_IRQHandler(void)
{
  static uint16_t comtime, last_comtime;

  /* Clear TIM1 COM pending bit */
  TIM_ClearITPendingBit(TIM1, TIM_IT_COM);

  comtime = TIM_GetCounter(TIM2);
  period = (comtime - last_comtime);
  last_comtime = comtime;

  pwm_states();        //configures adc channel for current commutation step
                       //and configures TIM1 registers for next step
 
}


/**
  * @brief  This function handles TIM1 CC interrupts
  *   requests.
  * @param  None
  * @retval None
  * Triggers ADC Conversion for BEMF zero crossing detection
  */
void TIM1_CC_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_CC4) != RESET)
  {
    /* Clear TIM1 CC pending bit */
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC4);
    curr_count = TIM_GetCounter(TIM2);
  }
  if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
  {
    /* Clear TIM1 CC pending bit */
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
    //GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5)));
  }
}


void TIM1_UP_IRQHandler(void)
{
  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
  //GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5)));
  //GPIO_WriteBit(GPIOA, GPIO_Pin_5, 0);
}




/**
  * @brief  This function handles TIM2 CC interrupts
  *   requests.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
  uint16_t comm_tim_capture;

  if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
  {
    /* Clear TIM2 CC pending bit */
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

    /* Generate TIM1 COM event by software */
    TIM_GenerateEvent(TIM1, TIM_EventSource_COM);       //Generate TIM1 Commuation event to change to next state

    if(!BEMF_active)           //force commutation if BEMF is inactive
    {
      comm_tim_capture = TIM_GetCapture1(TIM2);
      TIM_SetCompare1(TIM2, comm_tim_capture + forced_period);
    }
  }

}



/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{}


/**
  * @brief  This function handles CAN1 RX0 Handler.
  * @param  None
  * @retval None
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{

  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

  CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
  CAN_ClearITPendingBit(CAN1, CAN_IT_FOV0);
  CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP0);
  

  if ((RxMessage.StdId == 0x001)&&(RxMessage.IDE == CAN_ID_STD)&&(RxMessage.DLC == 8))              //valid speed frame?
  {
    speed = 4 * RxMessage.Data[address];

    if(BEMF_active)
    {
      setPW(speed);
    }
    cantimer = timeout;                                                                           //reset CAN timeout
  }
  
  if ((RxMessage.StdId == 0x100)&&(RxMessage.IDE == CAN_ID_STD)&&(RxMessage.DLC == 8))              //valid config frame?
  {
    RxConfigMessage = RxMessage;
    newConfig = 1;                                                                                  //set newConfig frame flag
  }
}




/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

