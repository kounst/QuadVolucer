// Header:
// File Name: main.c
// Author: Konstantin Reichmeyer
// Date:
 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "startup.h"
#include "GPIO.h"
#include "Com_TIM.h"
#include "pwm_TIM.h"
#include "ADC.h"
#include "bemf.h"
#include "CAN.h"
#include <stdio.h>
#include "stdlib.h"
#include <math.h>
#include "beep.h"
#include "eeprom.h"





/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern CanTxMsg TxMessage;

volatile uint16_t speed=0;
volatile uint16_t current=0;
volatile uint16_t period;
extern uint8_t BEMF_active;
uint16_t rpm=0;
uint16_t minpwm = MINPWM;
uint16_t maxpwm = MAXPWM;
uint16_t address = 0;
uint32_t idle_song = IDLE;
extern uint32_t flashADDRESS;
 

extern uint16_t slopecount;

enum mode
{
  off,
  start,
  stop,
  running
}op_mode = off;


extern uint16_t angle;
extern uint8_t startmode;


/* Virtual address defined by the user: 0xFFFF value is prohibited */
extern uint16_t VirtAddVarTab[NumbOfVar];


static __IO uint32_t TimingDelay;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void SysTick_Configuration(void);
void NVIC_Configuration(void);
void readeeprom(void);



/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)

{    
  /* System Clocks Configuration */
  RCC_Configuration();

  /* NVIC Configuration */
  NVIC_Configuration();

  /* GPIO Configuration */
  GPIO_Configuration();

  /* SysTick Configuration */
  SysTick_Configuration();

  /* Configuration of ADC to detect BEMF zero crossings */
  Com_ADC_Configuration();

  ADC_Current_Configuration();

  /* Comutation Timer Configuration */
  Com_TIM_Configuration();
  
  /* TIM_Configuration PWM*/
  pwm_TIM_Configuration();
  
  
  /* Unlock the Flash Program Erase controller */
  FLASH_Unlock();

  /* EEPROM Init */
  EE_Init();

  FLASH_Lock();

  readeeprom();

  /* CAN1 Configuration */
  CAN_Configuration();
  /* IT Configuration for CAN1 */  
  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

  startbeep();

  Delay((address+1) * 200);
  beep(700,200);
 

  while (1)
  {
    switch (op_mode)
    {
      case off:
        ConfigMessageOff();
        if(speed > minpwm)
        {
          op_mode = start;

        }
        break;

      case start:
        startup(startmode);

        op_mode = running;
        setPW(speed);

        break;

      case stop:
        Com_disable();
        BEMF_active = 0;
        setPW(0);
        rpm = 0;
        op_mode = off;

        idle_song = IDLE;
        break;

      case running:
        ConfigMessageOn();
        
        if(speed < minpwm)
        {
          op_mode = stop;
        }

        break;

      default:
        op_mode = off;

        break;
      
    }

    if(idle_song == 1)
    {
      idle(0);                   //play random song
      idle_song = IDLE;          //reset countdown
    }

  }
}


/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
     initialize the PLL and update the SystemFrequency variable. */
  SystemInit();

   /* PCLK1 = HCLK/2 */
  RCC_PCLK1Config(RCC_HCLK_Div2);

  RCC_ADCCLKConfig(RCC_PCLK2_Div4);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |  RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* ADC1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* ADC2 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
  
  /* CAN1 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
}


/* reads stored parameters from the emulated eeprom(flash) */
void readeeprom(void)
{
  if(!EE_ReadVariable(flashminpwm, &minpwm))        //if variable exists
    ;
  else                                              //else set to default
    minpwm = MINPWM;                                    

  if(!EE_ReadVariable(flashmaxpwm, &maxpwm))        //if variable exists
    ;
  else                                              //else set to default
    maxpwm = MAXPWM;
  if(!EE_ReadVariable(flashaddress, &address))      //if variable exists
    ;
  else                                              //else set to default
    address = ADDRESS;
  if(!EE_ReadVariable(flashangle, &angle))          //if variable exists
    ;
  else                                              //else set to default
    angle = ANGLE;
  TxMessage.StdId = 0x010 | (address+1);
}



/**
  * @brief  Configures the SysTick.
  * @param  None
  * @retval None
  */
void SysTick_Configuration(void)
{
  
  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }

  
}


/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  /* Enable the TIM2 gloabal interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM1 Commutation Interrupt */
  /* is triggered by TIM2 interrupt*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_TRG_COM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 

  /* Enable TIM1 CC interrupt */
  /* triggeres ADC conversion */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure and enable ADC interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable CAN1 RX interrupt */ 
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  /* SysTick Priority */
 NVIC_SetPriority(SysTick_IRQn, 4);
}




#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while (1)
  {}
}
#endif
/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
