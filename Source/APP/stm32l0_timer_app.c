/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        stm32l0_timer_app.c
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-06
** 描述:          定时器应用实例
** 功能:          定时器定时功能
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#include "stm32l0_timer_app.h"

TIM_HandleTypeDef TIM2_HandleInitStruct;                                        //TIM2参数初始化结构体
TIM_HandleTypeDef TIM3_HandleInitStruct;                                        //TIM3参数初始化结构体

//****************************************************************************************************************************************************************
// 名称               : gSimpleDelay()
// 创建日期           : 2017-05-06
// 作者               : 庄明群
// 功能               : 简单延时函数(毫秒级别)
// 输入参数           : gcnt(毫秒计数)
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 大致延时
// 修改内容           :
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
// 名称               : gTim2Config()
// 创建日期           : 2017-05-06
// 作者               : 庄明群
// 功能               : TIM2配置为定时中断模式，100ms定时时间，用于采集Pcap值
// 输入参数           : 无
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 准确定时
// 修改内容           :
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
// 名称               : gTim3_BaudrateRelate()
// 创建日期           : 2017-05-06
// 作者               : 庄明群
// 功能               : TIM3配置为通信定时，与波特率相关，用于通知接收完数据
// 输入参数           : Baudrate(波特率)
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 准确定时
// 修改内容           :
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
