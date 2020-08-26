/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        stm32l0_timer.c
** ����:          ׯ��Ⱥ
** �汾:          V1.0.0
** ����:          2017-05-05
** ����:          STM32l072 ͨ�ö�ʱ������
** ����:          STM32L072 ͨ�ö�ʱ ���ж�ѡ��
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#include "stm32l0_timer.h"

//****************************************************************************************************************************************************************
// ����               : gTimPeriodConfig()
// ��������           : 2017-05-05
// ����               : ׯ��Ⱥ
// ����               : ͨ�ö�ʱ����ʱ��������
// �������           : TIM_BaseTypeDef�ṹ��(ʱ������)
// �������           : TIM_HandleTypeDef�ṹ��(��ʱ������ṹ��)
// ���ؽ��           : TIM_SET_OK(�ɹ�),TIM_SET_NK(ʧ��)
// ע���˵��         : ���ж�ѡ��
// �޸�����           :
//****************************************************************************************************************************************************************

uint8_t gTimPeriodConfig(TIM_HandleTypeDef *gTim_InitStructure, TIM_BaseTypeDef *gTim_BaseInitStructure)
{
  IRQn_Type TIM_IRQn;
  
  HAL_StatusTypeDef HAL_Status;
  //��ַ��Ч�����ش���
  if((0 == gTim_InitStructure) || (0 == gTim_BaseInitStructure))
  {
    return TIM_SET_NK;
  }
  //ͨ�ö�ʱ��ѡ��
  if(TIM2 == gTim_BaseInitStructure ->TimX)
  {
    TIM_IRQn = TIM2_IRQn;
    
    __HAL_RCC_TIM2_CLK_ENABLE();
  }
  else if(TIM3 == gTim_BaseInitStructure ->TimX)
  {
    TIM_IRQn = TIM3_IRQn;
    
    __HAL_RCC_TIM3_CLK_ENABLE();
  }
  else if(TIM21 == gTim_BaseInitStructure ->TimX)
  {
    TIM_IRQn = TIM21_IRQn;
    
    __HAL_RCC_TIM21_CLK_ENABLE();
  }
  else if(TIM22 == gTim_BaseInitStructure ->TimX)
  {
    TIM_IRQn = TIM22_IRQn;
    
    __HAL_RCC_TIM22_CLK_ENABLE();
  }
  else
  {
    //��Чѡ�񣬷��ش���
    return TIM_SET_NK;
  }
  //����ģʽ
  gTim_InitStructure ->Init.CounterMode          = gTim_BaseInitStructure ->TimXCntMode;
  //ʱ�ӷ�Ƶ
  gTim_InitStructure ->Init.ClockDivision        = gTim_BaseInitStructure ->TimXClkDiv;
  //������������
  gTim_InitStructure ->Init.Period               = gTim_BaseInitStructure ->TimXPeriod;
  //��ʱ����Ƶ
  gTim_InitStructure ->Init.Prescaler            = gTim_BaseInitStructure ->TimXPrescaler;
  //ѡ��ʱ��
  gTim_InitStructure ->Instance                  = gTim_BaseInitStructure ->TimX;
  //��ʱ��ʱ����ʼ��
  HAL_Status = HAL_TIM_Base_Init(gTim_InitStructure);
  //��ʼ��ʧ���򷵻ش���
  if(HAL_OK != HAL_Status)
  {
    return TIM_SET_NK;
  }
  //��ʱ����������
  gTim_InitStructure ->Instance ->CNT = 0;
  //�Ƿ�ʹ�ܶ�ʱ�ж�
  if(TIM_TIMING_ENABLE == gTim_BaseInitStructure ->TimXTimingEn)
  {
    //�����жϱ�־
    __HAL_TIM_CLEAR_IT(gTim_InitStructure, TIM_IT_UPDATE);
    //�����ж����ȼ�
    HAL_NVIC_SetPriority(TIM_IRQn, gTim_BaseInitStructure ->TimXPreemptPrio, gTim_BaseInitStructure ->TimXSubPrio);
    //ʹ�ܶ�ʱ���ж�  
    HAL_NVIC_EnableIRQ(TIM_IRQn);
    //�ȹر��ж�
    HAL_Status = HAL_TIM_Base_Stop_IT(gTim_InitStructure);
    //�ر��ж�ʧ�ܷ��ش���
    if(HAL_OK != HAL_Status)
    {
      return TIM_SET_NK;
    }
  }
  else
  {
    //�����ֹ��ʱ�ж�
    if((TIM2_IRQn == TIM_IRQn) || (TIM3_IRQn == TIM_IRQn) || (TIM21_IRQn == TIM_IRQn) || (TIM22_IRQn == TIM_IRQn))
    {
      HAL_NVIC_DisableIRQ(TIM_IRQn);
    }
    
    HAL_Status = HAL_TIM_Base_Stop(gTim_InitStructure);
    
    if(HAL_OK != HAL_Status)
    {
      return TIM_SET_NK;
    }
  }
  
  return TIM_SET_OK;
}
