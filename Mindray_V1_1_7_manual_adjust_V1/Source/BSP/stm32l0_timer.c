/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        stm32l0_timer.c
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

#include "stm32l0_timer.h"

//****************************************************************************************************************************************************************
// 名称               : gTimPeriodConfig()
// 创建日期           : 2017-05-05
// 作者               : 庄明群
// 功能               : 通用定时器定时周期配置
// 输入参数           : TIM_BaseTypeDef结构体(时基参数)
// 输出参数           : TIM_HandleTypeDef结构体(定时器处理结构体)
// 返回结果           : TIM_SET_OK(成功),TIM_SET_NK(失败)
// 注意和说明         : 可中断选择
// 修改内容           :
//****************************************************************************************************************************************************************

uint8_t gTimPeriodConfig(TIM_HandleTypeDef *gTim_InitStructure, TIM_BaseTypeDef *gTim_BaseInitStructure)
{
  IRQn_Type TIM_IRQn;
  
  HAL_StatusTypeDef HAL_Status;
  //地址无效，返回错误
  if((0 == gTim_InitStructure) || (0 == gTim_BaseInitStructure))
  {
    return TIM_SET_NK;
  }
  //通用定时器选择
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
    //无效选择，返回错误
    return TIM_SET_NK;
  }
  //计数模式
  gTim_InitStructure ->Init.CounterMode          = gTim_BaseInitStructure ->TimXCntMode;
  //时钟分频
  gTim_InitStructure ->Init.ClockDivision        = gTim_BaseInitStructure ->TimXClkDiv;
  //计数个数周期
  gTim_InitStructure ->Init.Period               = gTim_BaseInitStructure ->TimXPeriod;
  //定时器分频
  gTim_InitStructure ->Init.Prescaler            = gTim_BaseInitStructure ->TimXPrescaler;
  //选择定时器
  gTim_InitStructure ->Instance                  = gTim_BaseInitStructure ->TimX;
  //定时器时基初始化
  HAL_Status = HAL_TIM_Base_Init(gTim_InitStructure);
  //初始化失败则返回错误
  if(HAL_OK != HAL_Status)
  {
    return TIM_SET_NK;
  }
  //定时器计数清零
  gTim_InitStructure ->Instance ->CNT = 0;
  //是否使能定时中断
  if(TIM_TIMING_ENABLE == gTim_BaseInitStructure ->TimXTimingEn)
  {
    //清零中断标志
    __HAL_TIM_CLEAR_IT(gTim_InitStructure, TIM_IT_UPDATE);
    //设置中断优先级
    HAL_NVIC_SetPriority(TIM_IRQn, gTim_BaseInitStructure ->TimXPreemptPrio, gTim_BaseInitStructure ->TimXSubPrio);
    //使能定时器中断  
    HAL_NVIC_EnableIRQ(TIM_IRQn);
    //先关闭中断
    HAL_Status = HAL_TIM_Base_Stop_IT(gTim_InitStructure);
    //关闭中断失败返回错误
    if(HAL_OK != HAL_Status)
    {
      return TIM_SET_NK;
    }
  }
  else
  {
    //如果禁止定时中断
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
