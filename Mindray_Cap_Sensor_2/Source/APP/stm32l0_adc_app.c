/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        stm32l0_adc_app.c
** ����:          ׯ��Ⱥ
** �汾:          V1.0.0
** ����:          2017-05-15
** ����:          ADCģ���Ӧ��
** ����:          ADC DMA �������� ADC�¶����ݻ�ȡ�봦��
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/
#include "stm32l0_adc_app.h"

ADC_HandleTypeDef ADC_HandleInitStructure;
DMA_HandleTypeDef DMA_ADC_HandleInitStructure;

//�û�ʹ�õ�ADCת���������
uint16_t uADC_GetResult[AD_GROUP_MAX][AD_CHANNEL_MAX];

//****************************************************************************************************************************************************************
// ����               : ADC_Chn_Config()
// ��������           : 2017-05-15
// ����               : ׯ��Ⱥ
// ����               : ѡ��ADCͨ������ADCͨ����ʼ��
// �������           : ��
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ADCͨ���û����Զ���
// �޸�����           :
//****************************************************************************************************************************************************************

void ADC_Chn_Config(void)
{
  ADC_Chn_ConfigTypeDef gADC_ChnInitStructure;
  //ʹ��ͨ��2
  gADC_ChnInitStructure.ADC_IN_2_State = ENABLE;
  //ʹ���ڲ��¶�ͨ��
  gADC_ChnInitStructure.ADC_Temp_State = ENABLE;
  //ADCͨ����������
  ADC_GPIO_Config(&gADC_ChnInitStructure);
  //ADC DMA���䷽ʽ����
  ADC_DMA_Init(&ADC_HandleInitStructure, &DMA_ADC_HandleInitStructure, &gADC_ChnInitStructure);
}

//****************************************************************************************************************************************************************
// ����               : ADC_GetChn_Value()
// ��������           : 2017-05-15
// ����               : ׯ��Ⱥ
// ����               : �û���ȡADCͨ���ϵ�����
// �������           : ��
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ADCͨ���Ͳ��������û����Զ���
// �޸�����           :
//****************************************************************************************************************************************************************

void ADC_GetChn_Value(void)
{
  uint8_t Ad_group;
  uint8_t Ad_Channel;
  static uint16_t DMA_TC_Cnt = 0;
  //DMA������ɲŻᴦ������
  if(RESET != __HAL_DMA_GET_FLAG(&DMA_ADC_HandleInitStructure, DMA_FLAG_TC1))
  {
    __HAL_DMA_CLEAR_FLAG(&DMA_ADC_HandleInitStructure, DMA_FLAG_TC1);
    DMA_TC_Cnt++;
    
    if(AD_SEQBUFF_MAX <= DMA_TC_Cnt)
    {
      //��ȡȫ��ADCͨ��������
      for(Ad_Channel = 0; Ad_Channel < AD_CHANNEL_MAX; Ad_Channel++)
      {
        for(Ad_group = 0; Ad_group < AD_GROUP_MAX; Ad_group++)
        {
          uADC_GetResult[Ad_group][Ad_Channel] = gADC_GetData[Ad_group][Ad_Channel];
        }
      }
      ADC_TemperData_Handle();
      RunVarParam.Temper_Fresh_Flag = FRESH_OK;
      DMA_TC_Cnt = 0;
    }
  }
}

//****************************************************************************************************************************************************************
// ����               : ADC_TemperData_Handle()
// ��������           : 2017-05-15
// ����               : ׯ��Ⱥ
// ����               : ADC�¶�ͨ�����ݴ���
// �������           : ��
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : �¶ȼ���ɲο�STM32L072�����ֲ�
// �޸�����           :
//****************************************************************************************************************************************************************
void ADC_TemperData_Handle(void)
{
    uint8_t cn;
    float Result;
    float Temp_rate;
    uint32_t AD_Sum;
    uint16_t AD_Average;
    AD_Sum = 0;
    for(cn = 0; cn < AD_GROUP_MAX; cn++)
    {
        //���¶�ͨ�������������ۼ�
        AD_Sum += uADC_GetResult[cn][1];
    }
    //����ƽ��ֵ
    AD_Average = AD_Sum / AD_GROUP_MAX;
    //��У׼�¶ȱȽϼ���
    Result = (float)AD_Average / 0.91 - Temp30_Vref_Adc;
    //�����¶ȱ仯б�ʵõ�ʵ���¶�
    RunVarParam.TemperInAir = Result * TempVarySlope + 30;
    //�����¶�K,Bֵ�����¶�
    RunVarParam.TemperInAir = RunVarParam.TemperInAir * UserParam.Temper_K1 + UserParam.Temper_B1;
    //���¶���ȫ�¶ȷ�Χ�ڵı���
    Temp_rate = (RunVarParam.TemperInAir + 40.0) / 140.0;
    //�����ʰٷְ�
    if(1.0 < Temp_rate)
    {
        Temp_rate = 1.0;
    }
    //���¶ȼ�����������DA��Ӧ��ADֵ
    RunVarParam.TemperInAirAD = (uint16_t)(Temp_rate * ProductParam.TempDARange) + ProductParam.TempDAMin; 
}
