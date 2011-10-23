// Header: This file provides templates for all exceptions handler and peripherals
//         interrupt service routine.
// File Name: stm32f10x_it.c
// Author: Konstantin Reichmeyer
// Date:





/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "main.h"
#include "GPIO.h"
#include "I2C.h"
#include "rcdsl.h"
#include "UART.h"

  /** I2C addresses */
#define LEFT  0x58
#define FRONT 0x52
#define RIGHT 0x54
#define REAR  0x56


short slave = 0;

/* Private variables ---------------------------------------------------------*/
uint8_t addr = 27;
extern char TxBuffer[];
u8 TxCounter = 0;
extern uint8_t tick;

extern unsigned char buf[10];
uint8_t n = 8;

extern struct props gas;
extern uint8_t NbrOfDataToTransfer;
extern uint8_t NoofByte;
extern uint16_t vadc;
extern uint8_t lowbat_flag;
extern uint8_t *TxBuf;
extern CanTxMsg TxMessage;
extern CanRxMsg RxConfigMessage;


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
  slave++;
  if(slave > 4)                //number of motors
	{
    slave = 0;

    #ifndef CAN_blc
    I2C2_Configuration();
    #endif
	}
  else
  {
    #ifdef CAN_blc
    TxMessage.Data[0] = gas.front;
    TxMessage.Data[1] = gas.right;
    TxMessage.Data[2] = gas.rear;
    TxMessage.Data[3] = gas.left;
    CAN_Transmit(CAN1, &TxMessage);

    #else
    I2C_ClearFlag(I2C2, I2C_FLAG_AF | I2C_FLAG_ARLO | I2C_FLAG_BERR);
    I2C_ClearITPendingBit(I2C2, I2C_IT_ARLO | I2C_IT_BERR | I2C_IT_TIMEOUT | I2C_IT_AF | I2C_IT_ADD10 | I2C_IT_SB | I2C_IT_ADDR);
    I2C_ITConfig(I2C2, I2C_IT_EVT, ENABLE);
    I2C_GenerateSTART(I2C2, ENABLE);    //triggers I2C transmission to EMCs
    #endif
  }

  tick++;

  if(vadc > ADC_GetConversionValue(ADC1))
    vadc--;
  else
    vadc++;

  TimingDelay_Decrement();  //Decrement ms counter of Delay() function
}



/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : slave, gas
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
  switch (I2C_GetLastEvent(I2C2))
  {
    /* Test on I2C1 EV5 and clear it */
    case I2C_EVENT_MASTER_MODE_SELECT:
        /* Send I2C2 slave Address for write */
        switch (slave)
        {
          case 1:
            I2C_Send7bitAddress(I2C2, 0x52, I2C_Direction_Transmitter);
            break;
          case 2:
            I2C_Send7bitAddress(I2C2, 0x54, I2C_Direction_Transmitter);
            
            break;
          case 3:
            I2C_Send7bitAddress(I2C2, 0x56, I2C_Direction_Transmitter);
            break;
          case 4:
            I2C_Send7bitAddress(I2C2, 0x58, I2C_Direction_Transmitter);
            break;
          default:
            break;
        }
      break;

    /* Test on I2C1 EV6 and first EV8 and clear them */
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: 
      /* Send the first data */
        switch (slave)
        {
          case 1:
            I2C_SendData(I2C2, gas.front );  /* EV8 just after EV6 */
            break;
          case 2:
            I2C_SendData(I2C2, gas.right );  /* EV8 just after EV6 */
            break;
          case 3:
            I2C_SendData(I2C2, gas.rear );  /* EV8 just after EV6 */
            break;
          case 4:
            I2C_SendData(I2C2, gas.left );  /* EV8 just after EV6 */
            break;
          default:
            break;
        }
      break;

    /* Test on I2C1 EV8 and clear it */
    case I2C_EVENT_MASTER_BYTE_TRANSMITTED:          
      { 
          I2C_GenerateSTOP(I2C2, ENABLE);
          I2C_ITConfig(I2C2, I2C_IT_EVT, DISABLE);
      }
      break;
      
    default:
      break;
  }
}



