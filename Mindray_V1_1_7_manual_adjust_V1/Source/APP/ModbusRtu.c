/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        ModbusRtu.c
** ����:          ׯ��Ⱥ
** �汾:          V1.0.0
** ����:          2017-05-08
** ����:          Modbus RTU ͨ��Э��
** ����:          ��д�������Ĵ����궨���ܡ��ɼ����ݽ���
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#include "ModbusRtu.h"
#include "stm32l0_timer_app.h"

uint32_t Calib_CapMin;                                                          //�궨�������ֵ
uint32_t Calib_CapMax;                                                          //�궨����������
uint16_t Calib_ADMin;                                                           //�궨����AD���ֵ
uint16_t Calib_ADLow;                                                           //�궨����AD�¿̶�
uint16_t Calib_ADHih;                                                           //�궨����AD�Ͽ̶�
uint16_t Calib_ADMax;                                                           //�궨����AD������
uint16_t Calib_CapDAMin;                                                        //�궨����DA���ֵ
uint16_t Calib_CapDALow;                                                        //�궨����DA�¿̶�
uint16_t Calib_CapDAHih;                                                        //�궨����DA�Ͽ̶�
uint16_t Calib_CapDAMax;                                                        //�궨����DA������
uint16_t Calib_TempDAMin;                                                       //�궨�¶�DA���ֵ
uint16_t Calib_TempDAMax;                                                       //�궨�¶�DA������

//CRC_16�����ѯ��
/* CRC��λ�ֽ�ֵ��*/
const uint8_t auchCRCLo[256] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* CRC��λ�ֽ�ֵ��*/
const uint8_t auchCRCHi[256] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

//****************************************************************************************************************************************************************
// ����               :MDB_Get_CRC16()
// ��������           :2017-05-08
// ����               :ׯ��Ⱥ
// ����               :���������Ϣ֡����CRC16У��
// �������           :Msg(��Ϣ�׵�ַ)��MsgLen(��Ϣ֡����)
// �������           :��
// ���ؽ��           :CRC16У����
// ע���˵��         :��Ϣ���Ȳ���Ϊ0
// �޸�����           :
//****************************************************************************************************************************************************************

uint16_t MDB_Get_CRC16(uint8_t *Msg,uint16_t MsgLen) 
{
  //CRC���ֽ�
  uint8_t CRCLo = 0xFF; 
  //CRC���ֽ�
  uint8_t CRCHi = 0xFF; 
  //����
  uint8_t uIndex;  
  uIndex = 0;
  
  while (MsgLen--)  
  { 
    uIndex = CRCLo ^ *Msg++; 
    CRCLo  = CRCHi ^ auchCRCLo[uIndex]; 
    CRCHi  = auchCRCHi[uIndex]; 
  } 
  
  return (CRCHi << 8) | CRCLo;
}

//****************************************************************************************************************************************************************
// ����               :MDB_Snd_Msg_RTU()
// ��������           :2017-05-08
// ����               :ׯ��Ⱥ
// ����               :���� Modbus RTU��Ϣ֡
// �������           :pMsg(��Ϣ�׵�ַ)��len(��Ϣ֡����)
// �������           :��
// ���ؽ��           :��
// ע���˵��         :��Ϣ���Ȳ���Ϊ0������ʱ�Ƚⶳ�豸���豸��ַ����Ϊ�㲥��ַ
// �޸�����           :
//****************************************************************************************************************************************************************

void MDB_Snd_Msg_RTU(uint8_t *pMsg, uint16_t len)
{
  uint8_t ucCRC[2];
  uint16_t uiCRC;
  //��Ч��ַ�������ݳ��ȳ���
  if(((uint8_t *)0 == pMsg) || (0 == len) || ((USART1_MAX_DATALEN - 2) < len))
  {
    return;
  }
  //�㲥��ַ�����豸������
  if((BROADCASTADDR == pMsg[0]) || (FREEZE_DISABLE != UserParam.Freeze))
  {
    return;
  }
  //�õ�CRC��
  uiCRC = MDB_Get_CRC16(pMsg, len);
  //CRC���ֽ���ǰ
  ucCRC[0] = (uint8_t)uiCRC;     
  //CRC���ֽ��ں�
  ucCRC[1] = (uint8_t)(uiCRC >> 8);     

  pMsg[len] = ucCRC[0];
  pMsg[len+1] = ucCRC[1];
  //485 TX_ON
  TX_ON(GPIOA, GPIO_PIN_8);
  SenBufLen = len + 2;
  //�����жϷ���
  HAL_UART_Transmit_IT(&UART_HandleStruct, SenBuf, SenBufLen);
}

//****************************************************************************************************************************************************************
// ����               :MDB_Snd_Msg_RTU_NoLimit()
// ��������           :2017-05-08
// ����               :ׯ��Ⱥ
// ����               :���� Modbus RTU��Ϣ֡(���豸��ַ����)
// �������           :pMsg(��Ϣ�׵�ַ)��len(��Ϣ֡����)
// �������           :��
// ���ؽ��           :��
// ע���˵��         :��Ϣ���Ȳ���Ϊ0������ʱ�Ƚⶳ�豸
// �޸�����           :
//****************************************************************************************************************************************************************

void MDB_Snd_Msg_RTU_NoLimit(uint8_t *pMsg, uint16_t len)
{
  uint8_t ucCRC[2];
  uint16_t uiCRC;
  //���ݵ�ַ��Ч�������ݳ��ȳ���
  if(((uint8_t *)0 == pMsg) || (0 == len) || ((USART1_MAX_DATALEN - 2) < len))
  {
    return;
  }
  //���ڶ��᲻��������
  if(FREEZE_DISABLE != UserParam.Freeze)
  {
    return;
  }
  //�õ�CRC��
  uiCRC = MDB_Get_CRC16(pMsg, len);
  //CRC���ֽ���ǰ
  ucCRC[0] = (uint8_t)uiCRC;   
  //CRC���ֽ��ں�
  ucCRC[1] = (uint8_t)(uiCRC >> 8);     

  pMsg[len] = ucCRC[0];
  pMsg[len+1] = ucCRC[1];
  //485 TX_ON
  TX_ON(GPIOA, GPIO_PIN_8);
  //����У����ĳ���
  SenBufLen = len + 2;
  //���������жϷ���
  HAL_UART_Transmit_IT(&UART_HandleStruct, SenBuf, SenBufLen);
}

//****************************************************************************************************************************************************************
// ����               :ModbusHandle()
// ��������           :2017-05-15
// ����               :ׯ��Ⱥ
// ����               :Modbus RTU��Ϣ֡������
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :
// �޸�����           :
//****************************************************************************************************************************************************************

