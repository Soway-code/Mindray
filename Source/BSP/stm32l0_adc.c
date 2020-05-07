/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        stm32l0_adc.c
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-09
** 描述:          STM32l072 ADC多通道配置 DMA传输配置 内部温度传感器配置
** 功能:          STM32L072 ADC模块多通道DMA驱动 内部温度ADC转换
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#include "stm32l0_adc.h"

float TempVarySlope;                                                           //温度变化斜率
uint16_t Temp30_Vref_Adc;                                                       //30摄氏度ADC基准值
uint16_t Temp130_Vref_Adc;                                                      //130摄氏度ADC基准值
volatile uint16_t gADC_GetData[AD_GROUP_MAX][AD_CHANNEL_MAX];                  //供多通道ADC DMA使用的内存

//***************************************************************************************************************************************************************
// 名称               : ADC_GPIO_Config()
// 创建日期           : 2017-05-09
// 作者        	      : 庄明群
// 功能               : 多通道ADC模块 模拟输入引脚配置
// 输入参数           : ADC_Chn_ConfigTypeDef结构体(多通道ADC引脚使能状态)
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : ADC引脚使能状态只有两种(ENABLE和DISABLE)
// 修改内容           :
//***************************************************************************************************************************************************************

void ADC_GPIO_Config(ADC_Chn_ConfigTypeDef *gADC_ChnConfig)
{
  uint16_t sGPIO_Pin;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //GPIOA端口任何一ADC通道模拟输入使能
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
    
    //GPIOA上的模拟输入引脚选择
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
    //引脚配置
    GPIO_InitStructure.Pin = sGPIO_Pin;
    //模拟输入模式
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    //无上下拉
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    //高速输入
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    //初始化GPIO引脚参数
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
  }
  //ADC的8,9通道
  if((ENABLE == gADC_ChnConfig ->ADC_IN_8_State)
     ||(ENABLE == gADC_ChnConfig ->ADC_IN_9_State)
    )
  {
    sGPIO_Pin = 0;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    //模拟输入引脚选择
    if(ENABLE == gADC_ChnConfig ->ADC_IN_8_State)
    {
      sGPIO_Pin |= GPIO_PIN_0;
    }
    if(ENABLE == gADC_ChnConfig ->ADC_IN_9_State)
    {
      sGPIO_Pin |= GPIO_PIN_1;
    }
    //引脚配置
    GPIO_InitStructure.Pin = sGPIO_Pin;
    //模拟输入模式
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    //无上下拉
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    //高速输入
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    //初始化引脚参数
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  }
  //使能内部温度传感器
  if(ENABLE == gADC_ChnConfig ->ADC_Temp_State)
  {
    HAL_ADCEx_EnableVREFINTTempSensor();
  }
}

