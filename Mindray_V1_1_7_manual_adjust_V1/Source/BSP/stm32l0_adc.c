/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        stm32l0_adc.c
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

#include "stm32l0_adc.h"

float TempVarySlope;                                                           //�¶ȱ仯б��
uint16_t Temp30_Vref_Adc;                                                       //30���϶�ADC��׼ֵ
uint16_t Temp130_Vref_Adc;                                                      //130���϶�ADC��׼ֵ
volatile uint16_t gADC_GetData[AD_GROUP_MAX][AD_CHANNEL_MAX];                  //����ͨ��ADC DMAʹ�õ��ڴ�

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

void ADC_GPIO_Config(ADC_Chn_ConfigTypeDef *gADC_ChnConfig)
{
  uint16_t sGPIO_Pin;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //GPIOA�˿��κ�һADCͨ��ģ������ʹ��
  if((ENABLE == gADC_ChnConfig ->ADC_IN_0_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_1_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_2_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_3_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_4_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_5_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_6_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_7_State) 
    )
  {
    sGPIO_Pin = 0;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    //GPIOA�ϵ�ģ����������ѡ��
    if(ENABLE == gADC_ChnConfig ->ADC_IN_0_State)
    {
      sGPIO_Pin |= GPIO_PIN_0;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_1_State)
    {
      sGPIO_Pin |= GPIO_PIN_1;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_2_State)
    {
      sGPIO_Pin |= GPIO_PIN_2;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_3_State)
    {
      sGPIO_Pin |= GPIO_PIN_3;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_4_State)
    {
      sGPIO_Pin |= GPIO_PIN_4;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_5_State)
    {
      sGPIO_Pin |= GPIO_PIN_5;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_6_State)
    {
      sGPIO_Pin |= GPIO_PIN_6;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_7_State)
    {
      sGPIO_Pin |= GPIO_PIN_7;
    }
    //��������
    GPIO_InitStructure.Pin = sGPIO_Pin;
    //ģ������ģʽ
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    //��������
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    //��������
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    //��ʼ��GPIO���Ų���
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
  }
  //ADC��8,9ͨ��
  if((ENABLE == gADC_ChnConfig ->ADC_IN_8_State)
     ||(ENABLE == gADC_ChnConfig ->ADC_IN_9_State)
    )
  {
    sGPIO_Pin = 0;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    //ģ����������ѡ��
    if(ENABLE == gADC_ChnConfig ->ADC_IN_8_State)
    {
      sGPIO_Pin |= GPIO_PIN_0;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_9_State)
    {
      sGPIO_Pin |= GPIO_PIN_1;
    }
    //��������
    GPIO_InitStructure.Pin = sGPIO_Pin;
    //ģ������ģʽ
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    //��������
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    //��������
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    //��ʼ�����Ų���
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  }
  //ʹ���ڲ��¶ȴ�����
  if(ENABLE == gADC_ChnConfig ->ADC_Temp_State)
  {
    HAL_ADCEx_EnableVREFINTTempSensor();
  }
}

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
void ADC_DMA_Init(ADC_HandleTypeDef *gADC_HandleStructure, DMA_HandleTypeDef *gDMA_HandleStructure, ADC_Chn_ConfigTypeDef *gADC_ChnConfig)
{
  //ADCͨ������
  uint8_t Chn_Rank;
  //ADC�궨ϵ��
  uint32_t CalibrationFactor;
  //ADCͨ�����ýṹ��
  ADC_ChannelConfTypeDef ADC_ChannelConfStucture;
  //ADC�κ�һͨ��ʹ��
  if((ENABLE == gADC_ChnConfig ->ADC_IN_0_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_1_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_2_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_3_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_4_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_5_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_6_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_7_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_8_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_IN_9_State) ||
     (ENABLE == gADC_ChnConfig ->ADC_Temp_State) 
    )
  {
    //��ADCʱ��
    __HAL_RCC_ADC1_CLK_ENABLE();
    //��DMAʱ��
    __HAL_RCC_DMA1_CLK_ENABLE();
    //ͨ������������
    Chn_Rank = 0;
    //ADCʱ��ͬ��PCLK 4��Ƶ
    gADC_HandleStructure ->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    //����ת��ģʽ
    gADC_HandleStructure ->Init.ContinuousConvMode = ENABLE;
    //ADC�����Ҷ���
    gADC_HandleStructure ->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //ͨ��ȫ����ת��
    gADC_HandleStructure ->Init.DiscontinuousConvMode = ENABLE;
    //DMA��������ʹ��
    gADC_HandleStructure ->Init.DMAContinuousRequests = ENABLE;
    //ADC����ת��������־
    gADC_HandleStructure ->Init.EOCSelection = ADC_EOC_SEQ_CONV;
    //ADCת���������
    gADC_HandleStructure ->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    //�ޱ��ش���
    gADC_HandleStructure ->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //���������ݸ���
    gADC_HandleStructure ->Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    //ADC 12bit���ݷֱ���
    gADC_HandleStructure ->Init.Resolution = ADC_RESOLUTION_12B;
    //ADC��������
    gADC_HandleStructure ->Init.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
    //ADC����ͨ����С����ɨ��
    gADC_HandleStructure ->Init.ScanConvMode = ADC_SCAN_ENABLE;
    //ADCģ�鲻����
    gADC_HandleStructure ->Lock = HAL_UNLOCKED;
    //ADCģ��״̬��λ
    gADC_HandleStructure ->State = HAL_ADC_STATE_RESET;
    //ADCָ���ӦDMA
    gADC_HandleStructure ->DMA_Handle = gDMA_HandleStructure;
    //ADC״̬�޴���
    gADC_HandleStructure ->ErrorCode = HAL_ADC_ERROR_NONE;
    //ѡ��ADC1����
    gADC_HandleStructure ->Instance = ADC1;
    //ADCģ���ʼ��
    HAL_ADC_Init(gADC_HandleStructure);
    //�����Ƕ�ADC��ͨ�������ú�˳��ָ��
    if(ENABLE == gADC_ChnConfig ->ADC_IN_0_State)
    {
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_0;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
      Chn_Rank++;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_1_State)
    {
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_1;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
      Chn_Rank++;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_2_State)
    {
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_2;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
      Chn_Rank++;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_3_State)
    {
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_3;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
      Chn_Rank++;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_4_State)
    {
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_4;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
      Chn_Rank++;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_5_State)
    {
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_5;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
      Chn_Rank++;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_6_State)
    {
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_6;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
      Chn_Rank++;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_7_State)
    {
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_7;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
      Chn_Rank++;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_8_State)
    {
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_8;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
      Chn_Rank++;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_9_State)
    {
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_9;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
      Chn_Rank++;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_Temp_State)
    {
      //��ȡ30���϶�ADC��׼ֵ
      Temp30_Vref_Adc = *(__IO uint16_t *)VREF30ADDR;
      //��ȡ130���϶�ADC��׼ֵ
      Temp130_Vref_Adc = *(__IO uint16_t *)VREF130ADDR;
      //�¶ȱ仯б�ʼ���
      TempVarySlope = 100.0 / (float)(Temp130_Vref_Adc - Temp30_Vref_Adc);
      
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_TEMPSENSOR;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      //�¶�ͨ������
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
    }
    //ADC�궨��ʼ
    HAL_ADCEx_Calibration_Start(gADC_HandleStructure, ADC_SINGLE_ENDED);
    //��ȡADC�궨ϵ��
    CalibrationFactor = HAL_ADCEx_Calibration_GetValue(gADC_HandleStructure, ADC_SINGLE_ENDED);
    //д��ADC�궨ϵ��
    HAL_ADCEx_Calibration_SetValue(gADC_HandleStructure, ADC_SINGLE_ENDED, CalibrationFactor);
    //DMA�������赽�ڴ�
    gDMA_HandleStructure ->Init.Direction = DMA_PERIPH_TO_MEMORY;
    //DMA���ݰ��ֶ���
    gDMA_HandleStructure ->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    //DMA�������ݰ��ֶ���
    gDMA_HandleStructure ->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    //DMA�ڴ��ַ��������
    gDMA_HandleStructure ->Init.MemInc = DMA_MINC_ENABLE;
    //DMA�����ַ������
    gDMA_HandleStructure ->Init.PeriphInc = DMA_PINC_DISABLE;
    //DMAѭ��ģʽ
    gDMA_HandleStructure ->Init.Mode = DMA_CIRCULAR;
    //DMA���ȼ���
    gDMA_HandleStructure ->Init.Priority = DMA_PRIORITY_HIGH;
    //DMA����0
    gDMA_HandleStructure ->Init.Request = DMA_REQUEST_0;
    //DMA������
    gDMA_HandleStructure ->Lock = HAL_UNLOCKED;
    //DMAͨ��1
    gDMA_HandleStructure ->Instance = DMA1_Channel1;
    //ָ����ӦADC�ṹ��
    gDMA_HandleStructure ->Parent = gADC_HandleStructure;
    //DMAģ���ʼ��
    HAL_DMA_Init(gDMA_HandleStructure);
    //���DMAͨ��1������ɱ�־λ
    __HAL_DMA_CLEAR_FLAG(&DMA_ADC_HandleInitStructure, DMA_FLAG_TC1);
    //��ʼADC DMA����
    HAL_ADC_Start_DMA(gADC_HandleStructure, (uint32_t*)&gADC_GetData[0][0], AD_SEQBUFF_MAX);
  }
}
