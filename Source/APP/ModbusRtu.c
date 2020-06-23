/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        ModbusRtu.c
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-08
** 描述:          Modbus RTU 通信协议
** 功能:          读写参数报文处理、标定功能、采集数据交互
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#include "ModbusRtu.h"
#include "stm32l0_timer_app.h"

uint32_t Calib_CapMin;                                                          //标定电容零点值
uint32_t Calib_CapMax;                                                          //标定电容满量程
uint16_t Calib_ADMin;                                                           //标定电容AD零点值
uint16_t Calib_ADLow;                                                           //标定电容AD下刻度
uint16_t Calib_ADHih;                                                           //标定电容AD上刻度
uint16_t Calib_ADMax;                                                           //标定电容AD满量程
uint16_t Calib_CapDAMin;                                                        //标定电容DA零点值
uint16_t Calib_CapDALow;                                                        //标定电容DA下刻度
uint16_t Calib_CapDAHih;                                                        //标定电容DA上刻度
uint16_t Calib_CapDAMax;                                                        //标定电容DA满量程
uint16_t Calib_TempDAMin;                                                       //标定温度DA零点值
uint16_t Calib_TempDAMax;                                                       //标定温度DA满量程

//CRC_16正规查询表
/* CRC低位字节值表*/
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

/* CRC高位字节值表*/
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
// 名称               :MDB_Get_CRC16()
// 创建日期           :2017-05-08
// 作者               :庄明群
// 功能               :对输入的消息帧进行CRC16校验
// 输入参数           :Msg(消息首地址)，MsgLen(消息帧长度)
// 输出参数           :无
// 返回结果           :CRC16校验码
// 注意和说明         :消息长度不能为0
// 修改内容           :
//****************************************************************************************************************************************************************

