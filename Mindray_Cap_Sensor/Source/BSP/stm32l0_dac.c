/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        stm32l0_dac.c
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-12
** 描述:          
** 功能:          STM32L072 DAC多通道配置 DAC转换
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#include "stm32l0_dac.h"

//***************************************************************************************************************************************************************
// 名称               : DAC_Chn_Init()
// 创建日期           : 2017-05-12
// 作者        	      : 庄明群
// 功能               : 多通道DAC初始化
// 输入参数           : DAC_Chn_ConfigTypeDef结构体(多通道DAC使能状态)
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : DAC引脚使能状态只有两种(ENABLE和DISABLE)
// 修改内容           :
//***************************************************************************************************************************************************************

void DAC_Chn_Init(DAC_Chn_ConfigTypeDef * gDAC_Chn_Config)
{
  uint16_t sGPIO_Pin;
  //GPIO初始化结构体
  GPIO_InitTypeDef GPIO_InitStructure; 
  //DAC 参数处理结构体
  DAC_HandleTypeDef DAC_HandleStructure;   
  //DAC 通道参数结构体
  DAC_ChannelConfTypeDef DAC_ChannelInitStructure;                              
  //是否有DAC通道选择
  if((ENABLE == gDAC_Chn_Config ->DAC_OUT0_State) || (ENABLE == gDAC_Chn_Config ->DAC_OUT1_State))
  {
    //使能DAC对应引脚GPIOA时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();   
    //使能DAC时钟
    __HAL_RCC_DAC_CLK_ENABLE();                                                 
    
    sGPIO_Pin = 0;
    //DAC通道1选择使能
    if(ENABLE == gDAC_Chn_Config ->DAC_OUT0_State)                              
    {
      sGPIO_Pin |= GPIO_PIN_4;                                                  
    }
    //DAC通道2选择使能
    if(ENABLE == gDAC_Chn_Config ->DAC_OUT1_State)                              
    {
      sGPIO_Pin |= GPIO_PIN_5;                                                  
    }
    //选择DAC 通道2输出引脚
    GPIO_InitStructure.Pin = sGPIO_Pin; 
    //输出模式为模拟输出
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;  
    //无上下拉
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    //速度高速
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH; 
    //初始化DAC GPIO
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);                                  
    //选择DAC外设
    DAC_HandleStructure.Instance = DAC; 
    //初始化DAC外设
    HAL_DAC_Init(&DAC_HandleStructure);                                         
    //DAC软件触发方式
    DAC_ChannelInitStructure.DAC_Trigger = DAC_TRIGGER_SOFTWARE;
    //DAC输出缓冲使能
    DAC_ChannelInitStructure.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;        
    
    if(ENABLE == gDAC_Chn_Config ->DAC_OUT0_State)                                                     
    {
      //DAC通道1输出使能
      HAL_DAC_ConfigChannel(&DAC_HandleStructure, &DAC_ChannelInitStructure, DAC_CHANNEL_1);
    }
    if(ENABLE == gDAC_Chn_Config ->DAC_OUT1_State)
    {
      //DAC通道2输出使能
      HAL_DAC_ConfigChannel(&DAC_HandleStructure, &DAC_ChannelInitStructure, DAC_CHANNEL_2);
    }
  }
}

//***************************************************************************************************************************************************************
// 名称               : DAC_Chn_SetValue()
// 创建日期           : 2017-05-12
// 作者        	      : 庄明群
// 功能               : DAC通道设置输出值
// 输入参数           : ChnValue(DA值),Chn(通道号)
// 输出参数           : 无
// 返回结果           : OP_SUCCESS(成功),OP_FAILED(失败)
// 注意和说明         : 输出值不能超过4095,通道号只有两种(1和2)
// 修改内容           :
//***************************************************************************************************************************************************************

uint8_t DAC_Chn_SetValue(uint16_t ChnValue, uint8_t Chn)
{
  __IO uint32_t tmp;
  uint32_t tmp1 = 0, tmp2 = 0;
  DAC_HandleTypeDef DAC_HandleStructure;
  //DAC值超出范围
  if(4095 < ChnValue)
  {
    return OP_FAILED;
  }
  //无效通道返回错误
  if((1 != Chn) && (2 != Chn))
  {
    return OP_FAILED;
  }
  //选择DAC外设
  DAC_HandleStructure.Instance = DAC;    
  //DAC准备好
  DAC_HandleStructure.State = HAL_DAC_STATE_READY; 
  
  tmp = (uint32_t)DAC;                                                          
  if(1 == Chn)
  {
    //通道1输出12位右对齐
    tmp += __DAC_DHR12R1_ALIGNEMENT(DAC_ALIGN_12B_R);                           
  }
  if(2 == Chn)
  {
    //通道2输出12位右对齐
    tmp += __DAC_DHR12R2_ALIGNEMENT(DAC_ALIGN_12B_R);
  }
  //写入DAC数据寄存器数值
  *(__IO uint32_t *) tmp = ChnValue;                                            
  
  if(1 == Chn)
  {
    //DAC 通道1触发使能
    tmp1 = DAC_HandleStructure.Instance->CR & DAC_CR_TEN1;  
    //DAC 选择通道1触发
    tmp2 = DAC_HandleStructure.Instance->CR & DAC_CR_TSEL1;                     
    if((tmp1 ==  DAC_CR_TEN1) && (tmp2 ==  DAC_CR_TSEL1))
    {
      //软件触发方式
      SET_BIT(DAC_HandleStructure.Instance->SWTRIGR, DAC_SWTRIGR_SWTRIG1);      
    }
    //打开DAC通道1输出
    __HAL_DAC_ENABLE(&DAC_HandleStructure, DAC_CHANNEL_1);                      
  }
  if(2 == Chn)
  {
    //DAC 通道2触发使能
    tmp1 = DAC_HandleStructure.Instance->CR & DAC_CR_TEN2; 
    //DAC 选择通道2触发
    tmp2 = DAC_HandleStructure.Instance->CR & DAC_CR_TSEL2;                     
    if((tmp1 ==  DAC_CR_TEN2) && (tmp2 ==  DAC_CR_TSEL2))
    {
      //软件触发方式
      SET_BIT(DAC_HandleStructure.Instance->SWTRIGR, DAC_SWTRIGR_SWTRIG2);      
    }
    //打开DAC通道2输出
    __HAL_DAC_ENABLE(&DAC_HandleStructure, DAC_CHANNEL_2);                      
  }
  
  return OP_SUCCESS;
}