void ModbusHandle(void)
{
  if(USART_RECV_TIMER_OK == UsartRecvOk)
  {
    //У����󣬵�ַ���󶼲�������Ϣ֡
    if((MDB_Get_CRC16(RcvBuf,RcvBufLen))||(UsartParam.DeviceAddr != RcvBuf[0])&&(BROADCASTADDR != RcvBuf[0]))
    {
      RcvBufLen = 0;
      UsartRecvOk = 0;
      return;
    }
    switch(RcvBuf[1])
    {
    //03�����봦��
    case 0x03:
      ModbusFunc03();
      break;
    //04�����봦��
    case 0x04:
      ModbusFunc04();
      break;
    //05�����봦��
    case 0x05:
      ModbusFunc05();
      break;
    //10�����봦��
    case 0x10:
      ModbusFunc10();
      break;
    //25�����봦��
    case 0x25:
      ModbusFunc25();
      break;
    //26�����봦��    
    case 0x26:
      ModbusFunc26();
      break;
    //27�����봦��    
    case 0x27:
      ModbusFunc27();
      break;
    //2A�����봦��    
    case 0x2A:
      ModbusFunc2A();
      break;
    //2B�����봦��  
    case 0x2B:
      ModbusFunc2B();
      break;
    //��Ч�����봦��  
    default:
      SenBuf[0] = RcvBuf[0];
      SenBuf[1] = (RcvBuf[1] | MB_REQ_FAILURE);
      SenBuf[2] = MB_FUNC_EXCEPTION;
      SenBufLen = 3;
      break;
    }
    //����Modbus RTU
    MDB_Snd_Msg_RTU(SenBuf, SenBufLen);
    RcvBufLen = 0;
    UsartRecvOk = 0;
  }
}

//****************************************************************************************************************************************************************
// ����               :ModbusFunc03()
// ��������           :2017-05-10
// ����               :ׯ��Ⱥ
// ����               :Modbus 03��������Ϣ֡����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :�Ĵ�����ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

void ModbusFunc03(void)
{
  uint16_t Nr;
  //�Ĵ�������
  uint16_t DataBuf;
  //�Ĵ�����ַ
  uint16_t ReadAddr;
  //�Ĵ�������
  uint16_t RegNum;
  //�Ĵ�����ַ���ֽ�
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //�Ĵ�����ַ���ֽ�
  ReadAddr |= RcvBuf[3];

  //�Ĵ������ݸ��ֽ�
  RegNum = RcvBuf[4];
  RegNum <<= 8;
  //�Ĵ����������ֽ�
  RegNum |= RcvBuf[5];
	
  SenBufLen = 0;
  //�豸��ַ
  SenBuf[SenBufLen++] = RcvBuf[0];
  //������
  SenBuf[SenBufLen++] = RcvBuf[1];
  //�����ֽ���
  SenBuf[SenBufLen++] = RegNum * 2;
  //���ʵ�ַ������Ч��Χ��
  if(!((ReadAddr >= HOLDING_REG_REGION1_BGEIN)&&((ReadAddr + RegNum) <= (HOLDING_REG_REGION1_END + 1))))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    //���ʵ�ַ��Ч
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }
	
  for(Nr = 0; Nr < RegNum; Nr++)
  {
    switch(ReadAddr)
    {
    case 0x0030:
      //�豸��ַ
      DataBuf = UsartParam.DeviceAddr;
      break;
			
    case 0x0031:
      //������
      DataBuf = UsartParam.BaudRate;
      break;
			
    case 0x0032:
      //��żУ��
      DataBuf = UsartParam.Parity;
      break;
	
    case 0x0033:
      //��
      DataBuf = 0;
      break;
      
    case 0x0034:
      //����ʹ��
      DataBuf = ProductParam.CompenEn;
      break;
      
    case 0x0035:
      //�˲�ϵ��
      DataBuf = UserParam.Filterfactor;
      break;
      
    case 0x0036:
      //�Զ��ϴ�����
      DataBuf = UsartParam.AutoUpload;
      break;
      
    case 0x0037:
      //��������ϵ��K
      DataBuf = (uint16_t)(UserParam.Correct_K * 100);
      break;
      
    case 0x0038:
      //��������ϵ��B
      DataBuf = (uint16_t)UserParam.Correct_B;
      break;
      
    case 0x0039:
      //�����ʽ ����
      DataBuf = UserParam.OutputMode;
      break;
      
    case 0x003F:
      //���� ����
      DataBuf = ProductParam.HeightRange;
      break;
      
    case 0x0040:
      //��
      DataBuf = 0;
      break;
      
    case 0x0041:    
      DataBuf = UserParam.LiquidType;                                           //Һ�����ͣ���ҩ���ͣ�
      break;
      
    default:
      DataBuf = 0;
      break;
    }
    SenBuf[SenBufLen++] = (unsigned char)(DataBuf >> 8);
    SenBuf[SenBufLen++] = (unsigned char)DataBuf;
    ReadAddr++;
  }
}

//****************************************************************************************************************************************************************
// ����               :ModbusFunc04()
// ��������           :2017-05-10
// ����               :ׯ��Ⱥ
// ����               :Modbus 04��������Ϣ֡����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :�Ĵ�����ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

void ModbusFunc04(void)
{
  //��������
  float fbuf;
  uint16_t Nr;
  //�Ĵ�����ַ
  uint16_t ReadAddr;
  //�Ĵ�������
  uint16_t RegNum;
  //�Ĵ�������
  uint32_t DataBuf;
  //�Ĵ�����ַ���ֽ�
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //�Ĵ�����ַ���ֽ�
  ReadAddr |= RcvBuf[3];

  //�Ĵ����������ֽ�
  RegNum = RcvBuf[4];
  RegNum <<= 8;
  //�Ĵ����������ֽ�
  RegNum |= RcvBuf[5];
	
  SenBufLen = 0;
  //�豸��ַ
  SenBuf[SenBufLen++] = RcvBuf[0];
  //������
  SenBuf[SenBufLen++] = RcvBuf[1];
  //�����ֽ���
  SenBuf[SenBufLen++] = RegNum * 2;
  //�Ĵ�����ַ��Ч
  if(!(((ReadAddr + RegNum) <= (INPUT_REG_REGION1_END + 2)) 
     || ((ReadAddr == INPUT_REG_REGION2_BEGIN) 
     && ((ReadAddr + RegNum) <= (INPUT_REG_REGION2_END + 2))))
     || (0 != (ReadAddr&0x0001))&&(0 != (RegNum&0x0001)))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_ADDR_EXCEPTION;
    return;
  }
	
  for(Nr = 0; Nr < RegNum; Nr += 2)
  {
    switch(ReadAddr)
    {
    case 0x0000:
      //Һλ�߶�(0.1mm)
      //fbuf = (float)RunVarParam.LiquidHeightAD;
      //DataBuf = *(unsigned long *)&fbuf;
        DataBuf = (uint32_t)(RunVarParam.LiquidHeight + 0.5f);
      break;
			
    case 0x0002:
      //����״̬
      DataBuf = (uint32_t)RunVarParam.ErrStatus;
      break;
			
    case 0x0004:
      //��������
      DataBuf = (uint32_t)UserParam.LiquidType;
      break;
		
    case 0x006:
      //�����¶�
      fbuf = (float)RunVarParam.TemperInAir;
      DataBuf = (uint32_t)((fbuf +273.1f) * 10.0f + 0.5f);
      break;
      
    default:
      DataBuf = 0;
      break;
    }
		
    SenBuf[SenBufLen++] = (DataBuf >> 24);
    SenBuf[SenBufLen++] = (DataBuf >> 16);
    SenBuf[SenBufLen++] = (DataBuf >> 8);
    SenBuf[SenBufLen++] = (unsigned char)DataBuf;
    ReadAddr += 2;
  }
}

//****************************************************************************************************************************************************************
// ����               :ModbusFunc05()
// ��������           :2017-05-10
// ����               :ׯ��Ⱥ
// ����               :Modbus 05��������Ϣ֡����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :д����ʱ��Ҫ��EEPROMд����ʹ�ܣ��Ĵ�����ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

