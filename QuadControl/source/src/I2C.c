// Header:
// File Name: 
// Author:
// Date:


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "I2C.h"

DMA_InitTypeDef  I2CDMA_InitStructure;
__IO uint32_t I2CDirection = I2C_DIRECTION_TX;
__IO uint32_t NumbOfBytes1;
__IO uint32_t NumbOfBytes2;
__IO uint8_t Address;

/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : I2C Configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_Configuration(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 

  I2C_DeInit(I2C1);

  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x40;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 40000;

  /* I2C Peripheral Enable */
  I2C_Cmd(I2C1, ENABLE);
  /* Apply I2C configuration after enabling it */
  I2C_Init(I2C1, &I2C_InitStructure);
  //I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
}


/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : I2C Configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_Configuration(void)
{
  I2C_InitTypeDef  I2C_InitStructure;

  I2C_DeInit(I2C2);
   

  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x40;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  
  I2C_InitStructure.I2C_ClockSpeed = 150000;
  
  /* I2C Peripheral Enable */
  I2C_Cmd(I2C2, ENABLE);
  /* Apply I2C configuration after enabling it */
  I2C_Init(I2C2, &I2C_InitStructure);
  I2C_ITConfig(I2C2, I2C_IT_EVT , ENABLE);
}


void readmem (unsigned char addr, uint8_t *buf, int n)
{
	while(I2C_GetFlagStatus (I2C1, I2C_FLAG_BUSY));
	/* Send START condition */
	I2C_GenerateSTART (I2C1, ENABLE);

	/* Test on EV5 and clear it */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	/* In the case of a single data transfer disable ACK before reading the data */

	/* Send EEPROM address for write */
	I2C_Send7bitAddress (I2C1, ITG3200_SLAVE_ADDR, I2C_Direction_Transmitter);

	/* Test on EV6 and clear it */
	while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/* Send the EEPROM's internal address to write to */
	I2C_SendData (I2C1, addr);
	/* Test on EV8 and clear it */
	while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	/* Send STRAT condition a second time */  
	I2C_GenerateSTART (I2C1, ENABLE);
	/* Test on EV5 and clear it */
	while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	/* Send EEPROM address for read */
	I2C_Send7bitAddress (I2C1, ITG3200_SLAVE_ADDR, I2C_Direction_Receiver);
	/* Test on EV6 and clear it */
	while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
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
			*buf = I2C_ReceiveData (I2C1);
			/* Point to the next location where the byte read will be saved */
			buf ++; 
			/* Decrement the read bytes counter */
			n --;    
		}   
	}

	/* Enable Acknowledgement to be ready for another reception */
	I2C_AcknowledgeConfig (I2C1, ENABLE);
}


/**
  * @brief  Reads buffer of bytes  from the slave.
  * @param pBuffer: Buffer of bytes to be read from the slave.
  * @param NumByteToRead: Number of bytes to be read by the Master.
  * @param Mode: Polling or DMA or Interrupt having the highest priority in the application.
  * @param SlaveAddress: The address of the slave to be addressed by the Master.
  * @retval : None.
  */
Status I2C_Master_BufferRead(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToRead, I2C_ProgrammingModel Mode, uint8_t SlaveAddress)

