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

#define TxBufferSize   (countof(TxBuffer) - 1)
char TxBuffer[] = "Buffer has never been written!                                                                                                                                                                                    ";


u8 rcdsl_battery;
u8 rcdsl_allocation;
volatile u8 rcdsl_RSSI = 0;

union pulsw pulswidth;
union channels neutral;
struct props gas = {0,0,0,0};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t buf[10];
int16_t GyroX, GyroY, GyroZ, Temp;
int16_t GyroXNeutral, GyroYNeutral, GyroZNeutral;
unsigned char who_am_i;
uint16_t tick = 0;
uint8_t NbrOfDataToTransfer = TxBufferSize;
uint16_t address = 0;
volatile uint16_t vadc = 10000;
uint8_t lowbat_flag = 0;


/* Virtual address defined by the user: 0xFFFF value is prohibited */
volatile uint16_t VirtAddVarTab[NumbOfVar] = {neutral_pw1, neutral_pw2, neutral_pw3, neutral_pw4};

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

//extern union channels set; //temporary
//extern struct pid level;
//extern struct pdyaw yaw;

extern float K_p;
extern float K_i;
extern float K_d;

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

  readeeprom();   //read RC channels neutral calibration data

  /* SysTick Configuration */
  SysTick_Configuration();

  /* UART configuration for debugging */
  USART_Configuration();

  USART_RC_Config();

  /* I2C configuration for EMCs*/
  I2C2_Configuration();

  /* Configuration of ADC1 for voltage measurement*/
  ADC1_Configuration();
  
  /* TIM3_Configuration */
  TIM3_Configuration(); 
   
  /* I2C configuration for Gyro*/
  I2C_Configuration();

  Delay(3000);

//  if((pulswidth.puls.pw4 < 8700) && (pulswidth.puls.pw3 < 8700))    //stick position: lower left corner
//  { 
//    QuadC_LEDOn(LED2);
//    while(pulswidth.puls.pw3 < 9000);
//    Delay(100);
//    QuadC_LEDOff(LED2);
//    CalibrateRC();
//  }

  CalibrateGyro();

  QuadC_LEDOn(LED1);     //boot process finished

  while (1)
  {
    if(tick > 4)
    {
      tick = 0;

      readmem(29,buf,6);
      GyroX = (buf[0] << 8) | buf[1];
      GyroY = (buf[2] << 8) | buf[3];
      GyroZ = (buf[4] << 8) | buf[5];

      GyroX -= GyroXNeutral;
      GyroY -= GyroYNeutral;
      GyroZ -= GyroZNeutral;

      mixer();

      //sprintf(TxBuffer,"PW1:%5d; PW2:%5d; PW3:%5d; PW4:%5d; \r\n",neutral.rotate.roll, neutral.rotate.pitch, neutral.rotate.yaw, neutral.rotate.throttle);
      //sprintf(TxBuffer,"GyroX:%5d; GyroY:%5d; GyroZ:%5d; GyroXNeutral:%5d; GyroYNeutral:%5d; GyroZNeutral:%5d \r\n", GyroX,GyroY,GyroZ, GyroXNeutral, GyroYNeutral, GyroZNeutral);
      //sprintf(TxBuffer,"Temperatur:%5d; GyroX:%5d; GyroY:%5d; GyroZ:%5d; pulswidth: pw1:%5d pw2:%5d pw3:%5d pw4:%5d pw5:%5d pw6:%5d pw7:%5d pw8:%5d \r\n",Temp, GyroX,GyroY,GyroZ, pulswidth.pw[0],pulswidth.pw[1],pulswidth.pw[2],pulswidth.pw[3],pulswidth.pw[4],pulswidth.pw[5],pulswidth.pw[6],pulswidth.pw[7]);
      //sprintf(TxBuffer,"Gas1: %5d; Yaw: %5d; Gyro: %5d; levelsign[x]: %5d; levelerror[x] %5d; setlevel[x] %5d \r\n",gas.front, set.rotate.yaw, GyroZ,yaw.yawsign,yaw.yawerror, set.level[2]);
      //sprintf(TxBuffer,"Gas.front: %5d; Gas.rear: %5d; Gas.left: %5d; Gas.right: %5d;\r\n",gas.front, gas.rear, gas.left, gas.right);
      //sprintf(TxBuffer,"K_p:%5f; K_i:%5f; K_d:%5f;\r\n",K_p, K_i, K_d);
      //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

      GUI_com();

      if(vadc < LOWBAT)
      {
        lowbat_flag = 1;
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

/* reads stored parameters from the emulated eeprom(flash) */
//void readeeprom(void)
//{
//  if(!EE_ReadVariable(flashminpwm, &minpwm))        //if variable exists
//    ;
//  else                                              //else set to default
//    minpwm = MINPWM;                                    
//
//  if(!EE_ReadVariable(flashmaxpwm, &maxpwm))        //if variable exists
//    ;
//  else                                              //else set to default
//    maxpwm = MAXPWM;
//  if(!EE_ReadVariable(flashaddress, &address))      //if variable exists
//    ;
//  else                                              //else set to default
//    address = ADDRESS;
//  if(!EE_ReadVariable(flashangle, &angle))          //if variable exists
//    ;
//  else                                              //else set to default
//    angle = ANGLE;
//  TxMessage.StdId = 0x010 | (address+1);
//}




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
  if(!EE_ReadVariable(neutral_pw1, (uint16_t *)&neutral.rotate.roll))        //if variable exists
    ;
  else                                                           //else set to default
    neutral.rotate.roll = NEUTRAL_ROTATE_ROLL;                                    

  if(!EE_ReadVariable(neutral_pw2, (uint16_t *)&neutral.rotate.pitch))       //if variable exists
    ;
  else                                                           //else set to default
    neutral.rotate.pitch = NEUTRAL_ROTATE_PITCH;

  if(!EE_ReadVariable(neutral_pw3, (uint16_t *)&neutral.rotate.yaw))        //if variable exists
    ;
  else                                                           //else set to default
    neutral.rotate.yaw = NEUTRAL_ROTATE_YAW;

  if(!EE_ReadVariable(neutral_pw4, (uint16_t *)&neutral.rotate.throttle))     //if variable exists
    ;
  else                                                           //else set to default
    neutral.rotate.throttle = NEUTRAL_ROTATE_THROTTLE;
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
