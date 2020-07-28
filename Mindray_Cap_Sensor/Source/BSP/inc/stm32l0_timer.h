/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        stm32l0_timer.h
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-05
** 描述:          STM32l072 通用定时器配置
** 功能:          STM32L072 通用定时 可中断选择
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#ifndef __STM32L0_TIMER_H
#define __STM32L0_TIMER_H

#include "stm32l0xx_hal_rcc.h"
#include "stm32l0xx_hal_dma.h"
#include "stm32l0xx_hal_tim.h"
#include "stm32l0xx_hal_tim_ex.h"
#include "stm32l0xx_hal_cortex.h"

//TIM设置成功
#define TIM_SET_OK                             0x00
//TIM设置失败
#define TIM_SET_NK                             0x01

//定时器定时中断使能
#define TIM_TIMING_ENABLE                      0x0C
//定时器定时中断禁止
#define TIM_TIMING_DISABLE                     0x00
//定时器时基参数结构体
typedef struct
{
  TIM_TypeDef *TimX;
  uint32_t TimXCntMode;
  uint32_t TimXClkDiv;
  uint32_t TimXPeriod;
  uint32_t TimXPrescaler;
  uint8_t TimXPreemptPrio;
  uint8_t TimXSubPrio;
  uint8_t TimXTimingEn;
}TIM_BaseTypeDef;

//****************************************************************************************************************************************************************
// 名称               : gTimPeriodConfig()
// 创建日期           : 2017-05-05
// 作者               : 庄明群
// 功能               : 通用定时器定时周期配置
// 输入参数           : TIM_BaseTypeDef结构体(时基参数)
// 输出参数           : TIM_HandleTypeDef结构体(定时器处理结构体)
// 返回结果           : 0(成功),1(失败)
// 注意和说明         : 可中断选择
// 修改内容           :
//****************************************************************************************************************************************************************

uint8_t gTimPeriodConfig(TIM_HandleTypeDef *gTim_InitStructure, TIM_BaseTypeDef *gTim_BaseInitStructure);

#endif