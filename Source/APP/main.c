#include "modbus.h"

uint8_t  cnt = 0;   //复位次数计数
uint8_t temp;
int main(void)
{
    //SysTick_Config(SystemCoreClock / 1000);
     Delay_us(1000);

    HAL_Init();
   
    cnt = Eeprom_Read_OneByte(RESET_CNT);
    if(cnt >=4) cnt = 0;
    Read_Device_Param();                                                        //读取设备参数
    gUart1_Config();                                                            //串口1初始化
    PCap_Init();                                                                //PCap芯片初始化
    gTim2Config();                                                              //定时器2配置
    ADC_Chn_Config();                                                           //ADC多通道配置
    DAC_Chn_Selection();                                                        //DAC通道选择配置
    User_Iwdg_Init();                                                         //初始化看门狗
     

      //EepInitFlg = Eeprom_Read_OneByte(RUN_ADDR_BASE);
    while(1)
    {
     User_Iwdg_Feed();                                                       //喂看门狗                                                         
      
      //Modbus通信处理
        if(UsartRecvOk == USART_RECV_TIMER_OK)
        {
            MBASCII_Function();
            UsartRecvOk = 0;                                                    //发送完接收标志置0            
        }
        else
        {
            SensorEvent();                                                      //传感器事件自动上报
        }
        GetPCapResult();                                                        //获取PCap采集数据
        DataFilterAndOutput();                                                  //PCap数据滤波和输出
        ADC_GetChn_Value();                                                     //ADC通道数据获取
        DAC_Chn_Handle();                                                       //DAC通道数据处理
    }
}

//**************************************************************************************************
// 名称         	: SystemClockConfiguration()
// 创建日期     	: 2017-04-26
// 作者        	 	: 庄明群
// 功能         	: 系统时钟配置
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void SystemClockConfiguration(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_CFGR_SW_HSI;//RCC_SYSCLKSOURCE_PLLCLK;
  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
  //if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  //{
  //  Error_Handler();
  //}
}