void ModbusFunc05(void)
{
  uint16_t Nr;
  //�Ĵ�����ַ
  uint16_t ReadAddr;
  //�Ĵ�������
  uint16_t DataBuf;

  //�Ĵ�����ַ���ֽ�
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //�Ĵ�����ַ���ֽ�
  ReadAddr |= RcvBuf[3];

  //����Ӧ���ݴ��뷢�ͻ���
  for(Nr = 0; Nr < 6; Nr++)
  {
    SenBuf[Nr] = RcvBuf[Nr];
  }
  SenBufLen = 6;

  //��ַ��Ч��EEPROMʹ���޴�
  if(!((ReadAddr >= SINGLE_COIL_REGION1_BEGIN)&&(ReadAddr <= SINGLE_COIL_REGION1_END)
       &&(EEPROM_WR_ENABLE == ProductParam.EepromWrEn) || (0x0053 == ReadAddr)))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }

  //�������ݸ��ֽ�
  DataBuf = RcvBuf[4];
  DataBuf <<= 8;
  //�������ݵ��ֽ�
  DataBuf |= RcvBuf[5];

  //����������Ч
  if((0x0000 != DataBuf)&&(0xFF00 != DataBuf))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_VALU_EXCEPTION;
    SenBufLen = 3;
    return;
  }
	
  switch(ReadAddr)
  {
  case 0x0050:
    //���ݱ궨
    if(0xFF00 == DataBuf)
    {
      Calib_CapMax = RunVarParam.CapBak;
      RunVarParam.CalibFlag |= CALIB_CAPMAX_FLAG;
    }
    else
    {
      Calib_CapMin = RunVarParam.CapBak;
      RunVarParam.CalibFlag |= CALIB_CAPMIN_FLAG;
    }
    //���ݱ궨��־λ��Ч��д��궨����  
    if(CALIB_CAPEOC_FLAG == RunVarParam.CalibFlag)
    {
      MB_Cap_Calibration();
      RunVarParam.CalibFlag = CALIB_CLEAR;
    }
    
    break;
	
  case 0x0051:
    //�ָ����ݱ궨����ֵ
    if(0xFF00 == DataBuf)
    {
      Calib_CapMin = ProductParam.CapMinBak;
      Calib_CapMax = ProductParam.CapMaxBak;
      MB_Cap_Calibration();
    }
    break;
    
  case 0x0052:
    //�豸�����ⶳ
    if(0xFF00 == DataBuf)
    {
      UserParam.Freeze = FREEZE_ENABLE;
    }
    else
    {
      UserParam.Freeze = FREEZE_DISABLE;
    }
    break;
    
  case 0x0053:
    //EEPROMʹ�ܻ��ֹ
    if(0xFF00 == DataBuf)
    {
      ProductParam.EepromWrEn = EEPROM_WR_ENABLE;
    }
    else
    {
      ProductParam.EepromWrEn = EEPROM_WR_DISABLE;
    }
    break;
			
  default:
    break;
  }	
}

//****************************************************************************************************************************************************************
// ����               :ModbusFunc10()
// ��������           :2017-05-10
// ����               :ׯ��Ⱥ
// ����               :Modbus 10��������Ϣ֡����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :д����ʱ��Ҫ��EEPROMд����ʹ�ܣ��Ĵ�����ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

void ModbusFunc10(void)
{
  uint16_t Nr;
  //��������
  uint16_t Index;
  //�Ĵ�����ַ
  uint16_t ReadAddr;
  //�Ĵ�������
  uint16_t RegNum;
  //16λ�����ݴ�
  uint16_t u16temp;
  //8λ��������
  uint8_t u8array[2];
  
  //�Ĵ�����ַ���ֽ�
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //�Ĵ�����ַ���ֽ�
  ReadAddr |= RcvBuf[3];
	
  //�Ĵ����������ֽ�
  RegNum = RcvBuf[4];
  RegNum <<= 8;
  //�Ĵ����������ֽ�
  RegNum |= RcvBuf[5];

  //��Ӧ֡����
  for(Nr = 0; Nr < 6; Nr++)
  {
    SenBuf[Nr] = RcvBuf[Nr];
  }
  SenBufLen = 6;
  
  //�Ĵ�����ַ��Ч
  if(!((ReadAddr >= MUL_REG_REGION1_BEGIN)&&((ReadAddr + RegNum) <= (MUL_REG_REGION1_END + 1))
       &&(RcvBuf[6] == (RegNum*2))))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }

  //EEPROM���ʽ�ֹ
  if(EEPROM_WR_ENABLE != ProductParam.EepromWrEn)
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_DEVC_EXCEPTION;
    SenBufLen = 3;
    return;
  }
  
  Index = 0;
	
  for(Nr = 0; Nr < RegNum; Nr++)
  {
    u8array[0] = RcvBuf[7 + Index];
    u8array[1] = RcvBuf[8 + Index];
    u16temp = (uint16_t)(RcvBuf[7 + Index] << 8) | RcvBuf[8 + Index];
    
    switch(ReadAddr)
    {
    case 0x0030:
      //�豸��ַ
      if((0 < RcvBuf[8 + Index]) && (0xF8 > RcvBuf[8 + Index]))
      {
        UsartParam.DeviceAddr = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(DEVICE_ADDR, &UsartParam.DeviceAddr, 1);
      }
      break;
			
    case 0x0031:
      //������
      if(0x06 > RcvBuf[8 + Index])
      {
        UsartParam.BaudRate = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(BAUDRATE, &UsartParam.BaudRate, 1);
      }
      break;
			
    case 0x0032:
      //��żУ��
      if(0x03 > RcvBuf[8 + Index])
      {
        UsartParam.Parity = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(PARITY, &UsartParam.Parity, 1);
      }
      break;
      
    case 0x0033:
      //��
      break;
      
    case 0x0034:
      //����ʹ��
      if(2 > RcvBuf[8 + Index])
      {
        ProductParam.CompenEn = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(COMPENSATE, &ProductParam.CompenEn, 1);
      }
      break;
      
    case 0x0035:
      //�˲�ϵ��
      if(0x0A > RcvBuf[8 + Index])
      {
        UserParam.Filterfactor = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(FILTER, &UserParam.Filterfactor, 1);
        SwitchCurFilter(UserParam.Filterfactor);
      }
      break;
      
    case 0x0036:
        //�Զ��ϴ�����
        UsartParam.AutoUpload = RcvBuf[8 + Index];
        if((UsartParam.AutoUpload == 0) || (UsartParam.AutoUpload > 4))
        {
            UsartParam.AutoUpload = 2;
        }
        //UsartParam.AutoUploadCycle = UsartParam.AutoUpload * 10;
        Eeprom_Write3T_MultiBytes(AUTO_UPLOAD, &UsartParam.AutoUpload, 1);
        break;
      
    case 0x0037:
      //��������ϵ��K
      if(0 < u16temp)
      {
        UserParam.Correct_K = (float)u16temp / 100.0;
        Eeprom_Write3T_MultiBytes(CORRECT_K, (uint8_t const *)u8array, 2);
      }
      break;
      
    case 0x0038:
      //��������ϵ��B
      UserParam.Correct_B = (float)u16temp;
      Eeprom_Write3T_MultiBytes(CORRECT_B, (uint8_t const *)u8array, 2);
      break;
      
    case 0x0039:
      //�����ʽ
      if(2 > RcvBuf[8 + Index])
      {
        UserParam.OutputMode = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(OUTPUTMODE, &UserParam.OutputMode, 1);
      }
      break;
      
    case 0x003F:
      //����
      if(u16temp > 0)
      {
        ProductParam.HeightRange = u16temp;
        Eeprom_Write3T_MultiBytes(HEIGHTRANGE, (uint8_t const *)u8array, 2);
      }
      break;
      
    case 0x0040:
      //��
      break;
      
    case 0x0041:
      UserParam.LiquidType = RcvBuf[8 + Index];
      if(UserParam.LiquidType == 0 || UserParam.LiquidType > 5)
     {
        UserParam.LiquidType = 1;
     }
     Eeprom_Write3T_MultiBytes(LIQUIDTYPE, &UserParam.LiquidType, 1);
      break;
      
    default:
      break;
    }     
		
    Index += 2;
    ReadAddr++;
  }
}

