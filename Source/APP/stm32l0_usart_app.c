/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        stm32l0_usart_app.c
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-04-28
** 描述:          串口通信
** 功能:          串口初始化、串口数据收发、中断处理
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#include "stm32l0_usart_app.h"
#include "stm32l0_timer_app.h"
#include "modbus_ascii.h"
//接收一字节
uint8_t ReceiveData;
//发送数据帧长度
uint8_t SenBufLen;
//接收数据帧长度
//uint8_t RcvBufLen;
//串口接收完成标志
uint8_t UsartRecvOk;
//串口发送缓存
uint8_t SenBuf[USART1_MAX_DATALEN];
//串口接收缓存
uint8_t RcvBuf[USART1_MAX_DATALEN];
//串口处理结构体
UART_HandleTypeDef UART_HandleStruct;

//***************************************************************************************************************************************************************
// 名称               : gUart1_Config()
// 创建日期           : 2017-04-28
// 作者        	      : 庄明群
// 功能               : 串口1基本通信参数配置
// 输入参数           : 无
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 参数的配置通过全局变量完成
// 修改内容           :
//***************************************************************************************************************************************************************

void gUart1_Config(void)
{
  UARTx_PortTypeDef UART1_PortInitStructure;
  UARTx_ConfigTypeDef UART1_ConfigStructure;
  
  UART1_PortInitStructure.UARTx = USART1;
  UART1_PortInitStructure.GPIOx = GPIOA;
  UART1_PortInitStructure.UARTx_TX = GPIO_PIN_9;
  UART1_PortInitStructure.UARTx_RX = GPIO_PIN_10;
  
  UART1_ConfigStructure.UARTx = USART1;
  switch(UsartParam.BaudRate)
  {
  case 1:
    UART1_ConfigStructure.UARTx_Baudrate = 2400;
  break;      
      
  case 2:
    UART1_ConfigStructure.UARTx_Baudrate = 4800;
  break;
    
  case 3:
    UART1_ConfigStructure.UARTx_Baudrate = 9600;
  break;
  
  case 4:
    UART1_ConfigStructure.UARTx_Baudrate = 19200;
  break;
  
  case 5:
    UART1_ConfigStructure.UARTx_Baudrate = 38400;
  break;
  
  case 6:
    UART1_ConfigStructure.UARTx_Baudrate = 57600;
  break;
  
  case 7:
    UART1_ConfigStructure.UARTx_Baudrate = 115200;
  break;
  
  default:
    UART1_ConfigStructure.UARTx_Baudrate = 9600;
  break;
  }
  
  switch(UsartParam.Parity)
  {
  case 1:
    UART1_ConfigStructure.UARTx_Parity = UART_PARITY_ODD;
  break;
  
  case 2:
    UART1_ConfigStructure.UARTx_Parity = UART_PARITY_EVEN;
  break;
  
  case 3:
    UART1_ConfigStructure.UARTx_Parity = UART_PARITY_NONE;
  break;
  
  default:
    UART1_ConfigStructure.UARTx_Parity = UART_PARITY_NONE;
  break;
  }
  
  UART1_ConfigStructure.UARTx_PreemptPrio = 1;
  UART1_ConfigStructure.UARTx_SubPrio = 1;
  UART1_ConfigStructure.UARTx_IntEn = ENABLE;
  
  //gUart_Rs485Gpio_Init(GPIOA, GPIO_PIN_8);
  gUart_Gpio_Config(&UART1_PortInitStructure);
  gUart_Param_Init(&UART_HandleStruct, &UART1_ConfigStructure);
  //gTim3_BaudrateRelate(UART1_ConfigStructure.UARTx_Baudrate);
  //HAL_UART_Receive_IT(&UART_HandleStruct, &ReceiveData, 1);
  __HAL_UART_ENABLE_IT(&UART_HandleStruct, UART_IT_RXNE);                     //使能接收中断
  __HAL_UART_ENABLE(&UART_HandleStruct);                                      //使能UART1
}

//***************************************************************************************************************************************************************
// 名称               : HAL_UART_RxCpltCallback()
// 创建日期           : 2017-04-28
// 作者        	      : 庄明群
// 功能               : 串口接收回调函数
// 输入参数           : UART_HandleTypeDef结构体
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         :
// 修改内容           :
//***************************************************************************************************************************************************************
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(USART1 == huart->Instance)
  {
    if((0 == UsartRecvOk) && (USART1_MAX_DATALEN > RcvBufLen))
    {
        MODBUS_ASCII_HandlRevData(ReceiveData);
    }  
    HAL_UART_Receive_IT(huart, &ReceiveData, 1);
  }
}
*/



