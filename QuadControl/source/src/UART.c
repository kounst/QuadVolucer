// Header:
// File Name: 
// Author:
// Date:


#include "stm32f10x.h"
#include "UART.h"
#include "stdio.h"
#include "string.h"
#include "main.h"
#include "GPIO.h"
#include "eeprom.h"

char msg[40];

extern char TxBuffer[];
extern union channels set;
extern union pulsw pulswidth;
extern float K_p, K_i, K_d;
extern float K_pY, K_iY, K_dY;
extern uint8_t idle_throttle;
extern uint16_t gyro_reverse;

char gui_SENSOR[13];
char gui_PARA[33];
char gui_packet[39];

char *TxBuf;

volatile uint8_t gui_RESET = 0;
volatile uint8_t gui_READpara = 0;
volatile uint8_t gui_READsens = 0;
volatile uint8_t gui_SEND = 0;
volatile uint8_t gui_rx_finished = 0;
uint8_t NoofByte = 0;

extern int16_t GyroX,GyroY,GyroZ;
extern uint16_t vadc;
extern uint16_t low_bat;

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART_Configuration()
{
  /* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 38400 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - Odd parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 38400;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure the USART1 */
  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);
}


/*******************************************************************************
* Function Name  : USART_RC_Configuration
* Description    : Configures the USART2 to receive channels from receiver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART_RC_Config()
{
  USART_InitTypeDef USART_InitStructure;
  #ifdef ACT
  USART_InitStructure.USART_BaudRate = 38400;
  #endif
  #ifdef MPX
  USART_InitStructure.USART_BaudRate = 115200;
  #endif

  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx;

  /* Configure the USART2 */
  USART_Init(USART2, &USART_InitStructure);

  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  /* Enable USART2 */
  USART_Cmd(USART2, ENABLE);
}

