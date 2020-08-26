/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        stm32l0_dac.c
** ����:          ׯ��Ⱥ
** �汾:          V1.0.0
** ����:          2017-05-12
** ����:          
** ����:          STM32L072 DAC��ͨ������ DACת��
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#include "stm32l0_dac.h"

//***************************************************************************************************************************************************************
// ����               : DAC_Chn_Init()
// ��������           : 2017-05-12
// ����        	      : ׯ��Ⱥ
// ����               : ��ͨ��DAC��ʼ��
// �������           : DAC_Chn_ConfigTypeDef�ṹ��(��ͨ��DACʹ��״̬)
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : DAC����ʹ��״ֻ̬������(ENABLE��DISABLE)
// �޸�����           :
//***************************************************************************************************************************************************************

void DAC_Chn_Init(DAC_Chn_ConfigTypeDef * gDAC_Chn_Config)
{
  uint16_t sGPIO_Pin;
  //GPIO��ʼ���ṹ��
  GPIO_InitTypeDef GPIO_InitStructure; 
  //DAC ��������ṹ��
  DAC_HandleTypeDef DAC_HandleStructure;   
  //DAC ͨ�������ṹ��
  DAC_ChannelConfTypeDef DAC_ChannelInitStructure;                              
  //�Ƿ���DACͨ��ѡ��
  if((ENABLE == gDAC_Chn_Config ->DAC_OUT0_State) || (ENABLE == gDAC_Chn_Config ->DAC_OUT1_State))
  {
    //ʹ��DAC��Ӧ����GPIOAʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();   
    //ʹ��DACʱ��
    __HAL_RCC_DAC_CLK_ENABLE();                                                 
    
    sGPIO_Pin = 0;
    //DACͨ��1ѡ��ʹ��
    if(ENABLE == gDAC_Chn_Config ->DAC_OUT0_State)                              
    {
      sGPIO_Pin |= GPIO_PIN_4;                                                  
    }
    //DACͨ��2ѡ��ʹ��
    if(ENABLE == gDAC_Chn_Config ->DAC_OUT1_State)                              
    {
      sGPIO_Pin |= GPIO_PIN_5;                                                  
    }
    //ѡ��DAC ͨ��2�������
    GPIO_InitStructure.Pin = sGPIO_Pin; 
    //���ģʽΪģ�����
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;  
    //��������
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    //�ٶȸ���
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH; 
    //��ʼ��DAC GPIO
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);                                  
    //ѡ��DAC����
    DAC_HandleStructure.Instance = DAC; 
    //��ʼ��DAC����
    HAL_DAC_Init(&DAC_HandleStructure);                                         
    //DAC���������ʽ
    DAC_ChannelInitStructure.DAC_Trigger = DAC_TRIGGER_SOFTWARE;
    //DAC�������ʹ��
    DAC_ChannelInitStructure.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;        
    
    if(ENABLE == gDAC_Chn_Config ->DAC_OUT0_State)                                                     
    {
      //DACͨ��1���ʹ��
      HAL_DAC_ConfigChannel(&DAC_HandleStructure, &DAC_ChannelInitStructure, DAC_CHANNEL_1);
    }
    if(ENABLE == gDAC_Chn_Config ->DAC_OUT1_State)
    {
      //DACͨ��2���ʹ��
      HAL_DAC_ConfigChannel(&DAC_HandleStructure, &DAC_ChannelInitStructure, DAC_CHANNEL_2);
    }
  }
}

//***************************************************************************************************************************************************************
// ����               : DAC_Chn_SetValue()
// ��������           : 2017-05-12
// ����        	      : ׯ��Ⱥ
// ����               : DACͨ���������ֵ
// �������           : ChnValue(DAֵ),Chn(ͨ����)
// �������           : ��
// ���ؽ��           : OP_SUCCESS(�ɹ�),OP_FAILED(ʧ��)
// ע���˵��         : ���ֵ���ܳ���4095,ͨ����ֻ������(1��2)
// �޸�����           :
//***************************************************************************************************************************************************************

uint8_t DAC_Chn_SetValue(uint16_t ChnValue, uint8_t Chn)
{
  __IO uint32_t tmp;
  uint32_t tmp1 = 0, tmp2 = 0;
  DAC_HandleTypeDef DAC_HandleStructure;
  //DACֵ������Χ
  if(4095 < ChnValue)
  {
    return OP_FAILED;
  }
  //��Чͨ�����ش���
  if((1 != Chn) && (2 != Chn))
  {
    return OP_FAILED;
  }
  //ѡ��DAC����
  DAC_HandleStructure.Instance = DAC;    
  //DAC׼����
  DAC_HandleStructure.State = HAL_DAC_STATE_READY; 
  
  tmp = (uint32_t)DAC;                                                          
  if(1 == Chn)
  {
    //ͨ��1���12λ�Ҷ���
    tmp += __DAC_DHR12R1_ALIGNEMENT(DAC_ALIGN_12B_R);                           
  }
  if(2 == Chn)
  {
    //ͨ��2���12λ�Ҷ���
    tmp += __DAC_DHR12R2_ALIGNEMENT(DAC_ALIGN_12B_R);
  }
  //д��DAC���ݼĴ�����ֵ
  *(__IO uint32_t *) tmp = ChnValue;                                            
  
  if(1 == Chn)
  {
    //DAC ͨ��1����ʹ��
    tmp1 = DAC_HandleStructure.Instance->CR & DAC_CR_TEN1;  
    //DAC ѡ��ͨ��1����
    tmp2 = DAC_HandleStructure.Instance->CR & DAC_CR_TSEL1;                     
    if((tmp1 ==  DAC_CR_TEN1) && (tmp2 ==  DAC_CR_TSEL1))
    {
      //���������ʽ
      SET_BIT(DAC_HandleStructure.Instance->SWTRIGR, DAC_SWTRIGR_SWTRIG1);      
    }
    //��DACͨ��1���
    __HAL_DAC_ENABLE(&DAC_HandleStructure, DAC_CHANNEL_1);                      
  }
  if(2 == Chn)
  {
    //DAC ͨ��2����ʹ��
    tmp1 = DAC_HandleStructure.Instance->CR & DAC_CR_TEN2; 
    //DAC ѡ��ͨ��2����
    tmp2 = DAC_HandleStructure.Instance->CR & DAC_CR_TSEL2;                     
    if((tmp1 ==  DAC_CR_TEN2) && (tmp2 ==  DAC_CR_TSEL2))
    {
      //���������ʽ
      SET_BIT(DAC_HandleStructure.Instance->SWTRIGR, DAC_SWTRIGR_SWTRIG2);      
    }
    //��DACͨ��2���
    __HAL_DAC_ENABLE(&DAC_HandleStructure, DAC_CHANNEL_2);                      
  }
  
  return OP_SUCCESS;
}
