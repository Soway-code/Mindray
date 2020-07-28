/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        stm32l0_adc_app.c
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-15
** 描述:          ADC模块的应用
** 功能:          ADC DMA 功能配置 ADC温度数据获取与处理
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/
#include "stm32l0_adc_app.h"

ADC_HandleTypeDef ADC_HandleInitStructure;
DMA_HandleTypeDef DMA_ADC_HandleInitStructure;

//用户使用的ADC转换结果缓存
uint16_t uADC_GetResult[AD_GROUP_MAX][AD_CHANNEL_MAX];

//****************************************************************************************************************************************************************
// 名称               : ADC_Chn_Config()
// 创建日期           : 2017-05-15
// 作者               : 庄明群
// 功能               : 选择ADC通道并对ADC通道初始化
// 输入参数           : 无
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : ADC通道用户可自定义
// 修改内容           :
//****************************************************************************************************************************************************************

void ADC_Chn_Config(void)
{
  ADC_Chn_ConfigTypeDef gADC_ChnInitStructure;
  //使能通道2
  gADC_ChnInitStructure.ADC_IN_2_State = ENABLE;
  //使能内部温度通道
  gADC_ChnInitStructure.ADC_Temp_State = ENABLE;
  //ADC通道引脚配置
  ADC_GPIO_Config(&gADC_ChnInitStructure);
  //ADC DMA传输方式配置
  ADC_DMA_Init(&ADC_HandleInitStructure, &DMA_ADC_HandleInitStructure, &gADC_ChnInitStructure);
}

//****************************************************************************************************************************************************************
// 名称               : ADC_GetChn_Value()
// 创建日期           : 2017-05-15
// 作者               : 庄明群
// 功能               : 用户获取ADC通道上的数据
// 输入参数           : 无
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : ADC通道和采样组数用户可自定义
// 修改内容           :
//****************************************************************************************************************************************************************

void ADC_GetChn_Value(void)
{
  uint8_t Ad_group;
  uint8_t Ad_Channel;
  static uint16_t DMA_TC_Cnt = 0;
  //DMA传输完成才会处理数据
  if(RESET != __HAL_DMA_GET_FLAG(&DMA_ADC_HandleInitStructure, DMA_FLAG_TC1))
  {
    __HAL_DMA_CLEAR_FLAG(&DMA_ADC_HandleInitStructure, DMA_FLAG_TC1);
    DMA_TC_Cnt++;
    
    if(AD_SEQBUFF_MAX <= DMA_TC_Cnt)
    {
      //获取全部ADC通道的数据
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
// 名称               : ADC_TemperData_Handle()
// 创建日期           : 2017-05-15
// 作者               : 庄明群
// 功能               : ADC温度通道数据处理
// 输入参数           : 无
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 温度计算可参考STM32L072数据手册
// 修改内容           :
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
        //对温度通道的所有数据累加
        AD_Sum += uADC_GetResult[cn][1];
    }
    //计算平均值
    AD_Average = AD_Sum / AD_GROUP_MAX;
    //与校准温度比较计算
    Result = (float)AD_Average / 0.91 - Temp30_Vref_Adc;
    //根据温度变化斜率得到实际温度
    RunVarParam.TemperInAir = Result * TempVarySlope + 30;
    //利用温度K,B值修正温度
    RunVarParam.TemperInAir = RunVarParam.TemperInAir * UserParam.Temper_K1 + UserParam.Temper_B1;
    //该温度在全温度范围内的比率
    Temp_rate = (RunVarParam.TemperInAir + 40.0) / 140.0;
    //最大比率百分百
    if(1.0 < Temp_rate)
    {
        Temp_rate = 1.0;
    }
    //将温度计算出待输出的DA对应的AD值
    RunVarParam.TemperInAirAD = (uint16_t)(Temp_rate * ProductParam.TempDARange) + ProductParam.TempDAMin; 
}