//****************************************************************************************************************************************************************
// ����               :ModbusFunc25()
// ��������           :2017-05-11
// ����               :ׯ��Ⱥ
// ����               :Modbus 25��������Ϣ֡����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :д����ʱ��Ҫ��EEPROMд����ʹ�ܣ��Ĵ�����ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

void ModbusFunc25(void)
{
  uint16_t Nr;
  //�Ĵ�����ַ
  uint16_t ReadAddr;
  //��������
  uint16_t DataBuf;
  
  //�Ĵ�����ַ���ֽ�
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //�Ĵ�����ַ���ֽ�
  ReadAddr |= RcvBuf[3];
	
  for(Nr = 0; Nr < 6; Nr++)
  {
    SenBuf[Nr] = RcvBuf[Nr];
  }
  SenBufLen = 6;
  
  //�Ĵ�����ַ��Ч
  if(!((ReadAddr < 0x000A)&&(EEPROM_WR_ENABLE == ProductParam.EepromWrEn)))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }
  
  //�Ĵ������ݸ��ֽ�
  DataBuf = RcvBuf[4];
  DataBuf <<= 8;
  //�Ĵ������ݵ��ֽ�
  DataBuf |= RcvBuf[5];
  
  //�Ĵ�������Ч
  if((0x0000 != DataBuf)&&(0xFF00 != DataBuf))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_VALU_EXCEPTION;
    SenBufLen = 3;
    return;
  }
  
  switch(ReadAddr)
  {
  case 0x0000:
    //��
    break;
    
  case 0x0001:
    //����DA�궨���¿̶���
    if(DAOUTCLIB_ENABLE == ProductParam.CapDA_ClibEn)
    {
      if(0xFF00 == DataBuf)
      {
        RunVarParam.CapDA_ClibFlag |= CALIB_CAPDAHIH_FLAG;
        Calib_CapDAHih = RunVarParam.CapDA_ClibValue;
      }
      else
      {
        RunVarParam.CapDA_ClibFlag |= CALIB_CAPDALOW_FLAG;
        Calib_CapDALow = RunVarParam.CapDA_ClibValue;
      }
    }
    break;
    
  case 0x0002:
    //����DA�궨ʹ��
    if(0xFF00 == DataBuf)
    {
      RunVarParam.CapDA_ClibFlag = CALIB_CLEAR;
      ProductParam.CapDA_ClibEn = DAOUTCLIB_ENABLE;
    }
    else
    {
      ProductParam.CapDA_ClibEn = DAOUTCLIB_DISABLE;
    }
    
    if((DAOUTCLIB_DISABLE == ProductParam.CapDA_ClibEn) && (CALIB_CAPDAEOC_FLAG == RunVarParam.CapDA_ClibFlag))
    {
      MB_CapDAOut_Calibration();
      RunVarParam.CapDA_ClibFlag = CALIB_CLEAR;
    }
    break;
    
  case 0x0003:
    //����DA�궨���������
    if(DAOUTCLIB_ENABLE == ProductParam.CapDA_ClibEn)
    {
      if(0xFF00 == DataBuf)
      {
        RunVarParam.CapDA_ClibFlag |= CALIB_CAPDAMAX_FLAG;
        Calib_CapDAMax = RunVarParam.CapDA_ClibValue;
      }
      else
      {
        RunVarParam.CapDA_ClibFlag |= CALIB_CAPDAMIN_FLAG;
        Calib_CapDAMin = RunVarParam.CapDA_ClibValue;
      }
    }
    break;
    
  case 0x0004:
    //�¶ȱ궨ʹ��
    if(0xFF00 == DataBuf)
    {
      RunVarParam.TempDA_ClibFlag = CALIB_CLEAR;
      ProductParam.TempDA_ClibEn = DAOUTCLIB_ENABLE;
    }
    else
    {
      ProductParam.TempDA_ClibEn = DAOUTCLIB_DISABLE;
    }
    
    if((DAOUTCLIB_DISABLE == ProductParam.TempDA_ClibEn) && (CALIB_TEMPDAEOC_FLAG == RunVarParam.TempDA_ClibFlag))
    {
      MB_TempDAOut_Calibration();
      RunVarParam.TempDA_ClibFlag = CALIB_CLEAR;
    }
    break;
    
  case 0x0005:
    //�¶ȱ궨
    if(DAOUTCLIB_ENABLE == ProductParam.TempDA_ClibEn)
    {
      if(0xFF00 == DataBuf)
      {
        RunVarParam.TempDA_ClibFlag |= CALIB_TEMPDAMAX_FLAG;
        Calib_TempDAMax = RunVarParam.TempDA_ClibValue;
      }
      else
      {
        RunVarParam.TempDA_ClibFlag |= CALIB_TEMPDAMIN_FLAG;
        Calib_TempDAMin = RunVarParam.TempDA_ClibValue;
      }
    }
    break;
    
  case 0x0007:
    //����AD�궨ʹ��
    if(0xFF00 == DataBuf)
    {
      RunVarParam.CapAD_ClibFlag = CALIB_CLEAR;
      ProductParam.CapAD_ClibEn = ADCLIB_ENABLE;
    }
    else
    {
      ProductParam.CapAD_ClibEn = ADCLIB_DISABLE;
    }
    
    if((ADCLIB_DISABLE == ProductParam.CapAD_ClibEn) && (CALIB_CAPADEOC_FLAG == RunVarParam.CapAD_ClibFlag))
    {
      MB_CapAD_Calibration();
      RunVarParam.CapAD_ClibFlag = CALIB_CLEAR;
    }
    
    break;
    
  case 0x0008:
    //����AD�궨���¿̶���
    if(ADCLIB_ENABLE == ProductParam.CapAD_ClibEn)
    {
      if(0xFF00 == DataBuf)
      {
        Calib_ADHih = RunVarParam.CapAD_ClibValue;
        RunVarParam.CapAD_ClibFlag |= CALIB_CAPADHIH_FLAG;
      }
      else
      {
        Calib_ADLow = RunVarParam.CapAD_ClibValue;
        RunVarParam.CapAD_ClibFlag |= CALIB_CAPADLOW_FLAG;
      }
    }
    break;
    
  case 0x0009:
    //����AD�궨���������
    if(ADCLIB_ENABLE == ProductParam.CapAD_ClibEn)
    {
      if(0xFF00 == DataBuf)
      {
        Calib_ADMax = RunVarParam.CapAD_ClibValue;
        RunVarParam.CapAD_ClibFlag |= CALIB_CAPADMAX_FLAG;
      }
      else
      {
        Calib_ADMin = RunVarParam.CapAD_ClibValue;
        RunVarParam.CapAD_ClibFlag |= CALIB_CAPADMIN_FLAG;
      }
    }
    break;
    
  default:
    break;
  }
}

