/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        stm32l0_usart.c
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-02
** 描述:          STM32l072 通用串口配置
** 功能:          STM32L072 通用串口 RS485使能选择
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#include "stm32l0_usart.h"

//****************************************************************************************************************************************************************
// 名称               : gUart_Rs485Gpio_Init()
// 创建日期           : 2017-05-02
// 作者               : 庄明群
// 功能               : RS485使能引脚配置
// 输入参数           : GPIO_TypeDef(端口),Gpio_Pin(引脚)
// 输出参数           : 无
// 返回结果           : OP_SUCCESS(成功),OP_FAILED(失败)
// 注意和说明         : 
// 修改内容           :
//****************************************************************************************************************************************************************

uint8_t gUart_Rs485Gpio_Init(GPIO_TypeDef *GPIOx, uint16_t Gpio_Pin)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  if((GPIOA != GPIOx) && (GPIOB != GPIOx))
  {
    return OP_FAILED;
  }
  
  if(GPIOA == GPIOx)
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();
  }
  else
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
  }
  
  GPIO_InitStructure.Pin = Gpio_Pin;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);
  HAL_GPIO_WritePin(GPIOx, Gpio_Pin, GPIO_PIN_RESET);//初始化为接收状态
  
  return OP_SUCCESS;
}

//****************************************************************************************************************************************************************
// 名称               : gUart_Gpio_Config()
// 创建日期           : 2017-05-02
// 作者               : 庄明群
// 功能               : 通用串口收发引脚配置
// 输入参数           : UARTx_PortTypeDef(串口端口引脚参数)
// 输出参数           : 无
// 返回结果           : OP_SUCCESS(成功),OP_FAILED(失败)
// 注意和说明         : 支持通用串口重映射
// 修改内容           :
//****************************************************************************************************************************************************************

uint8_t gUart_Gpio_Config(UARTx_PortTypeDef *UART_PortInitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  if(USART1 == UART_PortInitStruct ->UARTx)
  {
    if((GPIOA != UART_PortInitStruct ->GPIOx) && (GPIOB != UART_PortInitStruct ->GPIOx))
    {
      return OP_FAILED;
    }
    else
    {
      if(GPIOA == UART_PortInitStruct ->GPIOx)
      {
        if((GPIO_PIN_9 == UART_PortInitStruct ->UARTx_TX) && (GPIO_PIN_10 == UART_PortInitStruct ->UARTx_RX))
        {
          __HAL_RCC_GPIOA_CLK_ENABLE();
          GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
          GPIO_InitStructure.Pull = GPIO_PULLUP;
          GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
          GPIO_InitStructure.Pin = GPIO_PIN_9 | GPIO_PIN_10;
          GPIO_InitStructure.Alternate = GPIO_AF4_USART1;
          HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
        }
        else
        {
          return OP_FAILED;
        }
      }
      else
      {
        if((GPIO_PIN_6 == UART_PortInitStruct ->UARTx_TX) && (GPIO_PIN_7 == UART_PortInitStruct ->UARTx_RX))
        {
          __HAL_RCC_GPIOB_CLK_ENABLE();
          GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
          GPIO_InitStructure.Pull = GPIO_PULLUP;
          GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
          GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
          GPIO_InitStructure.Alternate = GPIO_AF0_USART1;
          HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
        }
        else
        {
          return OP_FAILED;
        }
      }
    }
  }
  else if(USART2 == UART_PortInitStruct ->UARTx)
  {
    if(GPIOA == UART_PortInitStruct ->GPIOx)
    {
      if(((GPIO_PIN_2 == UART_PortInitStruct ->UARTx_TX) && (GPIO_PIN_3 == UART_PortInitStruct ->UARTx_RX))
         ||((GPIO_PIN_14 == UART_PortInitStruct ->UARTx_TX) && (GPIO_PIN_15 == UART_PortInitStruct ->UARTx_RX))
         )
      {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStructure.Pull = GPIO_PULLUP;
        GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStructure.Alternate = GPIO_AF4_USART2;
        GPIO_InitStructure.Pin = UART_PortInitStruct ->UARTx_TX | UART_PortInitStruct ->UARTx_RX;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
      }
      else
      {
        return OP_FAILED;
      }
    }
    else
    {
      return OP_FAILED;
    }
  }
  else if(USART4 == UART_PortInitStruct ->UARTx)
  {
    if(GPIOA == UART_PortInitStruct ->GPIOx)
    {
      if((GPIO_PIN_0 == UART_PortInitStruct ->UARTx_TX) && (GPIO_PIN_1 == UART_PortInitStruct ->UARTx_RX))
      {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStructure.Pull = GPIO_PULLUP;
        GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStructure.Alternate = GPIO_AF6_USART4;
        GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
      }
      else
      {
        return OP_FAILED;
      }
    }
    else
    {
      return OP_FAILED;
    }
  }
  else if(USART5 == UART_PortInitStruct ->UARTx)
  {
    if(GPIOB == UART_PortInitStruct ->GPIOx)
    {
      if((GPIO_PIN_3 == UART_PortInitStruct ->UARTx_TX) && (GPIO_PIN_4 == UART_PortInitStruct ->UARTx_RX))
      {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStructure.Pull = GPIO_PULLUP;
        GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_4;
        GPIO_InitStructure.Alternate = GPIO_AF6_USART5;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
      }
      else
      {
        return OP_FAILED;
      }
    }
    else
    {
      return OP_FAILED;
    }
  }
  else
  {
    return OP_FAILED;
  }
  
  return OP_SUCCESS;
}