/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : slave, gas
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
  static uint8_t transmitting = 0;
  //extern uint8_t n;
  static  unsigned char *buffer;
  switch (I2C_GetLastEvent(I2C1))
  {
    /* Test on I2C1 EV5 and clear it */
    case I2C_EVENT_MASTER_MODE_SELECT:
        if(!transmitting)
        {
          /* Send I2C1 slave Address for write */
  	      I2C_Send7bitAddress (I2C1, ITG3200_SLAVE_ADDR, I2C_Direction_Transmitter);
          transmitting++;

          buffer = buf;
        }
        else
        {
          /* Send EEPROM address for read */
	        I2C_Send7bitAddress (I2C1, ITG3200_SLAVE_ADDR, I2C_Direction_Receiver);
          transmitting = 0;
        }
      break;

    /* Test on I2C1 EV6 and first EV8 and clear them */
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: 
      /* Send the EEPROM's internal address to write to */
	    I2C_SendData (I2C1, addr);
      break;

    /* Test on I2C1 EV8 and clear it */
    case I2C_EVENT_MASTER_BYTE_TRANSMITTED:          
      { 
        /* Send STRAT condition a second time */  
        I2C_GenerateSTART (I2C1, ENABLE);
      }
      break;

    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:

        I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
          /* While there is data to be read */
      	while (n) {
      		if(n == 1) {
      			I2C_AcknowledgeConfig (I2C1, DISABLE);		/* Disable Acknowledgement */
      			I2C_GenerateSTOP (I2C1, ENABLE);			/* Send STOP Condition */
      		}
      		/* Test on EV7 and clear it */
      		if (I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
      			if(n == 2)
      				I2C_AcknowledgeConfig (I2C1, DISABLE);		/* Disable Acknowledgement */
      			
      			/* Read a byte from the EEPROM */
      			*buffer = I2C_ReceiveData (I2C1);
      			/* Point to the next location where the byte read will be saved */
      			buffer ++; 
      			/* Decrement the read bytes counter */
      			n --;    
      		}   
      	}
      
      	/* Enable Acknowledgement to be ready for another reception */
      	I2C_AcknowledgeConfig (I2C1, ENABLE);
//      if(n == 1)
//      {
//        /* While there is data to be read */
//  			I2C_AcknowledgeConfig (I2C1, DISABLE);		/* Disable Acknowledgement */
//  			I2C_GenerateSTOP (I2C1, ENABLE);			/* Send STOP Condition */
//      }
//      	break;
//
//    case I2C_EVENT_MASTER_BYTE_RECEIVED:
//          {
//            if(n == 1)
//            {
//              /* While there is data to be read */
//        			I2C_AcknowledgeConfig (I2C1, DISABLE);		/* Disable Acknowledgement */
//        			I2C_GenerateSTOP (I2C1, ENABLE);			/* Send STOP Condition */
//            }
//      			if(n == 2)
//      				I2C_AcknowledgeConfig (I2C1, DISABLE);		/* Disable Acknowledgement */
//      			
//      			/* Read a byte from the EEPROM */
//      			buf[0] = I2C_ReceiveData (I2C1);
//      			/* Point to the next location where the byte read will be saved */
//      			//buffer ++; 
//      			/* Decrement the read bytes counter */
//      			n --;    
//      		   
//      	}
      
//      	/* Enable Acknowledgement to be ready for another reception */
//      	I2C_AcknowledgeConfig (I2C1, ENABLE);

        break;
      
    default:
      break;
  }
}

void I2C1_ER_IRQHandler(void)
{
  n++;
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
  if(ADC_GetITStatus(ADC1, ADC_IT_JEOC) != RESET)  
  {
    /* Clear ADC1 JEOC pending interrupt bit */
    ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
  
  }
  if(ADC_GetITStatus(ADC2, ADC_IT_JEOC) != RESET)
  {
    /* Clear ADC2 JEOC pending interrupt bit */
    ADC_ClearITPendingBit(ADC2, ADC_IT_JEOC);

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

}




/**
  * @brief  This function handles TIM2 CC interrupts
  *   requests.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{

}


/**
  * @brief  This function handles TIM3 CC interrupts
  *   requests.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
  int capture;

  TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

  /* This cannot be done in hardware because of an error on the MC -> see errata */

  if(lowbat_flag)
  {
    QuadC_LEDToggle(SUMMER);
  }

  capture = TIM_GetCapture1(TIM3);
  TIM_SetCompare1(TIM3, capture + 1500);
  
}



/**
  * @brief  This function handles CAN1 RX0 Handler.
  * @param  None
  * @retval None
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  CAN_Receive(CAN1, CAN_FIFO0, &RxConfigMessage);
}




/**
  * @brief  This function handles RTC global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
}



void RTCAlarm_IRQHandler(void)
{
}



/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
  static uint8_t count = 0;

  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {   
    /* Write one byte to the transmit data register */
    USART_SendData(USART1, TxBuf[count]);                    
    count++;

    if(count == NoofByte)                                  
    {
      /* Disable the USART1 Transmit interrupt */
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      count = 0;   
         
    }  
  }

  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    GUI_receive((uint8_t)USART_ReceiveData(USART1));
  }
}


/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : TxBuffer, TxCounter
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
  rcdsl_parse_data(USART_ReceiveData(USART2));
}


/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