//***************************************************************************************************************************************************************
// 名称               : HAL_UART_TxCpltCallback()
// 创建日期           : 2017-04-28
// 作者        	      : 庄明群
// 功能               : 串口发送回调函数
// 输入参数           : UART_HandleTypeDef结构体
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         :
// 修改内容           :
//***************************************************************************************************************************************************************
/*
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if(USART1 == huart->Instance)
  {
    if(0 == UART_HandleStruct.TxXferCount)      //发送完毕
    {
      while(RESET == __HAL_UART_GET_FLAG(&UART_HandleStruct, UART_FLAG_TC))//等待发送完标志位置位
      {
      }
      
      __HAL_UART_CLEAR_FLAG(&UART_HandleStruct, UART_CLEAR_TCF);//清标志位
      
      UsartRecvOk = 0;                                          //发送完接收标志置0
      //RcvBufLen = 0;                                            //接收数据长度置0
      TX_OFF(GPIOA, GPIO_PIN_8);                                //切换到接收状态               
      HAL_UART_Receive_IT(&UART_HandleStruct, &ReceiveData, 1);
    }
  }
}*/

//***************************************************************************************************************************************************************
// 名称               : UART1_ISR_Handle()
// 创建日期           : 2017-05-05
// 作者        	      : 庄明群
// 功能               : 串口中断服务子程序
// 输入参数           : 无
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         :
// 修改内容           :
//***************************************************************************************************************************************************************
/*
void UART1_ISR_Handle(void)
{
  if(UART_HandleStruct.ErrorCode != HAL_UART_ERROR_NONE)
  {
    UART_HandleStruct.ErrorCode = HAL_UART_ERROR_NONE;
    
    if(UART_HandleStruct.State == HAL_UART_STATE_BUSY_TX) 
    {
      UART_HandleStruct.State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      UART_HandleStruct.State = HAL_UART_STATE_BUSY_RX;
    }
  }
  HAL_UART_IRQHandler(&UART_HandleStruct);
}
*/

//不用中断发送，防止发送的同时接收数据，导致死机
uint32_t UART1_SendData(uint8_t *buf, uint32_t len)
{
    uint8_t i;
    uint32_t TimeOutCount;
    
    if((0 == len) || (((uint8_t*)NULL) == buf) || (len > sizeof(SenBuf) / sizeof(SenBuf[0])))
    {
        return 0;
    }
    //TX_ON;
    for(i = 0; i < len; i++)                                                   //注意i<=len,否则会少发最后一个字节     
    {   
        TimeOutCount = 0;
        *(SenBuf + i) = *(buf + i);
        USART1->TDR = *(SenBuf + i);
        while((USART1->ISR & ((uint32_t)1U << (UART_IT_TXE >> 0x08U))) == RESET)
        {
            if(TimeOutCount++ > 0xffff)                                         //超时
            {
                //TX_OFF;
                TimeOutCount = 0;
                return 0;
            }
        }
    } 
    //TX_OFF;
    return len;
}


void UART1_ISR_Handle(void)
{
    uint8_t RecvByteData;
  
    if(__HAL_UART_GET_IT(&UART_HandleStruct, UART_IT_RXNE) != RESET)
    {
        __HAL_UART_CLEAR_FLAG(&UART_HandleStruct, UART_IT_RXNE);
        
        RecvByteData = USART1->RDR;
        
        MODBUS_ASCII_HandlRevData(RecvByteData); //接收ASCII数据     
    }
/*    
    if (__HAL_UART_GET_IT(&UART_HandleStruct, UART_IT_TC) != RESET)
    {
        __HAL_UART_CLEAR_FLAG(&UART_HandleStruct, UART_IT_TC);

        if (u8SendIndex >= u8SendNum)
        {
            u8SendNum = 0;
            __HAL_UART_DISABLE_IT(&UART_HandleStruct, UART_IT_TC);
            TX_OFF;
        }
        else
        {
            USART1->TDR = UART1_TXBUFF[u8SendIndex++];
        }
    }*/
}

#include "string.h"
#include "stdarg.h"
#include "stdio.h"
void uprintf(const char *fmt,...)
{
    va_list marker;
    char buff[128];
    memset(buff,0,sizeof(buff));
    va_start(marker, fmt);
    vsprintf(buff,fmt,marker);
    va_end(marker);
    TX_ON(GPIOA, GPIO_PIN_8);
    HAL_UART_Transmit_IT(&UART_HandleStruct, (uint8_t*)buff, strlen(buff));      
}