//***************************************************************************************************************************************************************
// 名称               : ADC_DMA_Init()
// 创建日期           : 2017-05-09
// 作者        	      : 庄明群
// 功能               : 多通道ADC模块DMA方式初始化
// 输入参数           : ADC_Chn_ConfigTypeDef(ADC通道使能参数)
// 输出参数           : ADC_HandleTypeDef(ADC模块处理),DMA_HandleTypeDef(DMA模块处理)
// 返回结果           : 无
// 注意和说明         : ADC引脚使能状态只有两种(ENABLE和DISABLE),ADC_HandleTypeDef输出参数需先定义好全局结构体变量
// 修改内容           :
//***************************************************************************************************************************************************************
void ADC_DMA_Init(ADC_HandleTypeDef *gADC_HandleStructure, DMA_HandleTypeDef *gDMA_HandleStructure, ADC_Chn_ConfigTypeDef *gADC_ChnConfig)
{
  //ADC通道序列
  uint8_t Chn_Rank;
  //ADC标定系数
  uint32_t CalibrationFactor;
  //ADC通道配置结构体
  ADC_ChannelConfTypeDef ADC_ChannelConfStucture;
  //ADC任何一通道使能
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
    //打开ADC时钟
    __HAL_RCC_ADC1_CLK_ENABLE();
    //打开DMA时钟
    __HAL_RCC_DMA1_CLK_ENABLE();
    //通道序列先清零
    Chn_Rank = 0;
    //ADC时钟同步PCLK 4分频
    gADC_HandleStructure ->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    //连续转换模式
    gADC_HandleStructure ->Init.ContinuousConvMode = ENABLE;
    //ADC数据右对齐
    gADC_HandleStructure ->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //通道全序列转换
    gADC_HandleStructure ->Init.DiscontinuousConvMode = ENABLE;
    //DMA连续请求使能
    gADC_HandleStructure ->Init.DMAContinuousRequests = ENABLE;
    //ADC序列转换结束标志
    gADC_HandleStructure ->Init.EOCSelection = ADC_EOC_SEQ_CONV;
    //ADC转换软件触发
    gADC_HandleStructure ->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    //无边沿触发
    gADC_HandleStructure ->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //过采样数据覆盖
    gADC_HandleStructure ->Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    //ADC 12bit数据分辨率
    gADC_HandleStructure ->Init.Resolution = ADC_RESOLUTION_12B;
    //ADC采样周期
    gADC_HandleStructure ->Init.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
    //ADC采样通道从小到大扫描
    gADC_HandleStructure ->Init.ScanConvMode = ADC_SCAN_ENABLE;
    //ADC模块不锁定
    gADC_HandleStructure ->Lock = HAL_UNLOCKED;
    //ADC模块状态复位
    gADC_HandleStructure ->State = HAL_ADC_STATE_RESET;
    //ADC指向对应DMA
    gADC_HandleStructure ->DMA_Handle = gDMA_HandleStructure;
    //ADC状态无错误
    gADC_HandleStructure ->ErrorCode = HAL_ADC_ERROR_NONE;
    //选择ADC1外设
    gADC_HandleStructure ->Instance = ADC1;
    //ADC模块初始化
    HAL_ADC_Init(gADC_HandleStructure);
    //以下是对ADC多通道的配置和顺序指定
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
      //读取30摄氏度ADC基准值
      Temp30_Vref_Adc = *(__IO uint16_t *)VREF30ADDR;
      //读取130摄氏度ADC基准值
      Temp130_Vref_Adc = *(__IO uint16_t *)VREF130ADDR;
      //温度变化斜率计算
      TempVarySlope = 100.0 / (float)(Temp130_Vref_Adc - Temp30_Vref_Adc);
      
      ADC_ChannelConfStucture.Channel = ADC_CHANNEL_TEMPSENSOR;
      ADC_ChannelConfStucture.Rank = Chn_Rank;
      //温度通道配置
      HAL_ADC_ConfigChannel(gADC_HandleStructure, &ADC_ChannelConfStucture);
    }
    //ADC标定开始
    HAL_ADCEx_Calibration_Start(gADC_HandleStructure, ADC_SINGLE_ENDED);
    //获取ADC标定系数
    CalibrationFactor = HAL_ADCEx_Calibration_GetValue(gADC_HandleStructure, ADC_SINGLE_ENDED);
    //写入ADC标定系数
    HAL_ADCEx_Calibration_SetValue(gADC_HandleStructure, ADC_SINGLE_ENDED, CalibrationFactor);
    //DMA方向外设到内存
    gDMA_HandleStructure ->Init.Direction = DMA_PERIPH_TO_MEMORY;
    //DMA数据半字对齐
    gDMA_HandleStructure ->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    //DMA外设数据半字对齐
    gDMA_HandleStructure ->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    //DMA内存地址递增允许
    gDMA_HandleStructure ->Init.MemInc = DMA_MINC_ENABLE;
    //DMA外设地址不递增
    gDMA_HandleStructure ->Init.PeriphInc = DMA_PINC_DISABLE;
    //DMA循环模式
    gDMA_HandleStructure ->Init.Mode = DMA_CIRCULAR;
    //DMA优先级高
    gDMA_HandleStructure ->Init.Priority = DMA_PRIORITY_HIGH;
    //DMA请求0
    gDMA_HandleStructure ->Init.Request = DMA_REQUEST_0;
    //DMA不锁定
    gDMA_HandleStructure ->Lock = HAL_UNLOCKED;
    //DMA通道1
    gDMA_HandleStructure ->Instance = DMA1_Channel1;
    //指定对应ADC结构体
    gDMA_HandleStructure ->Parent = gADC_HandleStructure;
    //DMA模块初始化
    HAL_DMA_Init(gDMA_HandleStructure);
    //清除DMA通道1传输完成标志位
    __HAL_DMA_CLEAR_FLAG(&DMA_ADC_HandleInitStructure, DMA_FLAG_TC1);
    //开始ADC DMA传输
    HAL_ADC_Start_DMA(gADC_HandleStructure, (uint32_t*)&gADC_GetData[0][0], AD_SEQBUFF_MAX);
  }
}
