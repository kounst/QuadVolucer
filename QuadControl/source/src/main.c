// Header:
// File Name: main.c
// Author: Konstantin Reichmeyer
// Date:
 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "GPIO.h"
#include "ADC.h"
#include "eeprom.h"
#include "TIM.h"
#include "I2C.h"
#include "mixer.h"
#include "stdio.h"


u8 rcdsl_battery;
u8 rcdsl_allocation;
volatile u8 rcdsl_RSSI = 0;

union pulsw pulswidth;
union channels neutral;
struct props gas = {0,0,0,0};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t buf[10];                                    //I2C_buffer;
int16_t GyroX, GyroY, GyroZ, Temp;
int16_t GyroXNeutral, GyroYNeutral, GyroZNeutral;
volatile uint16_t gyro_reverse;                     //bit 0: GyroX, bit 1: GyroY, bit 2; GyroZ
//unsigned char who_am_i;
uint16_t tick = 0;                                  //syncs main loop with 1ms Systick
//uint16_t address = 0;
volatile uint16_t vadc = 10000;
volatile uint16_t low_bat = 0;
extern uint8_t idle_throttle;
uint8_t lowbat_flag = 0;
extern float K_p, K_i, K_d;                         //PID gain values for pitch & roll
extern float K_pY, K_iY, K_dY;                      //PID gain values for yaw

/* Virtual address defined by the user: 0xFFFF value is prohibited */
volatile uint16_t VirtAddVarTab[NumbOfVar] = {neutral_pw1, neutral_pw2, neutral_pw3, neutral_pw4, p_gain, i_gain, d_gain, p_gainy, i_gainy, d_gainy, lowbat, idlethrottle, gyroreverse};
                                                                                                  
