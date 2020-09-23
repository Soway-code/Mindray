/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        stm32l0_timer_app.c
** ����:          ׯ��Ⱥ
** �汾:          V1.0.0
** ����:          2017-05-06
** ����:          ��ʱ��Ӧ��ʵ��
** ����:          ��ʱ����ʱ����
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#include "stm32l0_timer_app.h"

TIM_HandleTypeDef TIM2_HandleInitStruct;                                        //TIM2������ʼ���ṹ��
TIM_HandleTypeDef TIM3_HandleInitStruct;                                        //TIM3������ʼ���ṹ��

//****************************************************************************************************************************************************************
// ����               : gSimpleDelay()
// ��������           : 2017-05-06
// ����               : ׯ��Ⱥ
// ����               : ����ʱ����(���뼶��)
// �������           : gcnt(�������)
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ������ʱ
// �޸�����           :
//****************************************************************************************************************************************************************

void gSimpleDelay(uint32_t gcnt)
{
  uint32_t gCnt;
  gCnt = gcnt * 6000;
  while(gCnt--)
  {
    
  }
}

//****************************************************************************************************************************************************************
// ����               : gTim2Config()
// ��������           : 2017-05-06
// ����               : ׯ��Ⱥ
// ����               : TIM2����Ϊ��ʱ�ж�ģʽ��100ms��ʱʱ�䣬���ڲɼ�Pcapֵ
// �������           : ��
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ׼ȷ��ʱ
// �޸�����           :
//****************************************************************************************************************************************************************

void gTim2Config(void)
{
  TIM_BaseTypeDef TIM2_BaseInitStructure;
  
  TIM2_BaseInitStructure.TimX = TIM2;
  TIM2_BaseInitStructure.TimXClkDiv = TIM_CLOCKDIVISION_DIV1;
  TIM2_BaseInitStructure.TimXCntMode = TIM_COUNTERMODE_UP;
  TIM2_BaseInitStructure.TimXPeriod = 1999;      //100MS
  TIM2_BaseInitStructure.TimXPrescaler = 1599;  //10KHZ
  TIM2_BaseInitStructure.TimXPreemptPrio = 2;
  TIM2_BaseInitStructure.TimXSubPrio = 1;
  TIM2_BaseInitStructure.TimXTimingEn = TIM_TIMING_ENABLE;
  gTimPeriodConfig(&TIM2_HandleInitStruct, &TIM2_BaseInitStructure);
  HAL_TIM_Base_Start_IT(&TIM2_HandleInitStruct);
}

//****************************************************************************************************************************************************************
// ����               : gTim3_BaudrateRelate()
// ��������           : 2017-05-06
// ����               : ׯ��Ⱥ
// ����               : TIM3����Ϊͨ�Ŷ�ʱ���벨������أ�����֪ͨ����������
// �������           : Baudrate(������)
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ׼ȷ��ʱ
// �޸�����           :
//****************************************************************************************************************************************************************

void gTim3_BaudrateRelate(uint32_t Baudrate)
{
  TIM_BaseTypeDef TIM3_BaseInitStructure;
  
  TIM3_BaseInitStructure.TimX = TIM3;
  TIM3_BaseInitStructure.TimXClkDiv = TIM_CLOCKDIVISION_DIV1;
  TIM3_BaseInitStructure.TimXCntMode = TIM_COUNTERMODE_UP;
  TIM3_BaseInitStructure.TimXPrescaler = (SystemCoreClock / 1000000) - 1;
  TIM3_BaseInitStructure.TimXPeriod = (1000000 / Baudrate) * 35;
  TIM3_BaseInitStructure.TimXPreemptPrio = 2;
  TIM3_BaseInitStructure.TimXSubPrio = 0;
  TIM3_BaseInitStructure.TimXTimingEn = TIM_TIMING_ENABLE;
  
  gTimPeriodConfig(&TIM3_HandleInitStruct, &TIM3_BaseInitStructure);
  __HAL_TIM_CLEAR_IT(&TIM3_HandleInitStruct, TIM_IT_UPDATE);
}
