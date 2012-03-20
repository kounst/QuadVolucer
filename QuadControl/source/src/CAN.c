// Header:
// File Name: CAN.c
// Author: Konstantin Reichmeyer
// Date: 16.10.11


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "CAN.h"
#include "main.h"



CanTxMsg TxMessage;
CanRxMsg RxConfigMessage;



void CAN_Configuration(void)
{
  CAN_InitTypeDef        CAN_InitStructure;

  /* CAN register init */
  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

  //250kbaud
  //CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
  //CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;
  //CAN_InitStructure.CAN_Prescaler = 8;

  //500kbaud
  CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;
  CAN_InitStructure.CAN_Prescaler = 4;

  CAN_Init(CAN1, &CAN_InitStructure);

  CAN_wrFilter(0x101, CAN_ID_STD);
  CAN_wrFilter(0x102, CAN_ID_STD);
  CAN_wrFilter(0x103, CAN_ID_STD);
  CAN_wrFilter(0x104, CAN_ID_STD);
  
  /* transmit */
  TxMessage.StdId = 0x001;
  TxMessage.ExtId = 0x01;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
  TxMessage.DLC = 8;

  /* IT Configuration for CAN1 */  
  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}


