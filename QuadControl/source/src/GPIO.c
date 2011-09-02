// Header: 		IO Port Configuation
// File Name: GPIO.c
// Author: 		Konstantin Reichmeyer
// Date:   		13.11.10


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "GPIO.h"


void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

//  /* GPIOA Configuration: Channel 1, 2 and 3 as alternate function push-pull */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//  /* GPIOB Configuration: Channel 1N, 2N and 3N as alternate function push-pull */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//  /* GPIOA Configuration: Testpad TP1 .. TP4 */
//
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //disable JTAG on Port Pins: PA15, PB3, PB4
//
//  GPIO_InitStructure.GPIO_Pin = TP1_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(TP1_GPIO_PORT, &GPIO_InitStructure);
//
//  GPIO_InitStructure.GPIO_Pin = TP2_PIN;
//  GPIO_Init(TP2_GPIO_PORT, &GPIO_InitStructure);
//
//  GPIO_InitStructure.GPIO_Pin = TP3_PIN;
//  GPIO_Init(TP3_GPIO_PORT, &GPIO_InitStructure);
//
//  GPIO_InitStructure.GPIO_Pin = TP4_PIN;
//  GPIO_Init(TP4_GPIO_PORT, &GPIO_InitStructure);
//
//  GPIO_InitStructure.GPIO_Pin = TP5_PIN;
//  GPIO_Init(TP5_GPIO_PORT, &GPIO_InitStructure);

  /* GPIOB Configuration: LED1 & LED2 */
  GPIO_InitStructure.GPIO_Pin = LED1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = LED2_PIN;
  GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);

  /* GPIOB Configuration: SUMMER*/
  GPIO_InitStructure.GPIO_Pin = SUMMER_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SUMMER_GPIO_PORT, &GPIO_InitStructure);

  //GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);


//  /* GPIOA Configuration: Phases 1: PA7*/
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//  /* GPIOB Configuration: Phases 2, 3: PB0, PB1*/
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//  /* GPIOA Configuration: Sensor 1 & Sensor 2 */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//  /* Configure CAN pin: RX */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//  
//  /* Configure CAN pin: TX */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//  GPIO_PinRemapConfig(GPIO_Remap1_CAN1 , ENABLE);
//
//  /* Configuration Voltage pin PA3 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure USART1 Tx (PA9) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART1 Rx (PA10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  /* Configure I2C1 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure I2C2 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//  /* Configuration Current pin PA6 */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);


/* Enable I2C1 reset state */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
/* Release I2C1 from reset state */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

  /* Enable I2C2 reset state */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);
/* Release I2C2 from reset state */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);

}


const uint16_t GPIO_PIN[3] = {LED1_PIN, LED2_PIN, SUMMER_PIN};
//const uint16_t GPIO_PIN_TP[5] = {TP1_PIN, TP2_PIN, TP3_PIN, TP4_PIN, TP5_PIN};
GPIO_TypeDef* GPIO_PORT[3] = {LED1_GPIO_PORT, LED2_GPIO_PORT, SUMMER_GPIO_PORT};
//GPIO_TypeDef* GPIO_PORT_TP[5] = {TP1_GPIO_PORT, TP2_GPIO_PORT, TP3_GPIO_PORT, TP4_GPIO_PORT, TP5_GPIO_PORT};

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2 
  * @retval None
  */
void QuadC_LEDToggle(Led_TypeDef Led)
{
  GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void QuadC_LEDOn(Led_TypeDef Led)
{
  GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];     
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void QuadC_LEDOff(Led_TypeDef Led)
{
  GPIO_PORT[Led]->BRR = GPIO_PIN[Led];  
}


/**
  * @brief  Turns selected TP Off.
  * @param  TP: Specifies the TP to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg TP1
  *     @arg TP2
  *     @arg TP3
  *     @arg TP4

  * @retval None
  */
//void DISCC_TPOff(TP_TypeDef TP)
//{
//  GPIO_PORT_TP[TP]->BRR = GPIO_PIN_TP[TP];  
//}


/**
  * @brief  Toggles the selected TP.
  * @param  TP: Specifies the TP to be toggled. 
  *   This parameter can be one of following parameters:
  *     @arg TP1
  *     @arg TP2
  *     @arg TP3
  *     @arg TP4

  * @retval None
  */
//void DISCC_TPToggle(TP_TypeDef TP)
//{
//  GPIO_PORT_TP[TP]->ODR ^= GPIO_PIN_TP[TP];
//}

/**
  * @brief  Turns selected TP On.
  * @param  TP: Specifies the TP to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg TP1
  *     @arg TP2
  *     @arg TP3
  *     @arg TP4

  * @retval None
  */
//void DISCC_TPOn(TP_TypeDef TP)
//{
//  GPIO_PORT_TP[TP]->BSRR = GPIO_PIN_TP[TP];     
//}
