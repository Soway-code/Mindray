/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        stm32l0_adc.h
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

#ifndef __STM32L0_ADC_H
#define __STM32L0_ADC_H

#include "stm32l0xx_hal_rcc.h"
#include "stm32l0xx_hal_rcc_ex.h"
#include "stm32l0xx_hal_gpio.h"
#include "stm32l0xx_hal_gpio_ex.h"
#include "stm32l0xx_hal_dma.h"
#include "stm32l0xx_hal_adc.h"
#include "stm32l0xx_hal_adc_ex.h"
#include "stm32l0xx_hal_cortex.h"
#include "VariaType.h"

#define AD_GROUP_MAX      10                                      //每个通道采集数量，用户可以自定义
#define AD_CHANNEL_MAX    2                                       //使能ADC通道数，用户可以自定义
#define AD_SEQBUFF_MAX   (AD_GROUP_MAX * AD_CHANNEL_MAX)          //ADC序列缓存大小

#define VREF30ADDR                      0x1FF8007A                //30摄氏度ADC参考值地址
#define VREF130ADDR                     0x1FF8007E                //130摄氏度ADC参考值地址

//以下的宏在stm32l0xx.h中定义
//typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
//FunctionalState 只有两种可能 使能和禁止

//ADC通道配置结构体
typedef struct
{
  FunctionalState ADC_IN_0_State;                                 //ADC通道0 打开或禁止
  FunctionalState ADC_IN_1_State;                                 //ADC通道1 打开或禁止
  FunctionalState ADC_IN_2_State;                                 //ADC通道2 打开或禁止
  FunctionalState ADC_IN_3_State;                                 //ADC通道3 打开或禁止
  FunctionalState ADC_IN_4_State;                                 //ADC通道4 打开或禁止
  FunctionalState ADC_IN_5_State;                                 //ADC通道5 打开或禁止
  FunctionalState ADC_IN_6_State;                                 //ADC通道6 打开或禁止
  FunctionalState ADC_IN_7_State;                                 //ADC通道7 打开或禁止
  FunctionalState ADC_IN_8_State;                                 //ADC通道8 打开或禁止
  FunctionalState ADC_IN_9_State;                                 //ADC通道9 打开或禁止
  FunctionalState ADC_Temp_State;                                 //温度通道 打开或禁止
}ADC_Chn_ConfigTypeDef;

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

void ADC_GPIO_Config(ADC_Chn_ConfigTypeDef *gADC_ChnConfig);

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
void ADC_DMA_Init(ADC_HandleTypeDef *gADC_HandleStructure, DMA_HandleTypeDef *gDMA_HandleStructure, ADC_Chn_ConfigTypeDef *gADC_ChnConfig);

extern float TempVarySlope;                                                   //温度变化斜率
extern uint16_t Temp30_Vref_Adc;                                               //30摄氏度ADC基准值
extern uint16_t Temp130_Vref_Adc;                                              //130摄氏度ADC基准值
extern volatile uint16_t gADC_GetData[AD_GROUP_MAX][AD_CHANNEL_MAX];          //供多通道ADC DMA使用的缓存

#endif