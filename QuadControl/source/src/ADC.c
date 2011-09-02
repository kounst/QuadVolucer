// Header: ADC1 initialization for directBEMF detection
// File Name: ADC.c
// Author: Konstantin Reichmeyer
// Date:



/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "ADC.h"





/* Configuration of ADC1 */
void  ADC1_Configuration (void)
{
  ADC_InitTypeDef ADC_InitStructure;

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_Cmd(ADC1, ENABLE);

  /* ADC2 regular channels configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_239Cycles5);     //voltage measurement


//  ADC_InjectedSequencerLengthConfig(ADC1, 1);
//  
//
//  ADC_InjectedChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_13Cycles5);
//  ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_CC4);      
  

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
  
  /* Enable ADC1 JEOC interrupt */
//  ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);

  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