//****************************************************************************************************************************************************************
// 名称               : gUart_Param_Init()
// 创建日期           : 2017-05-02
// 作者               : 庄明群
// 功能               : 通用串口参数初始化
// 输入参数           : UARTx_ConfigTypeDef(串口通信参数)
// 输出参数           : UART_HandleTypeDef(串口处理结构体)
// 返回结果           : OP_SUCCESS(成功),OP_FAILED(失败)
// 注意和说明         : 可选择中断
// 修改内容           :
//****************************************************************************************************************************************************************

uint8_t gUart_Param_Init(UART_HandleTypeDef *UART_HandleStruct, UARTx_ConfigTypeDef *UART_ConfigStruct)
{
  if((USART1 != UART_ConfigStruct ->UARTx) 
      &&(USART2 != UART_ConfigStruct ->UARTx)
      &&(USART4 != UART_ConfigStruct ->UARTx)
      &&(USART5 != UART_ConfigStruct ->UARTx)
     )
  {
    return OP_FAILED;
  }
  
  if((UART_PARITY_NONE != UART_ConfigStruct ->UARTx_Parity)
      &&(UART_PARITY_ODD != UART_ConfigStruct ->UARTx_Parity)
      &&(UART_PARITY_EVEN != UART_ConfigStruct ->UARTx_Parity)
     )
  {
    return OP_FAILED;
  }
  
  if(USART1 == UART_ConfigStruct ->UARTx)
  {
    __HAL_RCC_USART1_CLK_ENABLE();
    UART_HandleStruct ->Instance = USART1;
    UART_HandleStruct ->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UART_HandleStruct ->Init.BaudRate = UART_ConfigStruct ->UARTx_Baudrate;
    UART_HandleStruct ->Init.Parity = UART_ConfigStruct ->UARTx_Parity;
    
    if(UART_PARITY_NONE == UART_ConfigStruct ->UARTx_Parity)
    {
      UART_HandleStruct ->Init.WordLength = UART_WORDLENGTH_8B;
    }
    else if((UART_PARITY_ODD == UART_ConfigStruct ->UARTx_Parity)
             || (UART_PARITY_EVEN == UART_ConfigStruct ->UARTx_Parity)
            )
    {
      UART_HandleStruct ->Init.WordLength = UART_WORDLENGTH_9B;
    }
  }
  UART_HandleStruct ->Init.Mode = UART_MODE_RX | UART_MODE_TX;
  UART_HandleStruct ->Init.StopBits = UART_STOPBITS_1;
  HAL_UART_Init(UART_HandleStruct);
  
  if(ENABLE == UART_ConfigStruct ->UARTx_IntEn)
  {
    if(USART1 == UART_ConfigStruct ->UARTx)
    {
      HAL_NVIC_SetPriority(USART1_IRQn, UART_ConfigStruct ->UARTx_PreemptPrio, UART_ConfigStruct ->UARTx_SubPrio);
      HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
    else if(USART2 == UART_ConfigStruct ->UARTx)
    {
      HAL_NVIC_SetPriority(USART2_IRQn, UART_ConfigStruct ->UARTx_PreemptPrio, UART_ConfigStruct ->UARTx_SubPrio);
      HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
    else
    {
      HAL_NVIC_SetPriority(USART4_5_IRQn, UART_ConfigStruct ->UARTx_PreemptPrio, UART_ConfigStruct ->UARTx_SubPrio);
      HAL_NVIC_EnableIRQ(USART4_5_IRQn);
    }
  }
  
  return OP_SUCCESS;
}



