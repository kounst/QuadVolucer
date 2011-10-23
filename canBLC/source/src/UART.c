// Header: UART3 Configuration for comunication with PC
// File Name: UART.c
// Author: Konstantin Reichmeyer
// Date:


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "UART.h"

void UART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = 9600;  //115200Baud/s
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_Odd;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure the USART3 */
  USART_Init(USART3, &USART_InitStructure);
  

  /* Enable USART3 */
  USART_Cmd(USART3, ENABLE);
}