//****************************************************************************************************************************************************************
// ����               :ModbusFunc26()
// ��������           :2017-05-11
// ����               :ׯ��Ⱥ
// ����               :Modbus 26��������Ϣ֡����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :�Ĵ�����ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

void ModbusFunc26(void)
{
  uint8_t i;
  //��������
  float fBuf;
  uint16_t Nr;
  //�Ĵ�����ַ
  uint16_t ReadAddr;
  //�Ĵ�������
  uint16_t RegNum;
  //�Ĵ�����������
  uint32_t DataBuf;
  
  //�Ĵ�����ַ���ֽ�
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //�Ĵ�����ַ���ֽ�
  ReadAddr |= RcvBuf[3];
  
  //�Ĵ����������ֽ�
  RegNum = RcvBuf[4];
  RegNum <<= 8;
  //�Ĵ����������ֽ�
  RegNum |= RcvBuf[5];
  
  SenBufLen = 0;
  //�豸��ַ
  SenBuf[SenBufLen++] = RcvBuf[0];
  //������
  SenBuf[SenBufLen++] = RcvBuf[1];
  //�������ݳ���
  SenBuf[SenBufLen++] = RegNum * 2;
  //�Ĵ������ʵ�ַ��Ч
  if(!((((DEF_MUL_REG_REGION1_BEGIN <= ReadAddr) && (DEF_MUL_REG_REGION1_END >= ReadAddr) && ((ReadAddr + RegNum) <= (DEF_MUL_REG_REGION1_END + 2)))
     || ((DEF_MUL_REG_REGION2_BEGIN <= ReadAddr) && (DEF_MUL_REG_REGION2_END >= ReadAddr) && ((ReadAddr + RegNum) <= (DEF_MUL_REG_REGION2_END + 2)))
     || ((DEF_MUL_REG_REGION3_BEGIN <= ReadAddr) && (DEF_MUL_REG_REGION3_END >= ReadAddr) && ((ReadAddr + RegNum) <= (DEF_MUL_REG_REGION3_END + 2))))
     && (0x00 != RegNum) && (0 == (RegNum&0x01)) && (0 == (ReadAddr&0x01))))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }
  
  for(Nr = 0; Nr < RegNum; Nr += 2, ReadAddr += 2)
  {
    switch(ReadAddr)
    {
    case 0x0080:
      //��
      DataBuf = 0;
      break;
    
    case 0x0082:
      //�������
      fBuf = (float)ProductParam.CapMin;
      DataBuf = *(uint32_t *)&fBuf;
      break;
    
    case 0x0084:
      //����������
      fBuf = (float)ProductParam.CapMax;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x0086:
      //��
      DataBuf = 0;
      break;
    
    case 0x0090:
      //�����¶�����ϵ��K1
      fBuf = UserParam.Temper_K1;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x0092:
      //�����¶�����ϵ��B1
      fBuf = UserParam.Temper_B1;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x0094:
      //Һ���¶�����ϵ��K2
      fBuf = UserParam.Temper_K2;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x0096:
      //Һ���¶�����ϵ��B2
      fBuf = UserParam.Temper_B2;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x00B6:
      //����DA�궨ֵ
      fBuf = (float)RunVarParam.CapDA_ClibValue;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x00B8:
      //�����¶�DA�궨ֵ
      fBuf = (float)RunVarParam.TempDA_ClibValue;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x00BA:
      //����AD�궨ֵ
      fBuf = (float)RunVarParam.CapAD_ClibValue;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    default:
      break;
    }
    
    for(i = 4; i > 0; i--)
    {
      SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1)*8));
    }
  }
}

//****************************************************************************************************************************************************************
// ����               :ModbusFunc27()
// ��������           :2017-05-12
// ����               :ׯ��Ⱥ
// ����               :Modbus 27��������Ϣ֡����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :д����ʱ��Ҫ��EEPROMд����ʹ�ܣ��Ĵ�����ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