/*******************************************************************************
* Function Name  : GUI_com()
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GUI_com()
{
  uint8_t i = 0;

  if(gui_RESET)
  {
    //reset device                                        //not implemented as the gui doesn't seem to use this command anyway
    gui_RESET = 0; //reset flag
    //QuadC_LEDToggle(LED2);

  }

  if(gui_rx_finished)
  {
    //receive data from pc
    K_p = (float)gui_PARA[6] / 255;
    K_i = (float)gui_PARA[7] / 255;
    K_d = (float)gui_PARA[28] / 255;
    K_pY = (float)gui_PARA[11] / 255;
    K_iY = (float)gui_PARA[12] / 255;
    idle_throttle = gui_PARA[20];
    low_bat = (uint16_t)gui_PARA[21] * 11.09;
    if(gui_PARA[1])
      gyro_reverse |= 0x01;
    else
      gyro_reverse &= ~0x01;
    if(gui_PARA[2])
      gyro_reverse |= 0x02;
    else
      gyro_reverse &= ~0x02;
    if(gui_PARA[3])
      gyro_reverse |= 0x04;
    else
      gyro_reverse &= ~0x04;
    
    //write new data to emulated eeprom
    FLASH_Unlock();
    EE_WriteVariable(p_gain, (uint16_t)gui_PARA[6]);
    EE_WriteVariable(i_gain, (uint16_t)gui_PARA[7]);
    EE_WriteVariable(d_gain, (uint16_t)gui_PARA[28]);
    EE_WriteVariable(p_gainy, (uint16_t)gui_PARA[11]);
    EE_WriteVariable(i_gainy, (uint16_t)gui_PARA[12]);
    EE_WriteVariable(lowbat, (uint16_t)gui_PARA[21]);
    EE_WriteVariable(idlethrottle, (uint16_t)gui_PARA[20]);
    EE_WriteVariable(gyroreverse, (uint16_t)gyro_reverse);
    FLASH_Lock();

    gui_rx_finished = 0;                                                  //reset flag
  }

  if(gui_READpara)
  {
    

    gui_READpara = 0; //reset flag
    //send parameter to pc

    gui_PARA[0]  = 1;                                 //motors enable                 - not implemented
    gui_PARA[1]  = (uint8_t)(gyro_reverse & 0x01);   //gyro_roll_dir                 
    gui_PARA[2]  = (uint8_t)(gyro_reverse & 0x02);   //gyro_pitch_dir                
    gui_PARA[3]  = (uint8_t)(gyro_reverse & 0x04);   //gyro_yaw_dir                  
    gui_PARA[4]  = 0;                                 //acc_x_dir                     - not implemented
    gui_PARA[5]  = 0;                                 //acc_y_dir                     - not implemented
    gui_PARA[6]  = (uint8_t)(K_p * 255);              //P_Gain
    gui_PARA[7]  = (uint8_t)(K_i * 255);              //I_Gain
    gui_PARA[8]  = 0;                                 //P_Gain_angle                  - not implemented
    gui_PARA[9]  = 0;                                 //I_Gain_angle                  - not implemented
    gui_PARA[10] = 0;                                 //D_Gain_angle                  - not implemented
    gui_PARA[11] = (uint8_t)(K_pY * 255);             //P_Gain_yaw
    gui_PARA[12] = (uint8_t)(K_iY * 255);             //I_Gain_yaw
    gui_PARA[13] = 0;                                 //ACC influence sensorfusion    - not implemented
    gui_PARA[14] = 0;                                 //acc_x scale                   - not implemented
    gui_PARA[15] = 0;                                 //acc_y scale                   - not implemented
    gui_PARA[16] = 0;                                 //expo                          - not implemented
    gui_PARA[17] = 0;                                 //expo                          - not implemented
    gui_PARA[18] = 0;                                 //expo                          - not implemented
    gui_PARA[19] = 0;                                 //expo                          - not implemented
    gui_PARA[20] = idle_throttle;                     //minimum throttle  
    gui_PARA[21] = (uint8_t)((low_bat + 5) / 11.09);        //voltage warning    
    gui_PARA[22] = 0;                                 //acc_x offset                  - not implemented
    gui_PARA[23] = 0;                                 //acc_y offset                  - not implemented
    gui_PARA[24] = 4;                                 //throttle channel              - hardcoded
    gui_PARA[25] = 2;                                 //pitch channel                 - hardcoded
    gui_PARA[26] = 1;                                 //roll channel                  - hardcoded
    gui_PARA[27] = 3;                                 //yaw channel                   - hardcoded
    gui_PARA[28] = (uint8_t)(K_d * 255);              //D_Gain
    gui_PARA[29] = 0;                                 //D2_Gain                       - not implemented
    gui_PARA[30] = 0;                                 //n/a
    gui_PARA[31] = 0;                                 //n/a
    gui_PARA[32] = 0;                                 //n/a

    strcpy(gui_packet, "s#p!\r\n");                   //tells pc that there are parameter about to be sent
    for(i = 0; i < 33; i++)                           //append parameters to gui_packet
    {
      gui_packet[i+6] = gui_PARA[i];
    }

    TxBuf = gui_packet;
    NoofByte = 39;
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);     //trigger UART transmission


    
  }

  if(gui_READsens)
  {
    //send sensor data to pc
    gui_READsens = 0;                                               //reset flag

    //scale sensor values for transmission
    gui_SENSOR[0]  = (uint8_t)((GyroY / 128) + 128);
    gui_SENSOR[1]  = 0;                                             //angle
    gui_SENSOR[2]  = 0;                                             //acc
    gui_SENSOR[3]  = (uint8_t)((GyroX / 128) + 128);
    gui_SENSOR[4]  = 0;                                             //angle
    gui_SENSOR[5]  = 0;                                             //acc
    gui_SENSOR[6]  = (uint8_t)((GyroZ / 128) + 128);
    gui_SENSOR[7]  = (uint8_t)((set.rotate.throttle + 500) / 20);
    gui_SENSOR[8]  = (uint8_t)((set.rotate.roll + 3825) / 30);
    gui_SENSOR[9]  = (uint8_t)((set.rotate.pitch + 3825) / 30);
    gui_SENSOR[10] = (uint8_t)((set.rotate.yaw + 3825) / 30);
    gui_SENSOR[11] = (uint8_t)((pulswidth.puls.pw6 - 7350) / 30);
    gui_SENSOR[12] = (uint8_t)((vadc / 5.44) - 256);


    strcpy(gui_packet, "ss!\r\n");                    //tells pc that there a sensor data about to be sent
    for(i = 0; i < 13; i++)
    {
      gui_packet[i+5] = gui_SENSOR[i];
    }
    TxBuf = gui_packet;
    NoofByte = 18;
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);     //trigger UART transmission

  }
}



/*******************************************************************************
* Function Name  : GUI_receive()
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GUI_receive(char c)
{
  static uint8_t rx_count = 0;
  uint8_t i;

  msg[rx_count] = c;
  rx_count++;
  if(gui_SEND)
  {
    if(rx_count > 33)
    {
      rx_count = 0;
      gui_rx_finished = 1;
      gui_SEND = 0;
      for(i = 0; i< 33; i++)
      {
        gui_PARA[i] = msg[i+1];
      }
    }
  }
  else
  {
    if(c == '!' || rx_count >= 35)
    {
      rx_count = 0;
      if(strstr(msg,"reset!")) 
      {
        gui_RESET = 1;
      }  
      if(strstr(msg,"cr!"))
      {
        //pc wants to read data
        gui_READpara = 1;
      }
      if(strstr(msg,"cs!"))
      {
        //pc wants to read sensor
        gui_READsens = 1;
      }
      if(strstr(msg,"ci!"))
      {
        //pc wants to send data
        gui_SEND = 1;
        rx_count = 0;
      }
    }
  }
}