uint16_t MDB_Get_CRC16(uint8_t *Msg,uint16_t MsgLen) 
{
  //CRC低字节
  uint8_t CRCLo = 0xFF; 
  //CRC高字节
  uint8_t CRCHi = 0xFF; 
  //索引
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
// 名称               :MDB_Snd_Msg_RTU()
// 创建日期           :2017-05-08
// 作者               :庄明群
// 功能               :发送 Modbus RTU消息帧
// 输入参数           :pMsg(消息首地址)，len(消息帧长度)
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :消息长度不能为0，发送时先解冻设备，设备地址不能为广播地址
// 修改内容           :
//****************************************************************************************************************************************************************

void MDB_Snd_Msg_RTU(uint8_t *pMsg, uint16_t len)
{
  uint8_t ucCRC[2];
  uint16_t uiCRC;
  //无效地址或者数据长度超限
  if(((uint8_t *)0 == pMsg) || (0 == len) || ((USART1_MAX_DATALEN - 2) < len))
  {
    return;
  }
  //广播地址或者设备被冻结
  if((BROADCASTADDR == pMsg[0]) || (FREEZE_DISABLE != UserParam.Freeze))
  {
    return;
  }
  //得到CRC码
  uiCRC = MDB_Get_CRC16(pMsg, len);
  //CRC低字节在前
  ucCRC[0] = (uint8_t)uiCRC;     
  //CRC高字节在后
  ucCRC[1] = (uint8_t)(uiCRC >> 8);     

  pMsg[len] = ucCRC[0];
  pMsg[len+1] = ucCRC[1];
  //485 TX_ON
  TX_ON(GPIOA, GPIO_PIN_8);
  SenBufLen = len + 2;
  //开启中断发送
  HAL_UART_Transmit_IT(&UART_HandleStruct, SenBuf, SenBufLen);
}

//****************************************************************************************************************************************************************
// 名称               :MDB_Snd_Msg_RTU_NoLimit()
// 创建日期           :2017-05-08
// 作者               :庄明群
// 功能               :发送 Modbus RTU消息帧(无设备地址限制)
// 输入参数           :pMsg(消息首地址)，len(消息帧长度)
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :消息长度不能为0，发送时先解冻设备
// 修改内容           :
//****************************************************************************************************************************************************************

void MDB_Snd_Msg_RTU_NoLimit(uint8_t *pMsg, uint16_t len)
{
  uint8_t ucCRC[2];
  uint16_t uiCRC;
  //数据地址无效或者数据长度超限
  if(((uint8_t *)0 == pMsg) || (0 == len) || ((USART1_MAX_DATALEN - 2) < len))
  {
    return;
  }
  //串口冻结不发送数据
  if(FREEZE_DISABLE != UserParam.Freeze)
  {
    return;
  }
  //得到CRC码
  uiCRC = MDB_Get_CRC16(pMsg, len);
  //CRC低字节在前
  ucCRC[0] = (uint8_t)uiCRC;   
  //CRC高字节在后
  ucCRC[1] = (uint8_t)(uiCRC >> 8);     

  pMsg[len] = ucCRC[0];
  pMsg[len+1] = ucCRC[1];
  //485 TX_ON
  TX_ON(GPIOA, GPIO_PIN_8);
  //加上校验码的长度
  SenBufLen = len + 2;
  //开启串口中断发送
  HAL_UART_Transmit_IT(&UART_HandleStruct, SenBuf, SenBufLen);
}

//****************************************************************************************************************************************************************
// 名称               :ModbusHandle()
// 创建日期           :2017-05-15
// 作者               :庄明群
// 功能               :Modbus RTU消息帧处理函数
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :
// 修改内容           :
//****************************************************************************************************************************************************************

void ModbusHandle(void)
{
  if(USART_RECV_TIMER_OK == UsartRecvOk)
  {
    //校验错误，地址错误都不处理消息帧
    if((MDB_Get_CRC16(RcvBuf,RcvBufLen))||(UsartParam.DeviceAddr != RcvBuf[0])&&(BROADCASTADDR != RcvBuf[0]))
    {
      RcvBufLen = 0;
      UsartRecvOk = 0;
      return;
    }
    switch(RcvBuf[1])
    {
    //03功能码处理
    case 0x03:
      ModbusFunc03();
      break;
    //04功能码处理
    case 0x04:
      ModbusFunc04();
      break;
    //05功能码处理
    case 0x05:
      ModbusFunc05();
      break;
    //10功能码处理
    case 0x10:
      ModbusFunc10();
      break;
    //25功能码处理
    case 0x25:
      ModbusFunc25();
      break;
    //26功能码处理    
    case 0x26:
      ModbusFunc26();
      break;
    //27功能码处理    
    case 0x27:
      ModbusFunc27();
      break;
    //2A功能码处理    
    case 0x2A:
      ModbusFunc2A();
      break;
    //2B功能码处理  
    case 0x2B:
      ModbusFunc2B();
      break;
    //无效功能码处理  
    default:
      SenBuf[0] = RcvBuf[0];
      SenBuf[1] = (RcvBuf[1] | MB_REQ_FAILURE);
      SenBuf[2] = MB_FUNC_EXCEPTION;
      SenBufLen = 3;
      break;
    }
    //发送Modbus RTU
    MDB_Snd_Msg_RTU(SenBuf, SenBufLen);
    RcvBufLen = 0;
    UsartRecvOk = 0;
  }
}

//****************************************************************************************************************************************************************
// 名称               :ModbusFunc03()
// 创建日期           :2017-05-10
// 作者               :庄明群
// 功能               :Modbus 03功能码消息帧处理
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :寄存器地址不能越界
// 修改内容           :
//****************************************************************************************************************************************************************

void ModbusFunc03(void)
{
  uint16_t Nr;
  //寄存器数据
  uint16_t DataBuf;
  //寄存器地址
  uint16_t ReadAddr;
  //寄存器数量
  uint16_t RegNum;
  //寄存器地址高字节
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //寄存器地址高字节
  ReadAddr |= RcvBuf[3];

  //寄存器数据高字节
  RegNum = RcvBuf[4];
  RegNum <<= 8;
  //寄存器数量低字节
  RegNum |= RcvBuf[5];
	
  SenBufLen = 0;
  //设备地址
  SenBuf[SenBufLen++] = RcvBuf[0];
  //功能码
  SenBuf[SenBufLen++] = RcvBuf[1];
  //数据字节数
  SenBuf[SenBufLen++] = RegNum * 2;
  //访问地址不在有效范围内
  if(!((ReadAddr >= HOLDING_REG_REGION1_BGEIN)&&((ReadAddr + RegNum) <= (HOLDING_REG_REGION1_END + 1))))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    //访问地址无效
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }
	
  for(Nr = 0; Nr < RegNum; Nr++)
  {
    switch(ReadAddr)
    {
    case 0x0030:
      //设备地址
      DataBuf = UsartParam.DeviceAddr;
      break;
			
    case 0x0031:
      //波特率
      DataBuf = UsartParam.BaudRate;
      break;
			
    case 0x0032:
      //奇偶校验
      DataBuf = UsartParam.Parity;
      break;
	
    case 0x0033:
      //空
      DataBuf = 0;
      break;
      
    case 0x0034:
      //补偿使能
      DataBuf = ProductParam.CompenEn;
      break;
      
    case 0x0035:
      //滤波系数
      DataBuf = UserParam.Filterfactor;
      break;
      
    case 0x0036:
      //自动上传周期
      DataBuf = UsartParam.AutoUpload;
      break;
      
    case 0x0037:
      //电容修正系数K
      DataBuf = (uint16_t)(UserParam.Correct_K * 100);
      break;
      
    case 0x0038:
      //电容修正系数B
      DataBuf = (uint16_t)UserParam.Correct_B;
      break;
      
    case 0x0039:
      //输出方式 保留
      DataBuf = UserParam.OutputMode;
      break;
      
    case 0x003F:
      //量程 保留
      DataBuf = ProductParam.HeightRange;
      break;
      
    case 0x0040:
      //空
      DataBuf = 0;
      break;
      
    case 0x0041:    
      DataBuf = UserParam.LiquidType;                                           //液体类型（麻药类型）
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
// 名称               :ModbusFunc04()
// 创建日期           :2017-05-10
// 作者               :庄明群
// 功能               :Modbus 04功能码消息帧处理
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :寄存器地址不能越界
// 修改内容           :
//****************************************************************************************************************************************************************

void ModbusFunc04(void)
{
  //浮点数据
  float fbuf;
  uint16_t Nr;
  //寄存器地址
  uint16_t ReadAddr;
  //寄存器数量
  uint16_t RegNum;
  //寄存器数据
  uint32_t DataBuf;
  //寄存器地址高字节
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //寄存器地址低字节
  ReadAddr |= RcvBuf[3];

  //寄存器数量高字节
  RegNum = RcvBuf[4];
  RegNum <<= 8;
  //寄存器数量低字节
  RegNum |= RcvBuf[5];
	
  SenBufLen = 0;
  //设备地址
  SenBuf[SenBufLen++] = RcvBuf[0];
  //功能码
  SenBuf[SenBufLen++] = RcvBuf[1];
  //数据字节数
  SenBuf[SenBufLen++] = RegNum * 2;
  //寄存器地址无效
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
      //液位高度(0.1mm)
      //fbuf = (float)RunVarParam.LiquidHeightAD;
      //DataBuf = *(unsigned long *)&fbuf;
        DataBuf = (uint32_t)(RunVarParam.LiquidHeight + 0.5f);
      break;
			
    case 0x0002:
      //错误状态
      DataBuf = (uint32_t)RunVarParam.ErrStatus;
      break;
			
    case 0x0004:
      //介质类型
      DataBuf = (uint32_t)UserParam.LiquidType;
      break;
		
    case 0x006:
      //环境温度
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
// 名称               :ModbusFunc05()
// 创建日期           :2017-05-10
// 作者               :庄明群
// 功能               :Modbus 05功能码消息帧处理
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :写操作时先要打开EEPROM写操作使能，寄存器地址不能越界
// 修改内容           :
//****************************************************************************************************************************************************************

void ModbusFunc05(void)
{
  uint16_t Nr;
  //寄存器地址
  uint16_t ReadAddr;
  //寄存器数据
  uint16_t DataBuf;

  //寄存器地址高字节
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //寄存器地址低字节
  ReadAddr |= RcvBuf[3];

  //将响应数据存入发送缓存
  for(Nr = 0; Nr < 6; Nr++)
  {
    SenBuf[Nr] = RcvBuf[Nr];
  }
  SenBufLen = 6;

  //地址无效或EEPROM使能无打开
  if(!((ReadAddr >= SINGLE_COIL_REGION1_BEGIN)&&(ReadAddr <= SINGLE_COIL_REGION1_END)
       &&(EEPROM_WR_ENABLE == ProductParam.EepromWrEn) || (0x0053 == ReadAddr)))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }

  //数据内容高字节
  DataBuf = RcvBuf[4];
  DataBuf <<= 8;
  //数据内容低字节
  DataBuf |= RcvBuf[5];

  //数据内容无效
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
    //电容标定
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
    //电容标定标志位有效就写入标定内容  
    if(CALIB_CAPEOC_FLAG == RunVarParam.CalibFlag)
    {
      MB_Cap_Calibration();
      RunVarParam.CalibFlag = CALIB_CLEAR;
    }
    
    break;
	
  case 0x0051:
    //恢复电容标定出厂值
    if(0xFF00 == DataBuf)
    {
      Calib_CapMin = ProductParam.CapMinBak;
      Calib_CapMax = ProductParam.CapMaxBak;
      MB_Cap_Calibration();
    }
    break;
    
  case 0x0052:
    //设备冻结或解冻
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
    //EEPROM使能或禁止
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
// 名称               :ModbusFunc10()
// 创建日期           :2017-05-10
// 作者               :庄明群
// 功能               :Modbus 10功能码消息帧处理
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :写操作时先要打开EEPROM写操作使能，寄存器地址不能越界
// 修改内容           :
//****************************************************************************************************************************************************************

void ModbusFunc10(void)
{
  uint16_t Nr;
  //内容索引
  uint16_t Index;
  //寄存器地址
  uint16_t ReadAddr;
  //寄存器数量
  uint16_t RegNum;
  //16位数据暂存
  uint16_t u16temp;
  //8位数据数组
  uint8_t u8array[2];
  
  //寄存器地址高字节
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //寄存器地址低字节
  ReadAddr |= RcvBuf[3];
	
  //寄存器数量高字节
  RegNum = RcvBuf[4];
  RegNum <<= 8;
  //寄存器数量低字节
  RegNum |= RcvBuf[5];

  //响应帧内容
  for(Nr = 0; Nr < 6; Nr++)
  {
    SenBuf[Nr] = RcvBuf[Nr];
  }
  SenBufLen = 6;
  
  //寄存器地址无效
  if(!((ReadAddr >= MUL_REG_REGION1_BEGIN)&&((ReadAddr + RegNum) <= (MUL_REG_REGION1_END + 1))
       &&(RcvBuf[6] == (RegNum*2))))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }

  //EEPROM访问禁止
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
      //设备地址
      if((0 < RcvBuf[8 + Index]) && (0xF8 > RcvBuf[8 + Index]))
      {
        UsartParam.DeviceAddr = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(DEVICE_ADDR, &UsartParam.DeviceAddr, 1);
      }
      break;
			
    case 0x0031:
      //波特率
      if(0x06 > RcvBuf[8 + Index])
      {
        UsartParam.BaudRate = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(BAUDRATE, &UsartParam.BaudRate, 1);
      }
      break;
			
    case 0x0032:
      //奇偶校验
      if(0x03 > RcvBuf[8 + Index])
      {
        UsartParam.Parity = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(PARITY, &UsartParam.Parity, 1);
      }
      break;
      
    case 0x0033:
      //空
      break;
      
    case 0x0034:
      //补偿使能
      if(2 > RcvBuf[8 + Index])
      {
        ProductParam.CompenEn = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(COMPENSATE, &ProductParam.CompenEn, 1);
      }
      break;
      
    case 0x0035:
      //滤波系数
      if(0x0A > RcvBuf[8 + Index])
      {
        UserParam.Filterfactor = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(FILTER, &UserParam.Filterfactor, 1);
        SwitchCurFilter(UserParam.Filterfactor);
      }
      break;
      
    case 0x0036:
        //自动上传周期
        UsartParam.AutoUpload = RcvBuf[8 + Index];
        if((UsartParam.AutoUpload == 0) || (UsartParam.AutoUpload > 4))
        {
            UsartParam.AutoUpload = 2;
        }
        //UsartParam.AutoUploadCycle = UsartParam.AutoUpload * 10;
        Eeprom_Write3T_MultiBytes(AUTO_UPLOAD, &UsartParam.AutoUpload, 1);
        break;
      
    case 0x0037:
      //电容修正系数K
      if(0 < u16temp)
      {
        UserParam.Correct_K = (float)u16temp / 100.0;
        Eeprom_Write3T_MultiBytes(CORRECT_K, (uint8_t const *)u8array, 2);
      }
      break;
      
    case 0x0038:
      //电容修正系数B
      UserParam.Correct_B = (float)u16temp;
      Eeprom_Write3T_MultiBytes(CORRECT_B, (uint8_t const *)u8array, 2);
      break;
      
    case 0x0039:
      //输出方式
      if(2 > RcvBuf[8 + Index])
      {
        UserParam.OutputMode = RcvBuf[8 + Index];
        Eeprom_Write3T_MultiBytes(OUTPUTMODE, &UserParam.OutputMode, 1);
      }
      break;
      
    case 0x003F:
      //量程
      if(u16temp > 0)
      {
        ProductParam.HeightRange = u16temp;
        Eeprom_Write3T_MultiBytes(HEIGHTRANGE, (uint8_t const *)u8array, 2);
      }
      break;
      
    case 0x0040:
      //空
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
// 名称               :ModbusFunc25()
// 创建日期           :2017-05-11
// 作者               :庄明群
// 功能               :Modbus 25功能码消息帧处理
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :写操作时先要打开EEPROM写操作使能，寄存器地址不能越界
// 修改内容           :
//****************************************************************************************************************************************************************

void ModbusFunc25(void)
{
  uint16_t Nr;
  //寄存器地址
  uint16_t ReadAddr;
  //数据内容
  uint16_t DataBuf;
  
  //寄存器地址高字节
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //寄存器地址低字节
  ReadAddr |= RcvBuf[3];
	
  for(Nr = 0; Nr < 6; Nr++)
  {
    SenBuf[Nr] = RcvBuf[Nr];
  }
  SenBufLen = 6;
  
  //寄存器地址无效
  if(!((ReadAddr < 0x000A)&&(EEPROM_WR_ENABLE == ProductParam.EepromWrEn)))
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_ADDR_EXCEPTION;
    SenBufLen = 3;
    return;
  }
  
  //寄存器内容高字节
  DataBuf = RcvBuf[4];
  DataBuf <<= 8;
  //寄存器内容低字节
  DataBuf |= RcvBuf[5];
  
  //寄存内容无效
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
    //空
    break;
    
  case 0x0001:
    //电容DA标定上下刻度线
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
    //电容DA标定使能
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
    //电容DA标定零点满量程
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
    //温度标定使能
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
    //温度标定
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
    //电容AD标定使能
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
    //电容AD标定上下刻度线
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
    //电容AD标定零点满量程
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
// 名称               :ModbusFunc26()
// 创建日期           :2017-05-11
// 作者               :庄明群
// 功能               :Modbus 26功能码消息帧处理
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :寄存器地址不能越界
// 修改内容           :
//****************************************************************************************************************************************************************

void ModbusFunc26(void)
{
  uint8_t i;
  //浮点数据
  float fBuf;
  uint16_t Nr;
  //寄存器地址
  uint16_t ReadAddr;
  //寄存器数量
  uint16_t RegNum;
  //寄存器数据内容
  uint32_t DataBuf;
  
  //寄存器地址高字节
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //寄存器地址低字节
  ReadAddr |= RcvBuf[3];
  
  //寄存器数量高字节
  RegNum = RcvBuf[4];
  RegNum <<= 8;
  //寄存器数量低字节
  RegNum |= RcvBuf[5];
  
  SenBufLen = 0;
  //设备地址
  SenBuf[SenBufLen++] = RcvBuf[0];
  //功能码
  SenBuf[SenBufLen++] = RcvBuf[1];
  //数据内容长度
  SenBuf[SenBufLen++] = RegNum * 2;
  //寄存器访问地址无效
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
      //空
      DataBuf = 0;
      break;
    
    case 0x0082:
      //电容零点
      fBuf = (float)ProductParam.CapMin;
      DataBuf = *(uint32_t *)&fBuf;
      break;
    
    case 0x0084:
      //电容满量程
      fBuf = (float)ProductParam.CapMax;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x0086:
      //空
      DataBuf = 0;
      break;
    
    case 0x0090:
      //环境温度修正系数K1
      fBuf = UserParam.Temper_K1;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x0092:
      //环境温度修正系数B1
      fBuf = UserParam.Temper_B1;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x0094:
      //液体温度修正系数K2
      fBuf = UserParam.Temper_K2;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x0096:
      //液体温度修正系数B2
      fBuf = UserParam.Temper_B2;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x00B6:
      //电容DA标定值
      fBuf = (float)RunVarParam.CapDA_ClibValue;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x00B8:
      //环境温度DA标定值
      fBuf = (float)RunVarParam.TempDA_ClibValue;
      DataBuf = *(uint32_t *)&fBuf;
      break;
      
    case 0x00BA:
      //电容AD标定值
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
// 名称               :ModbusFunc27()
// 创建日期           :2017-05-12
// 作者               :庄明群
// 功能               :Modbus 27功能码消息帧处理
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :写操作时先要打开EEPROM写操作使能，寄存器地址不能越界
// 修改内容           :
//****************************************************************************************************************************************************************

void ModbusFunc27(void)
{
  //浮点数据
  float fbuf;
  uint16_t Nr;
  //索引
  uint16_t index;
  //寄存器地址
  uint16_t ReadAddr;
  //寄存器数量
  uint16_t RegNum;
  //数据内容
  uint16_t DataBuf;
  
  //寄存器地址高字节
  ReadAddr = RcvBuf[2];
  ReadAddr <<= 8;
  //寄存器地址低字节
  ReadAddr |= RcvBuf[3];
  
  //寄存器数量高字节
  RegNum = RcvBuf[4];
  RegNum <<= 8;
  //寄存器数量低字节
  RegNum |= RcvBuf[5];
  
  for(Nr = 0; Nr < 6; Nr++)
  {
    SenBuf[Nr] = RcvBuf[Nr];
  }
  SenBufLen = 6;
  //EEPROM未使能访问失败
  if(EEPROM_WR_ENABLE != ProductParam.EepromWrEn)
  {
    SenBuf[1] |= MB_REQ_FAILURE;
    SenBuf[2] = MB_DEVC_EXCEPTION;
    SenBufLen = 3;
    return;
  }
  //寄存器访问地址无效
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
    case 0x0080://保留
      //fbuf = HexToFloat(&RcvBuf[7 + index]);
      //ProductParam.HeightRange = (uint16_t)(fbuf * 100);
      //Cur_Param[HEIGHTRANGE] = (unsigned char)(ProductParam.HeightRange >> 8);
      //Cur_Param[HEIGHTRANGE + 1] = (unsigned char)ProductParam.HeightRange;
      //Eeprom_Write3T_MultiBytes(HEIGHTRANGE, (uint8_t const *)&Cur_Param[HEIGHTRANGE], 2);
      break;
      
    case 0x0082:
      //空
      break;
      
    case 0x0084:
      //空
      break;
      
    case 0x0090:
      //环境温度修正系数K1
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
      //环境温度修正系数B1
      UserParam.Temper_B1 = HexToFloat(&RcvBuf[7 + index]);
      DataBuf = (uint16_t)(UserParam.Temper_B1 * 100);
      Cur_Param[TEMPER_B1] = (unsigned char)(DataBuf >> 8);
      Cur_Param[TEMPER_B1 + 1] = (unsigned char)DataBuf;
      Eeprom_Write3T_MultiBytes(TEMPER_B1, (uint8_t const *)&Cur_Param[TEMPER_B1], 2);
      break;
      
    case 0x0094:
      //液体温度修正系数K2
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
      //液体温度修正系数B2
      UserParam.Temper_B2 = HexToFloat(&RcvBuf[7 + index]);
      DataBuf = (uint16_t)(UserParam.Temper_B2 * 100);
      Cur_Param[TEMPER_B2] = (unsigned char)(DataBuf >> 8);
      Cur_Param[TEMPER_B2 + 1] = (unsigned char)DataBuf;
      Eeprom_Write3T_MultiBytes(TEMPER_B2, (uint8_t const *)&Cur_Param[TEMPER_B2], 2);
      break;
      
    case 0x00B6:
      //电容DA标定使能就写入DA值
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
      //温度DA标定使能就写入DA值
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
      //电容AD标定使能就写入AD值
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
// 名称               :ModbusFunc2A()
// 创建日期           :2017-05-13
// 作者               :庄明群
// 功能               :Modbus 2A功能码消息帧处理,写版本信息
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :写操作时先要打开EEPROM写操作使能，寄存器地址不能越界
// 修改内容           :
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
    //机构名称
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
    //产品代号    
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
    //硬件版本    
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
    //软件版本    
    case 0x00E3:
      objlen = RcvBuf[j] + 1;
      //                                                                                                    
      j += objlen;
      break;
    //设备ID    
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
    //客户编码    
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
// 名称               :ModbusFunc2B()
// 创建日期           :2017-05-13
// 作者               :庄明群
// 功能               :Modbus 2B功能码消息帧处理,读版本信息
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :寄存器地址不能越界
// 修改内容           :
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
    //机构名称  
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
    //产品代号  
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
    //硬件版本  
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
    //软件版本 
    case 0x00E3:
      SenBuf[SenBufLen++] = 0x00;
      SenBuf[SenBufLen++] = 0xE3;                                                     
      SenBuf[SenBufLen++] = 0x07;
      
      for(objlen = 0; objlen < 7; objlen++)                                                        
      {
        SenBuf[SenBufLen++] = SensorSoftVersion[objlen + 1];
      }
      break;
    //设备ID  
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
    //客户编码  
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
// 名称               :ModbusAutoUpload()
// 创建日期           :2017-05-13
// 作者               :庄明群
// 功能               :Modbus 消息帧自动上传处理
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :
// 修改内容           :
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



//把编码转换成时间
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
        ReturnValue = 2;        //缺省值为2
        break;
    }
    return ReturnValue;
}


//****************************************************************************************************************************************************************
// 名称               :SensorEvent()
// 创建日期           :2017-05-14
// 作者               :庄明群
// 功能               :传感器事件处理
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :
// 修改内容           :
//****************************************************************************************************************************************************************

void SensorEvent(void)
{
    //如果使能自动上传且自动上传定时时间到
    if((1 != UsartParam.AutoUpload) && (UsartParam.AutoUploadCn >= AutoUploadTimeConvert(UsartParam.AutoUpload) * 10))
    {
        //Modbus帧自动上传
        ModbusAutoUpload();
        UsartParam.AutoUploadCn = 0;
    }
}

//****************************************************************************************************************************************************************
// 名称               :MB_Cap_Calibration()
// 创建日期           :2017-05-15
// 作者               :庄明群
// 功能               :电容标定功能
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :
// 修改内容           :
//****************************************************************************************************************************************************************

void MB_Cap_Calibration(void)
{
  if(Calib_CapMin < Calib_CapMax)
  {
    //电容零点存入缓存
    Cur_Param[CAPMIN] = (unsigned char)(Calib_CapMin >> 24);
    Cur_Param[CAPMIN + 1] = (unsigned char)(Calib_CapMin >> 16);
    Cur_Param[CAPMIN + 2] = (unsigned char)(Calib_CapMin >> 8);
    Cur_Param[CAPMIN + 3] = (unsigned char)Calib_CapMin;
    //电容满点存入缓存
    Cur_Param[CAPMAX] = (unsigned char)(Calib_CapMax >> 24);
    Cur_Param[CAPMAX + 1] = (unsigned char)(Calib_CapMax >> 16);
    Cur_Param[CAPMAX + 2] = (unsigned char)(Calib_CapMax >> 8);
    Cur_Param[CAPMAX + 3] = (unsigned char)Calib_CapMax;
    //电容零点
    ProductParam.CapMin = Calib_CapMin;
    //电容满点
    ProductParam.CapMax = Calib_CapMax;
    //电容标定数据写入EEPROM
    Eeprom_Write3T_MultiBytes(CAPMIN, (uint8_t const *)&Cur_Param[CAPMIN], 8);
    //如果电容满点备份值是缺省值就写入标定值
    if(0xFFFFFFFF == ProductParam.CapMaxBak)
    {
      //电容备份数据写入EEPROM
      Eeprom_Write3T_MultiBytes(CAPMINBAK, (uint8_t const *)&Cur_Param[CAPMIN], 8);
      //电容零点备份
      ProductParam.CapMinBak = Calib_CapMin;
      //电容满点备份
      ProductParam.CapMaxBak = Calib_CapMax;
    }
  }
  
  ProductParam.CapRange = ProductParam.CapMax - ProductParam.CapMin;
  //设置液位波动阈值为量程的百分之三
  ProductParam.CapWaveLimit = (uint32_t)(ProductParam.CapRange * 0.03);
}

//****************************************************************************************************************************************************************
// 名称               :MB_CapAD_Calibration()
// 创建日期           :2017-05-15
// 作者               :庄明群
// 功能               :电容AD值标定功能
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :标定值不能超过65535
// 修改内容           :
//****************************************************************************************************************************************************************

void MB_CapAD_Calibration(void)
{
  //如果电容AD标定值有效就写入EEPROM
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
// 名称               :MB_CapDAOut_Calibration()
// 创建日期           :2017-05-15
// 作者               :庄明群
// 功能               :电容DA值标定功能
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :标定值不能超过4095
// 修改内容           :
//****************************************************************************************************************************************************************

void MB_CapDAOut_Calibration(void)
{
  //如果电容DA标定值有效就写入EEPROM
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
// 名称               :MB_TempDAOut_Calibration()
// 创建日期           :2017-05-15
// 作者               :庄明群
// 功能               :温度DA值标定功能
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :标定值不能超过4095
// 修改内容           :
//****************************************************************************************************************************************************************

void MB_TempDAOut_Calibration(void)
{
  //如果温度DA标定值有效就写入EEPROM
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