void ModbusFunc27(void)
{
  //��������
  float fbuf;
  uint16_t Nr;
  //����
  uint16_t index;
  //�Ĵ�����ַ
  uint16_t ReadAddr;
  //�Ĵ�������
  uint16_t RegNum;
  //��������
  uint16_t DataBuf;
  
  //�Ĵ�����ַ���ֽ�
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //�Ĵ�����ַ���ֽ�
  ReadAddr |= RcvBuf[3];
  
  //�Ĵ����������ֽ�
  RegNum = RcvBuf[4];
  RegNum <<= 8;
  //�Ĵ����������ֽ�
  RegNum |= RcvBuf[5];
  
  for(Nr = 0; Nr < 6; Nr++)
  {
    SenBuf[Nr] = RcvBuf[Nr];
  }
  SenBufLen = 6;
  //EEPROMδʹ�ܷ���ʧ��
  if(EEPROM_WR_ENABLE != ProductParam.EepromWrEn)
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_DEVC_EXCEPTION;
    SenBufLen = 3;
    return;
  }
  //�Ĵ������ʵ�ַ��Ч
  if(!((((DEF_MUL_REG_REGION1_BEGIN <= ReadAddr) && (DEF_MUL_REG_REGION1_END >= ReadAddr) && ((ReadAddr + RegNum) <= (DEF_MUL_REG_REGION1_END + 2)))
     || ((DEF_MUL_REG_REGION2_BEGIN <= ReadAddr) && (DEF_MUL_REG_REGION2_END >= ReadAddr) && ((ReadAddr + RegNum) <= (DEF_MUL_REG_REGION2_END + 2)))
     || ((DEF_MUL_REG_REGION3_BEGIN <= ReadAddr) && (DEF_MUL_REG_REGION3_END >= ReadAddr) && ((ReadAddr + RegNum) <= (DEF_MUL_REG_REGION3_END + 2))))
     && (0x00 != RegNum) && (0 == (RegNum&0x01)) && (0 == (ReadAddr&0x01))))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }
  
  index = 0;
  
  for(Nr = 0; Nr < RegNum; Nr += 2, ReadAddr += 2)
  {
    switch(ReadAddr)
    {
    case 0x0080://����
      //fbuf = HexToFloat(&RcvBuf[7 + index]);
      //ProductParam.HeightRange = (uint16_t)(fbuf * 100);
      //Cur_Param[HEIGHTRANGE] = (unsigned char)(ProductParam.HeightRange >> 8);
      //Cur_Param[HEIGHTRANGE + 1] = (unsigned char)ProductParam.HeightRange;
      //Eeprom_Write3T_MultiBytes(HEIGHTRANGE, (uint8_t const *)&Cur_Param[HEIGHTRANGE], 2);
      break;
      
    case 0x0082:
      //��
      break;
      
    case 0x0084:
      //��
      break;
      
    case 0x0090:
      //�����¶�����ϵ��K1
      fbuf = HexToFloat(&RcvBuf[7 + index]);
      if(0 < fbuf)
      {
        UserParam.Temper_K1 = fbuf;
      }
      DataBuf = (uint16_t)(UserParam.Temper_K1 * 100);
      Cur_Param[TEMPER_K1] = (unsigned char)(DataBuf >> 8);
      Cur_Param[TEMPER_K1 + 1] = (unsigned char)DataBuf;
      Eeprom_Write3T_MultiBytes(TEMPER_K1, (uint8_t const *)&Cur_Param[TEMPER_K1], 2);
      break;
      
    case 0x0092:
      //�����¶�����ϵ��B1
      UserParam.Temper_B1 = HexToFloat(&RcvBuf[7 + index]);
      DataBuf = (uint16_t)(UserParam.Temper_B1 * 100);
      Cur_Param[TEMPER_B1] = (unsigned char)(DataBuf >> 8);
      Cur_Param[TEMPER_B1 + 1] = (unsigned char)DataBuf;
      Eeprom_Write3T_MultiBytes(TEMPER_B1, (uint8_t const *)&Cur_Param[TEMPER_B1], 2);
      break;
      
    case 0x0094:
      //Һ���¶�����ϵ��K2
      fbuf = HexToFloat(&RcvBuf[7 + index]);
      if(0 < fbuf)
      {
        UserParam.Temper_K2 = fbuf;
      }
      DataBuf = (uint16_t)(UserParam.Temper_K2 * 100);
      Cur_Param[TEMPER_K2] = (unsigned char)(DataBuf >> 8);
      Cur_Param[TEMPER_K2 + 1] = (unsigned char)DataBuf;
      Eeprom_Write3T_MultiBytes(TEMPER_K2, (uint8_t const *)&Cur_Param[TEMPER_K2], 2);
      break;
      
    case 0x0096:
      //Һ���¶�����ϵ��B2
      UserParam.Temper_B2 = HexToFloat(&RcvBuf[7 + index]);
      DataBuf = (uint16_t)(UserParam.Temper_B2 * 100);
      Cur_Param[TEMPER_B2] = (unsigned char)(DataBuf >> 8);
      Cur_Param[TEMPER_B2 + 1] = (unsigned char)DataBuf;
      Eeprom_Write3T_MultiBytes(TEMPER_B2, (uint8_t const *)&Cur_Param[TEMPER_B2], 2);
      break;
      
    case 0x00B6:
      //����DA�궨ʹ�ܾ�д��DAֵ
      if(DAOUTCLIB_ENABLE == ProductParam.CapDA_ClibEn)
      {
        fbuf = HexToFloat(&RcvBuf[7 + index]);
        if(DAC_VALUE_MAX > fbuf)
        {
          RunVarParam.CapDA_ClibValue = (uint16_t)fbuf;
        }
        else
        {
          RunVarParam.CapDA_ClibValue = CALIB_CLEAR;
        }
        DAC_Chn_SetValue(RunVarParam.CapDA_ClibValue, 1);
      }
      break;
      
    case 0x00B8:
      //�¶�DA�궨ʹ�ܾ�д��DAֵ
      if(DAOUTCLIB_ENABLE == ProductParam.TempDA_ClibEn)
      {
        fbuf = HexToFloat(&RcvBuf[7 + index]);
        if(DAC_VALUE_MAX > fbuf)
        {
          RunVarParam.TempDA_ClibValue = (uint16_t)fbuf;
        }
        else
        {
          RunVarParam.TempDA_ClibValue = CALIB_CLEAR;
        }
        DAC_Chn_SetValue(RunVarParam.TempDA_ClibValue, 2);
      }
      break;
      
    case 0x00BA:
      //����AD�궨ʹ�ܾ�д��ADֵ
      if(ADCLIB_ENABLE == ProductParam.CapAD_ClibEn)
      {
        fbuf = HexToFloat(&RcvBuf[7 + index]);
        if(65535.0 < fbuf)
        {
          fbuf = 0;
        }
        RunVarParam.CapAD_ClibValue = (uint16_t)fbuf;
      }
      break;
      
    default:
      break;
    }
    
    index += 4;
  }
}

//****************************************************************************************************************************************************************
// ����               :ModbusFunc2A()
// ��������           :2017-05-13
// ����               :ׯ��Ⱥ
// ����               :Modbus 2A��������Ϣ֡����,д�汾��Ϣ
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :д����ʱ��Ҫ��EEPROMд����ʹ�ܣ��Ĵ�����ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

void ModbusFunc2A(void)
{
  uint8_t i;
  uint8_t j;
  uint8_t objlen;
  uint8_t RegNum;
  uint16_t ReadAddr;
  
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  ReadAddr |= RcvBuf[3];
  RegNum = RcvBuf[5];
  
  for(i = 0; i < 6; i++)
  {
    SenBuf[i] = RcvBuf[i];
  }
  
  SenBufLen = 6;
  
  if(!((ReadAddr >= MUL_VERSION_INF_BEGIN)
     &&(ReadAddr <= MUL_VERSION_INF_END)
     &&((ReadAddr + RegNum) <= (MUL_VERSION_INF_END + 1))
     &&(0 != RegNum)&&(0x00 == RcvBuf[4])))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }
  
  j = 6;
  ProductParam.EepromWrEn = EEPROM_WR_ENABLE;
  for(i = 0; i < RegNum; i++)
  {
    switch(ReadAddr)
    {
    //��������
    case 0x00E0:
      objlen = RcvBuf[j] + 1;
      if(0x2F < objlen)
      {
        j += objlen;
        break;
      }
      Eeprom_Write_MultiBytes(ORGANIZATION, (uint8_t const *)&RcvBuf[j], objlen);                                    
      j += objlen;
      break;
    //��Ʒ����    
    case 0x00E1:
      objlen = RcvBuf[j] + 1;
      if(0x2F < objlen)
      {
        j += objlen;
        break;
      }
      Eeprom_Write_MultiBytes(PRODUCTION, (uint8_t const *)&RcvBuf[j], objlen);                               
      j += objlen;
      break;
    //Ӳ���汾    
    case 0x00E2:
      objlen = RcvBuf[j] + 1;
      if(0x2F < objlen)
      {
        j += objlen;
        break;
      }
      Eeprom_Write_MultiBytes(HARDWAREVER, (uint8_t const *)&RcvBuf[j], objlen);                                  
      j += objlen;
      break;
    //����汾    
    case 0x00E3:
      objlen = RcvBuf[j] + 1;
      //                                                                                                    
      j += objlen;
      break;
    //�豸ID    
    case 0x00E4:
      objlen = RcvBuf[j] + 1;
      if(0x2F < objlen)
      {
        j += objlen;
        break;
      }
      Eeprom_Write_MultiBytes(DEVICENUM, (uint8_t const *)&RcvBuf[j], objlen);                                     
      j += objlen;
      break;
    //�ͻ�����    
    case 0x00E5:
      objlen = RcvBuf[j] + 1;
      if(0x2F < objlen)
      {
        j += objlen;
        break;
      }
      Eeprom_Write_MultiBytes(CUSTOMER, (uint8_t const *)&RcvBuf[j], objlen);                               
      j += objlen;
      break;
        
    default:
      break;
    }
    ReadAddr++;
  }
  ProductParam.EepromWrEn = EEPROM_WR_DISABLE;
}

//****************************************************************************************************************************************************************
// ����               :ModbusFunc2B()
// ��������           :2017-05-13
// ����               :ׯ��Ⱥ
// ����               :Modbus 2B��������Ϣ֡����,���汾��Ϣ
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :�Ĵ�����ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