{
    __IO uint32_t temp = 0;
    __IO uint32_t Timeout = 0;

    /* Enable I2C errors interrupts (used in all modes: Polling, DMA and Interrupts */
    I2Cx->CR2 |= I2C_IT_ERR;

    if (Mode == DMA) /* I2Cx Master Reception using DMA */
    {
        /* Configure I2Cx DMA channel */
        I2C_DMAConfig(I2Cx, pBuffer, NumByteToRead, I2C_DIRECTION_RX);
        /* Set Last bit to have a NACK on the last received byte */
        I2Cx->CR2 |= CR2_LAST_Set;
        /* Enable I2C DMA requests */
        I2Cx->CR2 |= CR2_DMAEN_Set;
        Timeout = 0xFFFF;
        /* Send START condition */
        I2Cx->CR1 |= CR1_START_Set;
        /* Wait until SB flag is set: EV5  */
        while ((I2Cx->SR1&0x0001) != 0x0001)
        {
            if (Timeout-- == 0)
                return Error;
        }
        Timeout = 0xFFFF;
        /* Send slave address */
        /* Set the address bit0 for read */
        SlaveAddress |= OAR1_ADD0_Set;
        Address = SlaveAddress;
        /* Send the slave address */
        I2Cx->DR = Address;
        /* Wait until ADDR is set: EV6 */
        while ((I2Cx->SR1&0x0002) != 0x0002)
        {
            if (Timeout-- == 0)
                return Error;
        }
        /* Clear ADDR flag by reading SR2 register */
        temp = I2Cx->SR2;
        if (I2Cx == I2C1)
        {
            /* Wait until DMA end of transfer */
            while (!DMA_GetFlagStatus(DMA1_FLAG_TC7));
            /* Disable DMA Channel */
            DMA_Cmd(I2C1_DMA_CHANNEL_RX, DISABLE);
            /* Clear the DMA Transfer Complete flag */
            DMA_ClearFlag(DMA1_FLAG_TC7);

        }

        else /* I2Cx = I2C2*/
        {
            /* Wait until DMA end of transfer */
            while (!DMA_GetFlagStatus(DMA1_FLAG_TC5));
            /* Disable DMA Channel */
            DMA_Cmd(I2C2_DMA_CHANNEL_RX, DISABLE);
            /* Clear the DMA Transfer Complete flag */
            DMA_ClearFlag(DMA1_FLAG_TC5);
        }
        /* Program the STOP */
        I2Cx->CR1 |= CR1_STOP_Set;
        /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
        while ((I2Cx->CR1&0x200) == 0x200);
    }

    else if (Mode == Polling) /* I2Cx Master Reception using Polling */
    {


        if (NumByteToRead == 1)
        {
            Timeout = 0xFFFF;
            /* Send START condition */
            I2Cx->CR1 |= CR1_START_Set;
            /* Wait until SB flag is set: EV5  */
            while ((I2Cx->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* Send slave address */
            /* Reset the address bit0 for read */
            SlaveAddress |= OAR1_ADD0_Set;
            Address = SlaveAddress;
            /* Send the slave address */
            I2Cx->DR = Address;
            /* Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
            and program the STOP just after ADDR is cleared. The EV6_3 
            software sequence must complete before the current byte end of transfer.*/
            /* Wait until ADDR is set */
            Timeout = 0xFFFF;
            while ((I2Cx->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* Clear ACK bit */
            I2Cx->CR1 &= CR1_ACK_Reset;
            /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
            software sequence must complete before the current byte end of transfer */
            __disable_irq();
            /* Clear ADDR flag */
            temp = I2Cx->SR2;
            /* Program the STOP */
            I2Cx->CR1 |= CR1_STOP_Set;
            /* Re-enable IRQs */
            __enable_irq();
            /* Wait until a data is received in DR register (RXNE = 1) EV7 */
            while ((I2Cx->SR1 & 0x00040) != 0x000040);
            /* Read the data */
            *pBuffer = I2Cx->DR;
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2Cx->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2Cx->CR1 |= CR1_ACK_Set;

        }

        else if (NumByteToRead == 2)
        {
            /* Set POS bit */
            I2Cx->CR1 |= CR1_POS_Set;
            Timeout = 0xFFFF;
            /* Send START condition */
            I2Cx->CR1 |= CR1_START_Set;
            /* Wait until SB flag is set: EV5 */
            while ((I2Cx->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            Timeout = 0xFFFF;
            /* Send slave address */
            /* Set the address bit0 for read */
            SlaveAddress |= OAR1_ADD0_Set;
            Address = SlaveAddress;
            /* Send the slave address */
            I2Cx->DR = Address;
            /* Wait until ADDR is set: EV6 */
            while ((I2Cx->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* EV6_1: The acknowledge disable should be done just after EV6,
            that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and 
            ACK clearing */
            __disable_irq();
            /* Clear ADDR by reading SR2 register  */
            temp = I2Cx->SR2;
            /* Clear ACK */
            I2Cx->CR1 &= CR1_ACK_Reset;
            /*Re-enable IRQs */
            __enable_irq();
            /* Wait until BTF is set */
            while ((I2Cx->SR1 & 0x00004) != 0x000004);
            /* Disable IRQs around STOP programming and data reading because of the limitation ?*/
            __disable_irq();
            /* Program the STOP */
            I2C_GenerateSTOP(I2Cx, ENABLE);
            /* Read first data */
            *pBuffer = I2Cx->DR;
            /* Re-enable IRQs */
            __enable_irq();
            /**/
            pBuffer++;
            /* Read second data */
            *pBuffer = I2Cx->DR;
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2Cx->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2Cx->CR1  |= CR1_ACK_Set;
            /* Clear POS bit */
            I2Cx->CR1  &= CR1_POS_Reset;

        }

        else

        {

            Timeout = 0xFFFF;
            /* Send START condition */
            I2Cx->CR1 |= CR1_START_Set;
            /* Wait until SB flag is set: EV5 */
            while ((I2Cx->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            Timeout = 0xFFFF;
            /* Send slave address */
            /* Reset the address bit0 for write */
            SlaveAddress |= OAR1_ADD0_Set;;
            Address = SlaveAddress;
            /* Send the slave address */
            I2Cx->DR = Address;
            /* Wait until ADDR is set: EV6 */
            while ((I2Cx->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* Clear ADDR by reading SR2 status register */
            temp = I2Cx->SR2;
            /* Write address to read */
            I2Cx->DR = 27;
            /* While there is data to be read */
            while (NumByteToRead)
            {
                /* Receive bytes from first byte until byte N-3 */
                if (NumByteToRead != 3)
                {
                    /* Poll on BTF to receive data because in polling mode we can not guarantee the
                    EV7 software sequence is managed before the current byte transfer completes */
                    while ((I2Cx->SR1 & 0x00004) != 0x000004);
                    /* Read data */
                    *pBuffer = I2Cx->DR;
                    /* */
                    pBuffer++;
                    /* Decrement the read bytes counter */
                    NumByteToRead--;
                }

                /* it remains to read three data: data N-2, data N-1, Data N */
                if (NumByteToRead == 3)
                {

                    /* Wait until BTF is set: Data N-2 in DR and data N -1 in shift register */
                    while ((I2Cx->SR1 & 0x00004) != 0x000004);
                    /* Clear ACK */
                    I2Cx->CR1 &= CR1_ACK_Reset;

                    /* Disable IRQs around data reading and STOP programming because of the
                    limitation ? */
                    __disable_irq();
                    /* Read Data N-2 */
                    *pBuffer = I2Cx->DR;
                    /* Increment */
                    pBuffer++;
                    /* Program the STOP */
                    I2Cx->CR1 |= CR1_STOP_Set;
                    /* Read DataN-1 */
                    *pBuffer = I2Cx->DR;
                    /* Re-enable IRQs */
                    __enable_irq();
                    /* Increment */
                    pBuffer++;
                    /* Wait until RXNE is set (DR contains the last data) */
                    while ((I2Cx->SR1 & 0x00040) != 0x000040);
                    /* Read DataN */
                    *pBuffer = I2Cx->DR;
                    /* Reset the number of bytes to be read by master */
                    NumByteToRead = 0;

                }
            }
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2Cx->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2Cx->CR1 |= CR1_ACK_Set;

        }

    }

    else /* I2Cx Master Reception using Interrupts with highest priority in an application */
    {
        /* Enable EVT IT*/
        I2Cx->CR2 |= I2C_IT_EVT;
        /* Enable BUF IT */
        I2Cx->CR2 |= I2C_IT_BUF;
        /* Set the I2C direction to reception */
        I2CDirection = I2C_DIRECTION_RX;
        SlaveAddress |= OAR1_ADD0_Set;
        Address = SlaveAddress;
        if (I2Cx == I2C1)    NumbOfBytes1 = NumByteToRead;
        else NumbOfBytes2 = NumByteToRead;
        /* Send START condition */
        I2Cx->CR1 |= CR1_START_Set;
        /* Wait until the START condition is generated on the bus: START bit is cleared by hardware */
        while ((I2Cx->CR1&0x100) == 0x100);
        /* Wait until BUSY flag is reset (until a STOP is generated) */
        while ((I2Cx->SR2 &0x0002) == 0x0002);
        /* Enable Acknowledgement to be ready for another reception */
        I2Cx->CR1 |= CR1_ACK_Set;
    }

    return Success;
}


/**
  * @brief  Initializes DMA channel used by the I2C Write/read routines.
  * @param  None.
  * @retval None.
  */
void I2C_DMAConfig(I2C_TypeDef* I2Cx, uint8_t* pBuffer, uint32_t BufferSize, uint32_t Direction)
{
    /* Initialize the DMA with the new parameters */
    if (Direction == I2C_DIRECTION_TX)
    {
        /* Configure the DMA Tx Channel with the buffer address and the buffer size */
        I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
        I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        I2CDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;

        if (I2Cx == I2C1)
        {
            I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
            DMA_Cmd(I2C1_DMA_CHANNEL_TX, DISABLE);
            DMA_Init(I2C1_DMA_CHANNEL_TX, &I2CDMA_InitStructure);
            DMA_Cmd(I2C1_DMA_CHANNEL_TX, ENABLE);
        }
        else
        {
            I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C2_DR_Address;
            DMA_Cmd(I2C2_DMA_CHANNEL_TX, DISABLE);
            DMA_Init(I2C2_DMA_CHANNEL_TX, &I2CDMA_InitStructure);
            DMA_Cmd(I2C2_DMA_CHANNEL_TX, ENABLE);
        }
    }
    else /* Reception */
    {
        /* Configure the DMA Rx Channel with the buffer address and the buffer size */
        I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
        I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        I2CDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;
        if (I2Cx == I2C1)
        {

            I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
            DMA_Cmd(I2C1_DMA_CHANNEL_RX, DISABLE);
            DMA_Init(I2C1_DMA_CHANNEL_RX, &I2CDMA_InitStructure);
            DMA_Cmd(I2C1_DMA_CHANNEL_RX, ENABLE);
        }

        else
        {
            I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C2_DR_Address;
            DMA_Cmd(I2C2_DMA_CHANNEL_RX, DISABLE);
            DMA_Init(I2C2_DMA_CHANNEL_RX, &I2CDMA_InitStructure);
            DMA_Cmd(I2C2_DMA_CHANNEL_RX, ENABLE);
        }

    }
}