static __IO uint32_t TimingDelay;                                                                  
                                                                                                  
                                                                                                  
/* Private function prototypes -----------------------------------------------*/                 
void RCC_Configuration(void);
void SysTick_Configuration(void);
void NVIC_Configuration(void);
void readeeprom(void);
void USART_Configuration(void);
void USART_RC_Config(void);
void Delay                  (__IO uint32_t nTime);
void readeeprom(void);
void I2C_Configuration(void);
void I2C2_Configuration(void);
void CalibrateRC(void);
void CalibrateGyro(void);
void GUI_com(void);




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

  /* Prepare for EEProm emulation ----------------------------------------------*/
  /* Unlock the Flash Program Erase controller */
  FLASH_Unlock();
  /* EEPROM Init */
  EE_Init();
  /*Lock the Flash Program Erase controller */
  FLASH_Lock();
  /*----------------------------------------------------------------------------*/

  readeeprom();                                                 //reads parameters from eeprom

  /* SysTick Configuration */
  SysTick_Configuration();

  /* UART configuration for gui interface */
  USART_Configuration();

  /* UART configuration for ACT DSL receiver interface */
  USART_RC_Config();

  /* I2C configuration for EMCs */
  I2C2_Configuration();

  /* Configuration of ADC1 for voltage measurement (batterie warning)*/
  ADC1_Configuration();
  
  /* TIM3_Configuration for gerneration of 2kHz Signal for buzzer */
  TIM3_Configuration();                                         
   
  /* I2C configuration for Gyro*/
  I2C_Configuration();

  if((RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET))                     //calibrate only if there was a powerdown reset
  {
    RCC_ClearFlag();                                                  //clears all reset flags
    Delay(3000);
  
    if((pulswidth.puls.pw4 < 8700) && (pulswidth.puls.pw3 < 8700))    //stick position: lower left corner
    { 
      QuadC_LEDOn(LED2);
      while(pulswidth.puls.pw3 < 9000);                               //while stick in lower left corner
      Delay(200);
      QuadC_LEDOff(LED2);
      CalibrateRC();
    }
  
    CalibrateGyro();                                                  //determines gyros zero angular velocity values
  }
  else
  {
    QuadC_LEDOn(LED2);                                                //turn on red LED if unknown reset occured
  }

  QuadC_LEDOn(LED1);                                                  //boot process finished

  while (1)
  {
    if(tick > 4)                                                      //once every 4ms
    {
      tick = 0;

      readmem(29,buf,6);                                              //read gyro data
      GyroX = (buf[0] << 8) | buf[1];
      GyroY = (buf[2] << 8) | buf[3];
      GyroZ = (buf[4] << 8) | buf[5];

      GyroX -= GyroXNeutral;                                          
      GyroY -= GyroYNeutral;
      GyroZ -= GyroZNeutral;

      if(gyro_reverse & 0x01)                                         //if coresoponding bit is set - invert gyro value
        GyroX = (-1) * GyroX;
      if(gyro_reverse & 0x02)
        GyroY = (-1) * GyroY;
      if(gyro_reverse & 0x04)
        GyroZ = (-1) * GyroZ;

      mixer();                                                        //angular volocity control and mixing of channels

      GUI_com();                                                      //handels communication with gui

      if(vadc < low_bat)                                              //checks batterie voltage
      {
        lowbat_flag = 1;                                              //there is no clearing this flag
      }
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
  
  /* TIM1, GPIOA, GPIOB, GPIOE and AFIO clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |  RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

  /* TIM1 clock enable */
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /* TIM2 clock enable */
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* ADC1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* I2C1 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE); 

  /* I2C2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE); 
  
  /* USART1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 

  /* USART2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* ADC2 clock enable */
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
  
  /* DMA2 clock enable */
  //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

  /* CAN1 Periph clock enable */
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
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






void CalibrateRC()
{
  int i;
  long int neutral1 = 0;
  long int neutral2 = 0;
  long int neutral3 = 0;
  long int neutral4 = 0;

  for(i = 0; i < 100; i++)
  {
    neutral1 += pulswidth.puls.pw1;
    neutral2 += pulswidth.puls.pw2;
    neutral3 += pulswidth.puls.pw3;
    neutral4 += pulswidth.puls.pw4;
    Delay(20);     
  }
  neutral.rotate.roll = neutral1 / 100;
  neutral.rotate.pitch = neutral2 / 100;
  neutral.rotate.yaw = neutral3 / 100;
  neutral.rotate.throttle = neutral4 / 100;

  FLASH_Unlock();
  EE_WriteVariable(neutral_pw1, neutral.rotate.roll);
  EE_WriteVariable(neutral_pw2, neutral.rotate.pitch);
  EE_WriteVariable(neutral_pw3, neutral.rotate.yaw);
  EE_WriteVariable(neutral_pw4, neutral.rotate.throttle);
  FLASH_Lock();
}


void CalibrateGyro()
{
  int i;
  int neutralX = 0;
  int neutralY = 0;
  int neutralZ = 0;

  for(i = 0; i < 100; i++)
  {
    readmem(29,buf,6);
    neutralX += (int16_t)(buf[0] << 8) | buf[1];
    neutralY += (int16_t)(buf[2] << 8) | buf[3];
    neutralZ += (int16_t)(buf[4] << 8) | buf[5];
    Delay(20);
  }

  GyroXNeutral = neutralX / 100;
  GyroYNeutral = neutralY / 100;
  GyroZNeutral = neutralZ / 100;
}


/* reads stored parameters from the emulated eeprom(flash) */
void readeeprom(void)
{
  uint16_t temp;
  if(!EE_ReadVariable(neutral_pw1, (uint16_t *)&neutral.rotate.roll))       //if variable exists
    ;                                                                       
  else                                                                      //else set to default
    neutral.rotate.roll = NEUTRAL_ROTATE_ROLL;                                    

  if(!EE_ReadVariable(neutral_pw2, (uint16_t *)&neutral.rotate.pitch))      //if variable exists
    ;
  else                                                                      //else set to default
    neutral.rotate.pitch = NEUTRAL_ROTATE_PITCH;

  if(!EE_ReadVariable(neutral_pw3, (uint16_t *)&neutral.rotate.yaw))        //if variable exists
    ;
  else                                                                      //else set to default
    neutral.rotate.yaw = NEUTRAL_ROTATE_YAW;

  if(!EE_ReadVariable(neutral_pw4, (uint16_t *)&neutral.rotate.throttle))   //if variable exists
    ;
  else                                                                      //else set to default
    neutral.rotate.throttle = NEUTRAL_ROTATE_THROTTLE;

  if(!EE_ReadVariable(p_gain, (uint16_t *)&temp))                           //if variable exists
    K_p = (float)temp / 255;
  else                                                                      //else set to default
    K_p = P_GAIN;
  if(!EE_ReadVariable(i_gain, (uint16_t *)&temp))                           //if variable exists
    K_i = (float)temp / 255;
  else                                                                      //else set to default
    K_i = I_GAIN;
  if(!EE_ReadVariable(d_gain, (uint16_t *)&temp))                           //if variable exists
    K_d = (float)temp / 255;
  else                                                                      //else set to default
    K_d = D_GAIN;
  if(!EE_ReadVariable(p_gainy, (uint16_t *)&temp))                          //if variable exists
    K_pY = (float)temp / 255;
  else                                                                      //else set to default
    K_pY = P_GAINY;
  if(!EE_ReadVariable(i_gainy, (uint16_t *)&temp))                          //if variable exists
    K_iY = (float)temp / 255;
  else                                                                      //else set to default
    K_iY = I_GAINY;
  if(!EE_ReadVariable(d_gainy, (uint16_t *)&temp))                          //if variable exists
    K_dY = (float)temp / 255;
  else                                                                      //else set to default
    K_dY = D_GAINY;
  if(!EE_ReadVariable(lowbat, (uint16_t *)&temp))                           //if variable exists
    low_bat = (uint16_t)temp * 11.09;
  else                                                                      //else set to default
    low_bat = LOWBAT;
  if(!EE_ReadVariable(idlethrottle, (uint16_t *)&idle_throttle))            //if variable exists
    ;
  else                                                                      //else set to default
    idle_throttle = IDLE_THROTTLE;
  if(!EE_ReadVariable(gyroreverse, (uint16_t *)&gyro_reverse))              //if variable exists
    ;
  else                                                                      //else set to default
    gyro_reverse = 0x00;

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

  /* Configure and enable I2C1 event interrupt -------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

    /* Configure and enable I2C1 event interrupt -------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM3 gloabal interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
//
//  /* Enable the TIM1 Commutation Interrupt */
//  /* is triggered by TIM2 interrupt*/
//  NVIC_InitStructure.NVIC_IRQChannel = TIM1_TRG_COM_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure); 
//
//  /* Enable TIM1 CC interrupt */
//  /* triggeres ADC conversion */
//  NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//
//  /* Configure and enable ADC interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

  /* Enable CAN1 RX interrupt */ 
//  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

  /* Enable the RTC Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

//  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


  /* SysTick Priority */
  NVIC_SetPriority(SysTick_IRQn, 2);
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
