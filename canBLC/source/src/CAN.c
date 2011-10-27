// Header:
// File Name: CAN.c
// Author: Konstantin Reichmeyer
// Date: 30.04.10


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "CAN.h"
#include "main.h"
#include "bemf.h"
#include "beep.h"
#include "eeprom.h"

extern uint8_t  canlog;
extern CanTxMsg TxMessage;
extern CanRxMsg RxConfigMessage;
extern uint16_t minpwm;
extern uint16_t maxpwm;
extern uint16_t address;
extern uint8_t  newConfig;
extern uint8_t  angle;
uint8_t         startmode = 0;

/* Virtual address defined by the user: 0xFFFF value is prohibited */
volatile uint16_t VirtAddVarTab[NumbOfVar] = {flashminpwm, flashmaxpwm, flashaddress, flashangle};


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

  CAN_wrFilter(0x001, CAN_ID_STD);
  CAN_wrFilter(0x100, CAN_ID_STD);
  
  /* transmit */
  TxMessage.StdId = 0x010 | (address+1) ;
  TxMessage.ExtId = 0x01;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
  TxMessage.DLC = 6;
}


/* Configures motor while motor is turned off */
uint8_t ConfigMessageOff(void)
{
  uint8_t i;

  if(newConfig)                                                            //new configuration received?
  {
    newConfig = 0;                                            //reset flag

    beep(1000,200);
    if((RxConfigMessage.Data[0] == 0xF0) && (RxConfigMessage.Data[1] == 0x07) && (RxConfigMessage.Data[2] == 0xFA) && (RxConfigMessage.Data[3] == 0x17) && (RxConfigMessage.Data[4] == 0xED))
    { //if(f007f4173d) foolfailed
      //Reset everything to default
      address = ADDRESS;
      TxMessage.StdId = 0x010 | (address+1);                               //set log transmission address
      minpwm  = MINPWM;
      maxpwm  = MAXPWM;
      angle   = ANGLE;

      FLASH_Unlock();

      EE_WriteVariable(flashaddress,  ADDRESS);               //write address to flash
      EE_WriteVariable(flashminpwm,   MINPWM);                //write minpwm to flash
      EE_WriteVariable(flashmaxpwm,   MAXPWM);                //write maxpwm to flash
      EE_WriteVariable(flashangle,    ANGLE);                 //write angle to flash

      FLASH_Lock();

      beep(700,2000);
    }
    else
    {
      if((RxConfigMessage.Data[0] > 0) && (RxConfigMessage.Data[0] < 9) && (RxConfigMessage.Data[4] & 0x40))     //valid address?
      {
        if(address != (RxConfigMessage.Data[0] - 1))
        {
          address = RxConfigMessage.Data[0] - 1;                               //set data address
          TxMessage.StdId = 0x010 | (address+1);                               //set log transmission address
          Delay(200);
          for(i = 0; i< address +1; i++)
          {
            beep(700,200);
            Delay(200);
          }
        }
      }
      
      if(RxConfigMessage.Data[1] > 0 && RxConfigMessage.Data[1] < 16)
        angle = RxConfigMessage.Data[1];                                     //set commutation timing
    
      if(RxConfigMessage.Data[2] != 0)
        minpwm = RxConfigMessage.Data[2];                                    //set pwmmin
    
      if(RxConfigMessage.Data[3] != 0)
      {
        maxpwm = RxConfigMessage.Data[3] * 8;                                //set pwmmax
        if(maxpwm > 1900)
          maxpwm = 1900;
      }
      startmode = RxConfigMessage.Data[4] & 0x02;               //startmode selection bit set? push or ramp start?
      canlog = RxConfigMessage.Data[4] & 0x01;                  //turn on/off log transmission
    
      if(RxConfigMessage.Data[4] & 0x80)                        //flash write bit set?
      {
        FLASH_Unlock();
  
        EE_WriteVariable(flashaddress,  address);               //write address to flash
        EE_WriteVariable(flashminpwm,   minpwm);                //write minpwm to flash
        EE_WriteVariable(flashmaxpwm,   maxpwm);                //write maxpwm to flash
        EE_WriteVariable(flashangle,    angle);                 //write angle to flash

        FLASH_Lock();

        Delay(200);
        beep(1000,100);
        beep(700,200);
      }

      if((RxConfigMessage.Data[4] & 0x1C) != 0)
      {
        idle(((RxConfigMessage.Data[4] & 0x1C)>>2)-1);
      }
    }


    return 1;
  }
  else
    return 0;
}

/* Configure motor controller while motor is running
/  timing can be changed and log can be enabled/disabled*/
uint8_t ConfigMessageOn(void)
{
  if(newConfig)                                                             //new configuration received?
  {
    if((RxConfigMessage.Data[1] > 0) && (RxConfigMessage.Data[1] < 16) && (RxConfigMessage.Data[4] & 0x20))
      angle = RxConfigMessage.Data[1];                                      //set commutation timing

    canlog = RxConfigMessage.Data[4] & 0x01;                                //turn on/off log transmission

    newConfig = 0;                                                          //reset flag
    return 1;
  }
  else
    return 0;
}
