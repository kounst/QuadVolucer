// Header: ADC1 initialization for directBEMF detection
// File Name: ADC.c
// Author: Konstantin Reichmeyer
// Date:



/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "ADC.h"






/* Configuration of ADC1 for BEMF detection on Ch5,8,9 */
void  Com_ADC_Configuration (void)
{
  ADC_InitTypeDef ADC_InitStructure;

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 0;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_Cmd(ADC1, ENABLE);

  ADC_InjectedSequencerLengthConfig(ADC1, 1);
  
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_13Cycles5);
  ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_CC4);
  
  //ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
  
  /* Enable ADC1 JEOC interrupt */
  ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
}

void ADC_Current_Configuration(void)
{
  ADC_InitTypeDef ADC_InitStructure;

    /* ADC2 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 0;
  ADC_Init(ADC2, &ADC_InitStructure);
  /* ADC2 regular channels configuration */ 
  ADC_RegularChannelConfig(ADC2, ADC_Channel_7, 1, ADC_SampleTime_1Cycles5);

  ADC_Cmd(ADC2, ENABLE);
  
  //ADC_InjectedSequencerLengthConfig(ADC2, 1);
  
  //ADC_InjectedChannelConfig(ADC2, ADC_Channel_7, 1, ADC_SampleTime_13Cycles5);
  //ADC_ExternalTrigInjectedConvConfig(ADC2, ADC_ExternalTrigInjecConv_T1_TRGO);  

  /* Enable ADC2 reset calibaration register */   
  ADC_ResetCalibration(ADC2);
  
  /* Check the end of ADC2 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC2));

  /* Start ADC2 calibaration */
  ADC_StartCalibration(ADC2);
  
  /* Check the end of ADC2 calibration */
  while(ADC_GetCalibrationStatus(ADC2));

  //ADC_ExternalTrigInjectedConvCmd(ADC2, ENABLE);
  ADC_ExternalTrigConvCmd(ADC2, ENABLE);

  /* Enable ADC1 JEOC interrupt */
  //ADC_ITConfig(ADC2, ADC_IT_JEOC, ENABLE);
  //ADC_ITConfig(ADC2, ADC_IT_EOC, ENABLE);
}


