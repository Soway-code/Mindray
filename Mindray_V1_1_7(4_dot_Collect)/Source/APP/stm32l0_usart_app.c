/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        stm32l0_usart_app.c
** ����:          ׯ��Ⱥ
** �汾:          V1.0.0
** ����:          2017-04-28
** ����:          ����ͨ��
** ����:          ���ڳ�ʼ�������������շ����жϴ���
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#include "stm32l0_usart_app.h"
#include "stm32l0_timer_app.h"
#include "modbus_ascii.h"
//����һ�ֽ�
uint8_t ReceiveData;
//��������֡����
uint8_t SenBufLen;
//��������֡����
//uint8_t RcvBufLen;
//���ڽ�����ɱ�־
uint8_t UsartRecvOk;
//���ڷ��ͻ���
uint8_t SenBuf[USART1_MAX_DATALEN];
//���ڽ��ջ���
uint8_t RcvBuf[USART1_MAX_DATALEN];
//���ڴ���ṹ��
UART_HandleTypeDef UART_HandleStruct;

//***************************************************************************************************************************************************************
// ����               : gUart1_Config()
// ��������           : 2017-04-28
// ����        	      : ׯ��Ⱥ
// ����               : ����1����ͨ�Ų�������
// �������           : ��
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ����������ͨ��ȫ�ֱ������
// �޸�����           :
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
  __HAL_UART_ENABLE_IT(&UART_HandleStruct, UART_IT_RXNE);                     //ʹ�ܽ����ж�
  __HAL_UART_ENABLE(&UART_HandleStruct);                                      //ʹ��UART1
}

//***************************************************************************************************************************************************************
// ����               : HAL_UART_RxCpltCallback()
// ��������           : 2017-04-28
// ����        	      : ׯ��Ⱥ
// ����               : ���ڽ��ջص�����
// �������           : UART_HandleTypeDef�ṹ��
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         :
// �޸�����           :
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
// ����               : HAL_UART_TxCpltCallback()
// ��������           : 2017-04-28
// ����        	      : ׯ��Ⱥ
// ����               : ���ڷ��ͻص�����
// �������           : UART_HandleTypeDef�ṹ��
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         :
// �޸�����           :
//***************************************************************************************************************************************************************
/*
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if(USART1 == huart->Instance)
  {
    if(0 == UART_HandleStruct.TxXferCount)      //�������
    {
      while(RESET == __HAL_UART_GET_FLAG(&UART_HandleStruct, UART_FLAG_TC))//�ȴ��������־λ��λ
      {
      }
      
      __HAL_UART_CLEAR_FLAG(&UART_HandleStruct, UART_CLEAR_TCF);//���־λ
      
      UsartRecvOk = 0;                                          //��������ձ�־��0
      //RcvBufLen = 0;                                            //�������ݳ�����0
      TX_OFF(GPIOA, GPIO_PIN_8);                                //�л�������״̬               
      HAL_UART_Receive_IT(&UART_HandleStruct, &ReceiveData, 1);
    }
  }
}*/

//***************************************************************************************************************************************************************
// ����               : UART1_ISR_Handle()
// ��������           : 2017-05-05
// ����        	      : ׯ��Ⱥ
// ����               : �����жϷ����ӳ���
// �������           : ��
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         :
// �޸�����           :
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

//�����жϷ��ͣ���ֹ���͵�ͬʱ�������ݣ���������
uint32_t UART1_SendData(uint8_t *buf, uint32_t len)
{
    uint8_t i;
    uint32_t TimeOutCount;
    
    if((0 == len) || (((uint8_t*)NULL) == buf) || (len > sizeof(SenBuf) / sizeof(SenBuf[0])))
    {
        return 0;
    }
    //TX_ON;
    for(i = 0; i < len; i++)                                                   //ע��i<=len,������ٷ����һ���ֽ�     
    {   
        TimeOutCount = 0;
        *(SenBuf + i) = *(buf + i);
        USART1->TDR = *(SenBuf + i);
        while((USART1->ISR & ((uint32_t)1U << (UART_IT_TXE >> 0x08U))) == RESET)
        {
            if(TimeOutCount++ > 0xffff)                                         //��ʱ
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
        
        MODBUS_ASCII_HandlRevData(RecvByteData); //����ASCII����     
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