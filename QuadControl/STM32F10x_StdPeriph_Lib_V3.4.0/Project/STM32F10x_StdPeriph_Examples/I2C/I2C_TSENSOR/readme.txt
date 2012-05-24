/**
  @page I2C_TSENSOR I2C_TSENSOR
  
  @verbatim
  ******************** (C) COPYRIGHT 2010 STMicroelectronics *******************
  * @file    I2C/I2C_TSENSOR/readme.txt 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Description of the I2C and LM75 Temperature Sensor communication 
  *          example.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example provides a description of how to use the I2C to communicate with 
an STLM75 (or a compatible device) I2C temperature sensor is mounted on the 
evaluation board and used to get instantaneous external temperature
(-55°C to +125°C).
Thanks to STM32 I2C SMBus feature, we can easiley monitor the temperature 
variations. This is managed by the SMBus Alert which is generating a dedicated 
interrupt informing the system that the temperature is out of the selected 
ranges.
The user can configure the TOS and THYS thanks to a dedicated define values in 
the code.
  
   #define TEMPERATURE_THYS 31
   #define TEMPERATURE_TOS 32

@par Directory contents 

  - I2C/I2C_TSENSOR/stm32f10x_conf.h    Library Configuration file
  - I2C/I2C_TSENSOR/stm32f10x_it.c      Interrupt handlers
  - I2C/I2C_TSENSOR/stm32f10x_it.h      Interrupt handlers header file
  - I2C/I2C_TSENSOR/main.c              Main program
  - I2C/I2C_TSENSOR/system_stm32f10x.c  STM32F10x system source file
  
@par Hardware and Software environment 

  - This example runs on STM32F10x Connectivity line, High-Density, High-Density
    Value line, Medium-Density, XL-Density, Medium-Density Value line, Low-Density 
    and Low-Density Value line Devices.
  
  - This example has been tested with STMicroelectronics STM32100E-EVAL (High-Density
    Value line), STM32100B-EVAL (Medium-Density Value line), STM3210E-EVAL (High-Density
    and XL-Density) and STM3210B-EVAL (Medium-Density) evaluation boards and can be 
    easily tailored to any other supported device and development board.

  - STM3210E-EVAL Set-up
    @note The jumper 11 must be fitted.

  - STM32100E-EVAL Set-up
    @note The jumper 6 must be set in position 2<->3
            

@par How to use it ? 

In order to make the program work, you must do the following :
- Create a project and setup all project configuration
- Add the required Library files:
  - stm32f10x_dma.c
  - stm32f10x_exti.c
  - stm32f10x_fsmc.c 
  - stm32f10x_gpio.c 
  - stm32f10x_i2c.c
  - stm32f10x_rcc.c 
  - stm32f10x_spi.c
  - stm32f10x_usart.c 
  - misc.c      
  - stm32_eval.c (under Utilities\STM32_EVAL)
  - stm32_eval_i2c_tsensor.c (under Utilities\STM32_EVAL\Common)
  - stm32100e_eval_lcd.c (under Utilities\STM32_EVAL\STM32100E_EVAL, for High-Density Value line)
  - stm3210e_eval_lcd.c (under Utilities\STM32_EVAL\STM3210E_EVAL,   for High-Density and XL-Density)
  - stm32100b_eval_lcd.c (under Utilities\STM32_EVAL\STM32100B_EVAL, for Medium-Density Value line)
  - stm3210b_eval_lcd.c (under Utilities\STM32_EVAL\STM3210B_EVAL,   for Medium-Density)
  

- Edit stm32f10x.h file to select the device you are working on.
  
@b Tip: You can tailor the provided project template to run this example, for 
        more details please refer to "stm32f10x_stdperiph_lib_um.chm" user 
        manual; select "Peripheral Examples" then follow the instructions 
        provided in "How to proceed" section.   
- Link all compiled files and load your image into target memory
- Run the example

@note
 - Low-density Value line devices are STM32F100xx microcontrollers where the 
   Flash memory density ranges between 16 and 32 Kbytes.
 - Low-density devices are STM32F101xx, STM32F102xx and STM32F103xx 
   microcontrollers where the Flash memory density ranges between 16 and 32 Kbytes.
 - Medium-density Value line devices are STM32F100xx microcontrollers where
   the Flash memory density ranges between 64 and 128 Kbytes.  
 - Medium-density devices are STM32F101xx, STM32F102xx and STM32F103xx 
   microcontrollers where the Flash memory density ranges between 64 and 128 Kbytes.
 - High-density Value line devices are STM32F100xx microcontrollers where
   the Flash memory density ranges between 256 and 512 Kbytes.   
 - High-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 256 and 512 Kbytes.
 - XL-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 512 and 1024 Kbytes.
 - Connectivity line devices are STM32F105xx and STM32F107xx microcontrollers.
   
 * <h3><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h3>
 */