void ModbusFunc2B(void)
{
  uint8_t i;
  uint8_t objlen;
  uint8_t RegNum;
  uint16_t ReadAddr;
  
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  ReadAddr |= RcvBuf[3];
  RegNum = RcvBuf[5];
  
  SenBufLen = 0;
  SenBuf[SenBufLen++] = RcvBuf[0];
  SenBuf[SenBufLen++] = 0x2B;
  SenBuf[SenBufLen++] = RcvBuf[4];
  SenBuf[SenBufLen++] = RcvBuf[5];
  
  if(!((ReadAddr >= MUL_VERSION_INF_BEGIN)
     &&(ReadAddr <= MUL_VERSION_INF_END)
     &&((ReadAddr + RegNum) <= (MUL_VERSION_INF_END + 1))
     &&(0 != RegNum)&&(0x00 == RcvBuf[4])))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }
  
  for(i = 0; i < RegNum; i++)
  {
    switch(ReadAddr)
    {
    //��������  
    case 0x00E0:
      SenBuf[SenBufLen++] = 0x00;
      SenBuf[SenBufLen++] = 0xE0;
      objlen = Eeprom_Read_OneByte(ORGANIZATION);                                                       
      SenBuf[SenBufLen++] = objlen;
      if((objlen > (USART1_MAX_DATALEN/3))||(0 == objlen))
      {
        SenBuf[SenBufLen - 1] = 1;
        SenBuf[SenBufLen++] = 0;
        break;
      }
      Eeprom_Read_MultiBytes((ORGANIZATION + 1), &SenBuf[SenBufLen], objlen);
      SenBufLen += objlen;
      break;
    //��Ʒ����  
    case 0x00E1:
      SenBuf[SenBufLen++] = 0x00;
      SenBuf[SenBufLen++] = 0xE1;
      objlen = Eeprom_Read_OneByte(PRODUCTION);                                                  
      SenBuf[SenBufLen++] = objlen;
      if((objlen > (USART1_MAX_DATALEN/3))||(0 == objlen))
      {
        SenBuf[SenBufLen - 1] = 1;
        SenBuf[SenBufLen++] = 0;
        break;
      }
      Eeprom_Read_MultiBytes((PRODUCTION + 1), &SenBuf[SenBufLen], objlen);
      SenBufLen += objlen;
      break;
    //Ӳ���汾  
    case 0x00E2:
      SenBuf[SenBufLen++] = 0x00;
      SenBuf[SenBufLen++] = 0xE2;
      objlen = Eeprom_Read_OneByte(HARDWAREVER);                                                     
      SenBuf[SenBufLen++] = objlen;
      if((objlen > (USART1_MAX_DATALEN/3))||(0 == objlen))
      {
        SenBuf[SenBufLen - 1] = 1;
        SenBuf[SenBufLen++] = 0;
        break;
      }
      Eeprom_Read_MultiBytes((HARDWAREVER + 1), &SenBuf[SenBufLen], objlen);
      SenBufLen += objlen;
      break;
    //����汾 
    case 0x00E3:
      SenBuf[SenBufLen++] = 0x00;
      SenBuf[SenBufLen++] = 0xE3;                                                     
      SenBuf[SenBufLen++] = 0x07;
      
      for(objlen = 0; objlen < 7; objlen++)                                                        
      {
        SenBuf[SenBufLen++] = SensorSoftVersion[objlen + 1];
      }
      break;
    //�豸ID  
    case 0x00E4:
      SenBuf[SenBufLen++] = 0x00;
      SenBuf[SenBufLen++] = 0xE4;
      objlen = Eeprom_Read_OneByte(DEVICENUM);                                                        
      SenBuf[SenBufLen++] = objlen;
      if((objlen > (USART1_MAX_DATALEN/3))||(0 == objlen))
      {
        SenBuf[SenBufLen - 1] = 1;
        SenBuf[SenBufLen++] = 0;
        break;
      }
      Eeprom_Read_MultiBytes((DEVICENUM + 1), &SenBuf[SenBufLen], objlen);
      SenBufLen += objlen;
      break;
    //�ͻ�����  
    case 0x00E5:
      SenBuf[SenBufLen++] = 0x00;
      SenBuf[SenBufLen++] = 0xE5;
      objlen = Eeprom_Read_OneByte(CUSTOMER);                                                  
      SenBuf[SenBufLen++] = objlen;
      if((objlen > (USART1_MAX_DATALEN/3))||(0 == objlen))
      {
        SenBuf[SenBufLen - 1] = 1;
        SenBuf[SenBufLen++] = 0;
        break;
      }
      Eeprom_Read_MultiBytes((CUSTOMER + 1), &SenBuf[SenBufLen], objlen);
      SenBufLen += objlen;
      break;
      
    default:
      break;
    }
    
    ReadAddr++;
  }
}

//****************************************************************************************************************************************************************
// ����               :ModbusAutoUpload()
// ��������           :2017-05-13
// ����               :ׯ��Ⱥ
// ����               :Modbus ��Ϣ֡�Զ��ϴ�����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :
// �޸�����           :
//****************************************************************************************************************************************************************

void ModbusAutoUpload(void)
{
    float fbuf;
    uint8_t i;
    uint32_t DataBuf;

    SenBufLen = 0;
    SenBuf[SenBufLen++] = UsartParam.DeviceAddr;
    SenBuf[SenBufLen++] = 0x04;
    SenBuf[SenBufLen++] = 0x10;

    fbuf = (float)(RunVarParam.LiquidHeight + 0.5f);
    DataBuf = (uint32_t)fbuf;
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }

    DataBuf = (uint32_t)RunVarParam.ErrStatus;
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }

    
    DataBuf = (uint32_t)UserParam.LiquidType;
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1)*8));
    }
    
    fbuf = (float)RunVarParam.TemperInAir;
    DataBuf = (uint32_t)((fbuf +273.1f) * 10.0f + 0.5f);
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1)*8));
    }
    
    MDB_Snd_Msg_RTU_NoLimit(SenBuf, SenBufLen);
}



//�ѱ���ת����ʱ��
static uint8_t AutoUploadTimeConvert(uint8_t Code)
{
    uint8_t RecvCode = Code;
    uint8_t ReturnValue;
    switch(RecvCode)
    {
    case 1:
        ReturnValue = 0;
        break;
        
    case 2:
        ReturnValue = 1;
        break;
        
    case 3:
        ReturnValue = 5;
        break;
        
    case 4:
        ReturnValue = 10;
        break;
        
    default:
        ReturnValue = 2;        //ȱʡֵΪ2
        break;
    }
    return ReturnValue;
}


//****************************************************************************************************************************************************************
// ����               :SensorEvent()
// ��������           :2017-05-14
// ����               :ׯ��Ⱥ
// ����               :�������¼�����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :
// �޸�����           :
//****************************************************************************************************************************************************************

void SensorEvent(void)
{
    //���ʹ���Զ��ϴ����Զ��ϴ���ʱʱ�䵽
    if((1 != UsartParam.AutoUpload) && (UsartParam.AutoUploadCn >= AutoUploadTimeConvert(UsartParam.AutoUpload) * 10))
    {
        //Modbus֡�Զ��ϴ�
        ModbusAutoUpload();
        UsartParam.AutoUploadCn = 0;
    }
}

