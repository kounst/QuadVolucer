/**
  ******************************************************************************
  * @file    FSMC/OneNAND/main.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32_eval.h"
#include "stm32100e_eval_fsmc_onenand.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup FSMC_OneNAND
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define BUFFER_SIZE        0x200
#define WRITE_READ_ADDR    0x800
uint16_t TxBuffer[BUFFER_SIZE], RxBuffer[BUFFER_SIZE];

OneNAND_IDTypeDef OneNand_ID;
uint16_t WriteReadStatus = 0;
uint32_t index = 0;
uint16_t OneNand_SAMSUNG_ID[2] = {0x00EC, 0x0025};

/* Private function prototypes -----------------------------------------------*/
void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */
  
  /* Initialize LEDs on STM32100E-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);  
  
  /* FSMC Initialization */
  OneNAND_Init();

  /* Read OneNAND memory ID */
  OneNAND_ReadID(&OneNand_ID);

  /* Verify the OneNAND ID */ 
  if((OneNand_SAMSUNG_ID[0] == OneNand_ID.Manufacturer_ID) && (OneNand_SAMSUNG_ID[1] == OneNand_ID.Device_ID)) 
  {
    /* Erase the OneNAND Block */
    OneNAND_EraseBlock(WRITE_READ_ADDR);
    
    /* Write data to FSMC OneNAND memory */
    /* Fill the buffer to send */
    Fill_Buffer(TxBuffer, BUFFER_SIZE, 0x365A);

    OneNAND_WriteBuffer(TxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
  
    /* Read back the written data */
    OneNAND_AsynchronousRead(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
 
    /* Verify the written data */
    for(index = 0; index < BUFFER_SIZE; index++)
    {
      if(TxBuffer[index] != RxBuffer[index])
      {     
        WriteReadStatus++;
      } 
    }
    if (WriteReadStatus == 0)
    {	/* OK */
      /* Turn on LD1 */
      STM_EVAL_LEDToggle(LED1);
    }
    else
    { /* KO */
      /* Turn on LD2 */
      STM_EVAL_LEDToggle(LED2);     
    }
  }
  else
  {
    /* Turn on LD3 */
    STM_EVAL_LEDToggle(LED3); 
  }

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  Fill the buffer.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferSize: size of the buffer to fill
  * @param  Offset: first value to fill on the Buffer
  * @retval None
  */
void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
  uint16_t IndexTmp = 0;

  /* Put in global buffer diferent values */
  for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
  {
    pBuffer[IndexTmp] = IndexTmp + Offset;
  }
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

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
