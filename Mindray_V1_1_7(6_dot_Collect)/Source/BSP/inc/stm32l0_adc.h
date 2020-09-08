/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        stm32l0_adc.h
** ����:          ׯ��Ⱥ
** �汾:          V1.0.0
** ����:          2017-05-09
** ����:          STM32l072 ADC��ͨ������ DMA�������� �ڲ��¶ȴ���������
** ����:          STM32L072 ADCģ���ͨ��DMA���� �ڲ��¶�ADCת��
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#ifndef __STM32L0_ADC_H
#define __STM32L0_ADC_H

#include "stm32l0xx_hal_rcc.h"
#include "stm32l0xx_hal_rcc_ex.h"
#include "stm32l0xx_hal_gpio.h"
#include "stm32l0xx_hal_gpio_ex.h"
#include "stm32l0xx_hal_dma.h"
#include "stm32l0xx_hal_adc.h"
#include "stm32l0xx_hal_adc_ex.h"
#include "stm32l0xx_hal_cortex.h"
#include "VariaType.h"

#define AD_GROUP_MAX      10                                      //ÿ��ͨ���ɼ��������û������Զ���
#define AD_CHANNEL_MAX    2                                       //ʹ��ADCͨ�������û������Զ���
#define AD_SEQBUFF_MAX   (AD_GROUP_MAX * AD_CHANNEL_MAX)          //ADC���л����С

#define VREF30ADDR                      0x1FF8007A                //30���϶�ADC�ο�ֵ��ַ
#define VREF130ADDR                     0x1FF8007E                //130���϶�ADC�ο�ֵ��ַ

//���µĺ���stm32l0xx.h�ж���
//typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
//FunctionalState ֻ�����ֿ��� ʹ�ܺͽ�ֹ

//ADCͨ�����ýṹ��
typedef struct
{
  FunctionalState ADC_IN_0_State;                                 //ADCͨ��0 �򿪻��ֹ
  FunctionalState ADC_IN_1_State;                                 //ADCͨ��1 �򿪻��ֹ
  FunctionalState ADC_IN_2_State;                                 //ADCͨ��2 �򿪻��ֹ
  FunctionalState ADC_IN_3_State;                                 //ADCͨ��3 �򿪻��ֹ
  FunctionalState ADC_IN_4_State;                                 //ADCͨ��4 �򿪻��ֹ
  FunctionalState ADC_IN_5_State;                                 //ADCͨ��5 �򿪻��ֹ
  FunctionalState ADC_IN_6_State;                                 //ADCͨ��6 �򿪻��ֹ
  FunctionalState ADC_IN_7_State;                                 //ADCͨ��7 �򿪻��ֹ
  FunctionalState ADC_IN_8_State;                                 //ADCͨ��8 �򿪻��ֹ
  FunctionalState ADC_IN_9_State;                                 //ADCͨ��9 �򿪻��ֹ
  FunctionalState ADC_Temp_State;                                 //�¶�ͨ�� �򿪻��ֹ
}ADC_Chn_ConfigTypeDef;

//***************************************************************************************************************************************************************
// ����               : ADC_GPIO_Config()
// ��������           : 2017-05-09
// ����        	      : ׯ��Ⱥ
// ����               : ��ͨ��ADCģ�� ģ��������������
// �������           : ADC_Chn_ConfigTypeDef�ṹ��(��ͨ��ADC����ʹ��״̬)
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ADC����ʹ��״ֻ̬������(ENABLE��DISABLE)
// �޸�����           :
//***************************************************************************************************************************************************************

void ADC_GPIO_Config(ADC_Chn_ConfigTypeDef *gADC_ChnConfig);

//***************************************************************************************************************************************************************
// ����               : ADC_DMA_Init()
// ��������           : 2017-05-09
// ����        	      : ׯ��Ⱥ
// ����               : ��ͨ��ADCģ��DMA��ʽ��ʼ��
// �������           : ADC_Chn_ConfigTypeDef(ADCͨ��ʹ�ܲ���)
// �������           : ADC_HandleTypeDef(ADCģ�鴦��),DMA_HandleTypeDef(DMAģ�鴦��)
// ���ؽ��           : ��
// ע���˵��         : ADC����ʹ��״ֻ̬������(ENABLE��DISABLE),ADC_HandleTypeDef����������ȶ����ȫ�ֽṹ�����
// �޸�����           :
//***************************************************************************************************************************************************************
void ADC_DMA_Init(ADC_HandleTypeDef *gADC_HandleStructure, DMA_HandleTypeDef *gDMA_HandleStructure, ADC_Chn_ConfigTypeDef *gADC_ChnConfig);

extern float TempVarySlope;                                                   //�¶ȱ仯б��
extern uint16_t Temp30_Vref_Adc;                                               //30���϶�ADC��׼ֵ
extern uint16_t Temp130_Vref_Adc;                                              //130���϶�ADC��׼ֵ
extern volatile uint16_t gADC_GetData[AD_GROUP_MAX][AD_CHANNEL_MAX];          //����ͨ��ADC DMAʹ�õĻ���

#endif