//****************************************************************************************************************************************************************
// ����               :MB_Cap_Calibration()
// ��������           :2017-05-15
// ����               :ׯ��Ⱥ
// ����               :���ݱ궨����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :
// �޸�����           :
//****************************************************************************************************************************************************************

void MB_Cap_Calibration(void)
{
  if(Calib_CapMin < Calib_CapMax)
  {
    //���������뻺��
    Cur_Param[CAPMIN] = (unsigned char)(Calib_CapMin >> 24);
    Cur_Param[CAPMIN + 1] = (unsigned char)(Calib_CapMin >> 16);
    Cur_Param[CAPMIN + 2] = (unsigned char)(Calib_CapMin >> 8);
    Cur_Param[CAPMIN + 3] = (unsigned char)Calib_CapMin;
    //����������뻺��
    Cur_Param[CAPMAX] = (unsigned char)(Calib_CapMax >> 24);
    Cur_Param[CAPMAX + 1] = (unsigned char)(Calib_CapMax >> 16);
    Cur_Param[CAPMAX + 2] = (unsigned char)(Calib_CapMax >> 8);
    Cur_Param[CAPMAX + 3] = (unsigned char)Calib_CapMax;
    //�������
    ProductParam.CapMin = Calib_CapMin;
    //��������
    ProductParam.CapMax = Calib_CapMax;
    //���ݱ궨����д��EEPROM
    Eeprom_Write3T_MultiBytes(CAPMIN, (uint8_t const *)&Cur_Param[CAPMIN], 8);
    //����������㱸��ֵ��ȱʡֵ��д��궨ֵ
    if(0xFFFFFFFF == ProductParam.CapMaxBak)
    {
      //���ݱ�������д��EEPROM
      Eeprom_Write3T_MultiBytes(CAPMINBAK, (uint8_t const *)&Cur_Param[CAPMIN], 8);
      //������㱸��
      ProductParam.CapMinBak = Calib_CapMin;
      //�������㱸��
      ProductParam.CapMaxBak = Calib_CapMax;
    }
  }
  
  ProductParam.CapRange = ProductParam.CapMax - ProductParam.CapMin;
  //����Һλ������ֵΪ���̵İٷ�֮��
  ProductParam.CapWaveLimit = (uint32_t)(ProductParam.CapRange * 0.03);
}

//****************************************************************************************************************************************************************
// ����               :MB_CapAD_Calibration()
// ��������           :2017-05-15
// ����               :ׯ��Ⱥ
// ����               :����ADֵ�궨����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :�궨ֵ���ܳ���65535
// �޸�����           :
//****************************************************************************************************************************************************************

void MB_CapAD_Calibration(void)
{
  //�������AD�궨ֵ��Ч��д��EEPROM
  if((Calib_ADMin < Calib_ADLow) && (Calib_ADLow < Calib_ADHih) && (Calib_ADHih < Calib_ADMax))
  {
    Cur_Param[CAPADMIN] = (unsigned char)(Calib_ADMin >> 8);
    Cur_Param[CAPADMIN + 1] = (unsigned char)Calib_ADMin;
    Cur_Param[CAPADLOW] = (unsigned char)(Calib_ADLow >> 8);
    Cur_Param[CAPADLOW + 1] = (unsigned char)Calib_ADLow;
    Cur_Param[CAPADHIH] = (unsigned char)(Calib_ADHih >> 8);
    Cur_Param[CAPADHIH + 1] = (unsigned char)Calib_ADHih;
    Cur_Param[CAPADMAX] = (unsigned char)(Calib_ADMax >> 8);
    Cur_Param[CAPADMAX + 1] = (unsigned char)Calib_ADMax;
    
    ProductParam.CapADMin = Calib_ADMin;
    ProductParam.CapADLow = Calib_ADLow;
    ProductParam.CapADHih = Calib_ADHih;
    ProductParam.CapADMax = Calib_ADMax;
    Eeprom_Write3T_MultiBytes(CAPADMIN, (uint8_t const *)&Cur_Param[CAPADMIN], 8);
  }
  
  ProductParam.CapADRange = ProductParam.CapADMax - ProductParam.CapADMin;
}

//****************************************************************************************************************************************************************
// ����               :MB_CapDAOut_Calibration()
// ��������           :2017-05-15
// ����               :ׯ��Ⱥ
// ����               :����DAֵ�궨����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :�궨ֵ���ܳ���4095
// �޸�����           :
//****************************************************************************************************************************************************************

void MB_CapDAOut_Calibration(void)
{
  //�������DA�궨ֵ��Ч��д��EEPROM
  if((Calib_CapDALow > Calib_CapDAMin) && (Calib_CapDAHih > Calib_CapDALow) && (Calib_CapDAMax > Calib_CapDAHih))
  {
    Cur_Param[CAPDAMIN] = (unsigned char)(Calib_CapDAMin >> 8);
    Cur_Param[CAPDAMIN + 1] = (unsigned char)Calib_CapDAMin;
    Cur_Param[CAPDALOW] = (unsigned char)(Calib_CapDALow >> 8);
    Cur_Param[CAPDALOW + 1] = (unsigned char)Calib_CapDALow;
    Cur_Param[CAPDAHIH] = (unsigned char)(Calib_CapDAHih >> 8);
    Cur_Param[CAPDAHIH + 1] = (unsigned char)Calib_CapDAHih;
    Cur_Param[CAPDAMAX] = (unsigned char)(Calib_CapDAMax >> 8);
    Cur_Param[CAPDAMAX + 1] = (unsigned char)Calib_CapDAMax;
    Eeprom_Write3T_MultiBytes(CAPDAMIN, (uint8_t const *)&Cur_Param[CAPDAMIN], 8);
        
    ProductParam.CapDAMin = Calib_CapDAMin;
    ProductParam.CapDALow = Calib_CapDALow;
    ProductParam.CapDAHih = Calib_CapDAHih;
    ProductParam.CapDAMax = Calib_CapDAMax;
    ProductParam.CapDARange = ProductParam.CapDAMax - ProductParam.CapDAMin;
  }
}

//****************************************************************************************************************************************************************
// ����               :MB_TempDAOut_Calibration()
// ��������           :2017-05-15
// ����               :ׯ��Ⱥ
// ����               :�¶�DAֵ�궨����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :�궨ֵ���ܳ���4095
// �޸�����           :
//****************************************************************************************************************************************************************

void MB_TempDAOut_Calibration(void)
{
  //����¶�DA�궨ֵ��Ч��д��EEPROM
  if((Calib_TempDAMax > Calib_TempDAMin) && (DAC_VALUE_MAX > Calib_TempDAMax) && (DAC_VALUE_MAX > Calib_TempDAMin))
  {
    Cur_Param[TEMPDAMIN] = (unsigned char)(Calib_TempDAMin >> 8);
    Cur_Param[TEMPDAMIN + 1] = (unsigned char)Calib_TempDAMin;
    Cur_Param[TEMPDAMAX] = (unsigned char)(Calib_TempDAMax >> 8);
    Cur_Param[TEMPDAMAX + 1] = (unsigned char)Calib_TempDAMax;
    Eeprom_Write3T_MultiBytes(TEMPDAMIN, (uint8_t const *)&Cur_Param[TEMPDAMIN], 4);
        
    ProductParam.TempDAMin = Calib_TempDAMin;
    ProductParam.TempDAMax = Calib_TempDAMax;
    ProductParam.TempDARange = ProductParam.TempDAMax - ProductParam.TempDAMin;
  }
}
