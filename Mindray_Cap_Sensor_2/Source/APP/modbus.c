#include "stm32l0_eeprom_app.h"
#include "modbus.h"
#include "Softspi.h"

//uint32_t Calib_CapMin;                                                        //标定电容零点值
//uint32_t Calib_CapMax;                                                        //标定电容满量程
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

extern uint8_t LiquidStableFlag;                                                //液位稳定标志
extern uint8_t RcvBuf[USART1_MAX_DATALEN];                                      //串口接收缓存
extern uint8_t SenBuf[USART1_MAX_DATALEN];
extern uint8_t const SensorSoftVersion[8];                                      //软件版本
extern uint8_t const User_Default_Param[PRO_DEFAULT_LEN];                       //用户缺省参数
uint8_t SendLen;                                                                //发送数据长度

extern   uint8_t  cnt ;
extern uint8_t  DataError;
//**************************************************************************************************
// 名称         	: MBASCII_GetSlaveAddr()
// 创建日期   	        : 2015-10-29
// 作者         	: wang
// 功能         	:获得设备地址
// 输入参数     	:接收的缓冲数据(u8 *u8Msg)
// 输出参数     	: 设备地址
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

uint32_t MBASCII_GetSlaveAddr(uint8_t *u8Msg)
{
    return(u8Msg[0]);
}

//**************************************************************************************************
// 名称         	: MBASCII_GetFunCode()
// 创建日期   	        : 2015-10-29
// 作者         	: wang
// 功能         	:获得功能码
// 输入参数     	:接收的缓冲数据(u8 *u8Msg)
// 输出参数     	: 功能码
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

uint32_t MBASCII_GetFunCode(uint8_t *u8Msg)
{
    return(u8Msg[1]);
}

//**************************************************************************************************
// 名称         	: MBASCII_SendMsg()
// 创建日期   	        : 2015-10-29
// 作者         	: wang
// 功能         	:发送指令
// 输入参数     	:发送缓冲数据(uint8_t *u8Msg),数据长度(uint32_t u8MsgLen)
// 输出参数     	: 设备地址
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_SendMsg(uint8_t *u8Msg, uint32_t u8MsgLen)
{
    if(MB_ADDRESS_BROADCAST != MBASCII_GetSlaveAddr(u8Msg))
    {
        MODBUS_ASCII_SendData(u8Msg, u8MsgLen );	
    }
}


//**************************************************************************************************
// 名称         	: MBASCII_SendErr()
// 创建日期     	: 2015-07-13
// 作者        	 	: 王小鑫
// 功能         	: 发送MODBUS 错误
// 输入参数     	: u8 ErrCode
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_SendErr(uint8_t ErrCode)
{
    SendLen = 1;
    SenBuf[SendLen++] |= 0x80;
    SenBuf[SendLen++] = ErrCode;

    MBASCII_SendMsg(SenBuf,SendLen);
}

//**************************************************************************************************
// 名称         	: MBASCII_SendMsg_NoLimit()
// 创建日期   	        : 2015-10-29
// 作者         	: wang
// 功能         	:发送指令
// 输入参数     	:发送缓冲数据(uint8_t *u8Msg),数据长度(uint32_t u8MsgLen)
// 输出参数     	: 设备地址
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_SendMsg_NoLimit(uint8_t *u8Msg, uint32_t u8MsgLen)
{
    if (UserParam.Freeze == FREEZE_DISABLE)
    {
        MODBUS_ASCII_SendData(u8Msg, u8MsgLen );
    }
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun03()
// 创建日期     	: 2016-09-01
// 作者        	 	: 庄明群
// 功能         	: Modbus 03功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun03(void)
{
    uint8_t i;
    //uint8_t Addrflg;                                                          //寄存器地址标志 用于标识只读一个从机地址信息时忽略请求帧的从机地址
    uint8_t ReadAdrH;                                                           //寄存器地址高8位
    uint8_t ReadAdrL;                                                           //寄存器地址低8位
    uint16_t Data_Buf;                                                          //数据内容
    uint16_t RegisterNum = (uint16_t)RcvBuf[4]*256 + RcvBuf[5];                 //寄存器数量

    ReadAdrH = RcvBuf[2];
    ReadAdrL = RcvBuf[3];

    SendLen = 0;
    SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;
    SenBuf[SendLen++] = MB_FUNC_READ_HOLDING_REGISTER;
    SenBuf[SendLen++] = RegisterNum * 2;	   
   
    //判断访问寄存器地址是否在有效范围内
    if(!(((ReadAdrL >= ASCII_HOLDING_REG_REGION1_BGEIN) && (ReadAdrL <= ASCII_HOLDING_REG_REGION1_END)          //打开广播地址读设备地址
    && (ReadAdrL + RegisterNum <= (ASCII_HOLDING_REG_REGION1_END + 1))&&((ReadAdrH == UsartParam.DeviceAddr) || (ReadAdrH == MB_ADDRESS_BROADCAST)))
    ||((RegisterNum * 2 == RcvBuf[6])&&(ReadAdrH == UsartParam.DeviceAddr))
    ||((ASCII_HOLDING_REG_REGION1_BGEIN == ReadAdrL)&&(1 == RegisterNum)))||(0 == RegisterNum))
    {
        //返回非法数据地址
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    for(long k = 0; k <  RegisterNum; ReadAdrL++, k++)
    {
        switch(ReadAdrL)
        {
        case 0x30://从机地址
            Data_Buf = UsartParam.DeviceAddr;
            break;

        case 0x31://波特率
            Data_Buf = UsartParam.BaudRate;
            break;

        case 0x32://奇偶校验
            Data_Buf = UsartParam.Parity;
            break;
            
        case 0x33:
            Data_Buf = 0;
            break;            

        case 0x34://补偿使能
            Data_Buf = ProductParam.CompenEn;
            break;

        case 0x35://滤波等级
            Data_Buf = UserParam.Filterfactor;
            break;

        case 0x36://自动上传周期
            Data_Buf = UsartParam.AutoUpload;
            break;

        case 0x37://电容修正系数K
            Data_Buf = (uint32_t)(UserParam.Correct_K * 100);
            break;

        case 0x38://电容修正系数B
            Data_Buf = (uint32_t)UserParam.Correct_B;
            break;

        case 0x39:
            Data_Buf = UserParam.OutputMode;
            break;

        case 0x3A:
            Data_Buf = (uint16_t)(UserParam.Temper_K1 * 100);
            break;    

        case 0x3B:
            Data_Buf = (uint16_t)(UserParam.Temper_B1 * 100);
            break;

        case 0x3C:
            Data_Buf = 0;
            break;

        case 0x3D:
            Data_Buf = 0;
            break;

        case 0x3E:
            Data_Buf = 0;
            break;

        case 0x3F:
            Data_Buf = ProductParam.HeightRange;
            break;

        case 0x40:
            Data_Buf = 0;
            break;

        case 0x41://麻药类型
            Data_Buf = UserParam.LiquidType;
            break;

        default:
            Data_Buf = 0;
            break;
        }

        for(i = 2; i > 0; i--)
        {
            SenBuf[SendLen++] = (uint8_t)(Data_Buf >> ((i - 1) * 8));
        }
    }
    if(ReadAdrH == MB_ADDRESS_BROADCAST)
    {
        MBASCII_SendMsg_NoLimit(SenBuf,SendLen);
    }
    else
    {
        MBASCII_SendMsg(SenBuf,SendLen);
    }
}



//**************************************************************************************************
// 名称         	: MBASCII_Fun04()
// 创建日期     	: 2016-09-01
// 作者        	 	: 庄明群
// 功能         	: Modbus 04功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun04(void)
{
    uint8_t i;
    uint8_t ReadAdrH;//寄存器地址高8位
    uint8_t ReadAdrL;//寄存器地址低8位
    uint32_t Data_Buf;//数据内容
    uint16_t RegisterNum = (uint16_t)RcvBuf[4] * 256 + RcvBuf[5];//寄存器数量

    ReadAdrH = RcvBuf[2];
    ReadAdrL = RcvBuf[3];

    SendLen = 0;
    SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;//从机地址
    SenBuf[SendLen++] = MB_FUNC_READ_INPUT_REGISTER;//功能码
    SenBuf[SendLen++] = RegisterNum * 2;//数据长度

    //如果读取范围溢出
    if(!(((ReadAdrL <= ASCII_INPUT_REG_REGION1_END)
    && ((ReadAdrL + RegisterNum) <= (ASCII_INPUT_REG_REGION1_END + 2))))
    ||(0 == RegisterNum)||(0 != (RegisterNum & 0x01))|| (0 != (ReadAdrL & 0x01))||(ReadAdrH != UsartParam.DeviceAddr))
    {
        //返回非法访问地址
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    for(long k = 0; k < RegisterNum; ReadAdrL += 2, k += 2)
    {
        switch(ReadAdrL)
        {
        case 0x00:
            Data_Buf = (uint32_t)(RunVarParam.LiquidHeight *10.0f + 0.5f);      //液位高度
            break;

        case 0x02:
            Data_Buf = (uint32_t)RunVarParam.ErrStatus;                         //错误状态
            break;

        case 0x04:
            Data_Buf = (uint32_t)UserParam.LiquidType;                          //介质类型
            break;

        case 0x06:                                                              //环境温度
            Data_Buf = (uint32_t)((RunVarParam.TemperInAir +273.1f) * 10.0f + 0.5f);                
            break;
            
        case 0x08:
            Data_Buf = (uint32_t)RunVarParam.CapBak;                            //当前电容值              
            break;

        case 0x0A:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMIN1]);               //介质1零点电容            
            break;            

        case 0x0C:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMAX1]);               //介质1满点电容
            break;  
            
        case 0x0E:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMIN2]);           //介质2零点电容     
            break; 
            
        case 0x10:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMAX2]);           //介质2满点电容              
            break;
            
        case 0x12:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMIN3]);           //介质3零点电容     
            break; 
            
        case 0x14:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMAX3]);           //介质3满点电容              
            break;
            
        case 0x16:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMIN4]);           //介质4零点电容     
            break; 
            
        case 0x18:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMAX4]);           //介质4满点电容              
            break;

        case 0x1A:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMIN5]);           //介质5零点电容     
            break; 
            
        case 0x1C:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMAX5]);           //介质5满点电容              
            break;                  
            
        case 0x1E:
            Data_Buf = RunVarParam.EmptyCap;                                    //空电容              
            break;            

        default:
            Data_Buf = 0;
            break;
        }
        //将数值装载到缓存      
        for(i = 4; i > 0; i--)
        {
            SenBuf[SendLen++] = (uint8_t)(Data_Buf >> ((i - 1) * 8));
        }
    }
    
           MBASCII_SendMsg(SenBuf,SendLen);
 
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun05()
// 创建日期     	: 2016-09-01
// 作者        	 	: 庄明群
// 功能         	: Modbus 05功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun05(void)
{
    uint8_t i, AddrOffset;
    uint8_t ReadAdrH;                                                           //寄存器高8位地址
    uint8_t ReadAdrL;                                                           //寄存器低8位地址

    ReadAdrH = RcvBuf[2];
    ReadAdrL = RcvBuf[3];

    SendLen = 0;
    SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;//从机地址
    SenBuf[SendLen++] = MB_FUNC_WRITE_SINGLE_COIL;//功能码
    //如果操作地址溢出
    if((ReadAdrL < ASCII_SINGLE_COIL_ADDR_BGEIN) || (ReadAdrL > ASCII_SINGLE_COIL_ADDR_END) || (ReadAdrH != UsartParam.DeviceAddr))
    {
        //返回非法数据地址错误
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }
    //如果Flash写禁止且与Flash开关操作无关
    if((ReadAdrL != 0x53) && (ProductParam.EepromWrEn == EEPROM_WR_DISABLE))
    {
        //返回访问设备失败错误
        MBASCII_SendErr(MB_EX_SLAVE_DEVICE_FAILURE);
        return;
    }
    //如果数据内容无效
    if(!(((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00)) || ((RcvBuf[4] == 0x00) && (RcvBuf[5] == 0x00))))
    {
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    switch(ReadAdrL)
    {   
        case 0x50:
            if((RcvBuf[4] == 0x00) && (RcvBuf[5] == 0x00))                      //标定低液位
            {
                ProductParam.CapMin = RunVarParam.CapBak;
                RunVarParam.CalibFlag |= CALIB_CAPMIN_FLAG;
            }
            else if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))                 //标定高液位
            {
                ProductParam.CapMax = RunVarParam.CapBak;
                RunVarParam.CalibFlag |= CALIB_CAPMAX_FLAG;
            }
            if((CALIB_CAPMAX_FLAG | CALIB_CAPMIN_FLAG) == (RunVarParam.CalibFlag & (CALIB_CAPMAX_FLAG | CALIB_CAPMIN_FLAG)))                      //高低液位值都得到了
            {
                if(ProductParam.CapMin < ProductParam.CapMax)
                {
                    AddrOffset = (UserParam.LiquidType - 1) * 16;                //地址随液体类型偏移
                    for(i = 0; i < 4; i ++)
                    {                                                           //放入缓存
                        Cur_Param[CAPMIN1 + AddrOffset + i] = (uint8_t)(ProductParam.CapMin >> (3 - i) * 8);
                        Cur_Param[CAPMAX1 + AddrOffset + i] = (uint8_t)(ProductParam.CapMax >> (3 - i) * 8);
                    }                                                           //写进EEPROM，获得电容量程
                    Eeprom_Write3T_MultiBytes(CAPMIN1 + AddrOffset, (uint8_t const *)&Cur_Param[CAPMIN1 + AddrOffset], 4);
                    Eeprom_Write3T_MultiBytes(CAPMAX1 + AddrOffset, (uint8_t const *)&Cur_Param[CAPMAX1 + AddrOffset], 4);
                    ProductParam.CapRange = ProductParam.CapMax - ProductParam.CapMin;
                    RunVarParam.CalibFlag &= ~(CALIB_CAPMAX_FLAG | CALIB_CAPMIN_FLAG);
                }
                else
                {
                    MBASCII_SendErr(MB_EX_CALIBRATION_ERR);
                    RunVarParam.CalibFlag &= ~(CALIB_CAPMAX_FLAG | CALIB_CAPMIN_FLAG);
                    return;
                }
            }
            break;

        case 0x51:
            //恢复出厂设置
            if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
            {
                ProductParam.EepromWrEn = EEPROM_WR_ENABLE;
                Cur_Param[0] = User_Default_Param[0] + 1;
                Eeprom_Write3T_MultiBytes(RUN_ADDR_BASE, Cur_Param, 1);
                Read_Device_Param();
            }
            break;

        case 0x52:
            if ((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
            {
                UserParam.Freeze = FREEZE_ENABLE;
            }
            else
            {
                UserParam.Freeze = FREEZE_DISABLE;
            }
            break;

        case 0x53:
            if ((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
            {
                ProductParam.EepromWrEn = EEPROM_WR_ENABLE;//Flash写使能
            }
            else
            {
                ProductParam.EepromWrEn = EEPROM_WR_DISABLE;//Flash写禁止
            }
            break;
            
        case 0x54:
            if((RcvBuf[4] == 0x00) && (RcvBuf[5] == 0x00))                      //标定下刻度液位
            {
                ProductParam.CapLow = RunVarParam.CapBak;
                RunVarParam.CalibFlag |= CALIB_CAPLOW_FLAG;
            }
            else if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))                 //标定上刻度液位
            {
                ProductParam.CapHigh = RunVarParam.CapBak;
                RunVarParam.CalibFlag |= CALIB_CAPHIGH_FLAG;
            }
            if((CALIB_CAPLOW_FLAG | CALIB_CAPHIGH_FLAG) == (RunVarParam.CalibFlag & (CALIB_CAPLOW_FLAG | CALIB_CAPHIGH_FLAG)))                      //上/下刻度液位值都得到了
            {
                if(ProductParam.CapLow < ProductParam.CapHigh)
                {
                    AddrOffset = (UserParam.LiquidType - 1) * 16;                //地址随液体类型偏移
                    for(i = 0; i < 4; i ++)
                    {                                                           //放入缓存
                        Cur_Param[CAPLOW1 + AddrOffset + i] = (uint8_t)(ProductParam.CapLow >> (3 - i) * 8);
                        Cur_Param[CAPHIGH1 + AddrOffset + i] = (uint8_t)(ProductParam.CapHigh >> (3 - i) * 8);
                    }                                                           //写进EEPROM，获得电容量程
                    Eeprom_Write3T_MultiBytes(CAPLOW1 + AddrOffset, (uint8_t const *)&Cur_Param[CAPLOW1 + AddrOffset], 4);
                    Eeprom_Write3T_MultiBytes(CAPHIGH1 + AddrOffset, (uint8_t const *)&Cur_Param[CAPHIGH1 + AddrOffset], 4);
                    RunVarParam.CalibFlag &= ~(CALIB_CAPLOW_FLAG | CALIB_CAPHIGH_FLAG);
                }
                else
                {
                    MBASCII_SendErr(MB_EX_CALIBRATION_ERR);
                    RunVarParam.CalibFlag &= ~(CALIB_CAPLOW_FLAG | CALIB_CAPHIGH_FLAG);
                    return;
                }
            }
            break;

        default:
            break;
    }
    //Modbus发送操作成功
    MBASCII_SendMsg(RcvBuf, 6);
}




//**************************************************************************************************
// 名称         	: MBASCII_Fun10()
// 创建日期     	: 2016-09-01
// 作者        	 	: 庄明群
// 功能         	: Modbus 10功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun10(void)
{
    uint8_t i = 0;
    uint8_t ReadAdrH;
    uint8_t ReadAdrL;
    uint16_t index = 0;
    uint16_t AddrOffset;
    uint16_t RegisterNum = (uint16_t)RcvBuf[4] * 256 + RcvBuf[5];

    ReadAdrH = RcvBuf[2];
    ReadAdrL = RcvBuf[3];

    SendLen = 0;
    SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;
    SenBuf[SendLen++] = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
    SenBuf[SendLen++] = RegisterNum * 2;

        //如果读取范围溢出
    if (!(((ReadAdrL >= ASCII_MUL_REG_REGION1_BGEIN) && (ReadAdrL <= ASCII_MUL_REG_REGION1_END)
            && (ReadAdrL + RegisterNum <= (ASCII_MUL_REG_REGION1_END + 1))))
        ||(0 == RegisterNum)||((RegisterNum * 2) != RcvBuf[6])||((ReadAdrH != UsartParam.DeviceAddr) && (ReadAdrH != 0)))
    	{
		MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
	       return;
	}
        ProductParam.EepromWrEn = EEPROM_WR_ENABLE;
        
	for (long k = 0; k < RegisterNum; ReadAdrL++, k++)
	{        
          
          switch(ReadAdrL)
          {
          case 0x30://设备地址
            if((RcvBuf[8 + index] < 0x41)||(RcvBuf[8 + index] > 0x42))
            {
              UsartParam.DeviceAddr = 0x41;
            }
            else
            {
              UsartParam.DeviceAddr = RcvBuf[8 + index];
            }
            Eeprom_Write3T_MultiBytes(DEVICE_ADDR, &UsartParam.DeviceAddr, 1);
            break;
            
          case 0x31://波特率
              if(0x06 > RcvBuf[8 + index])
              {
                UsartParam.BaudRate = RcvBuf[8 + index];
                Eeprom_Write3T_MultiBytes(BAUDRATE, &UsartParam.BaudRate, 1);
              }
            break;
            
          case 0x32://奇偶校验
              if(0x03 > RcvBuf[8 + index])
              {
                UsartParam.Parity = RcvBuf[8 + index];
                Eeprom_Write3T_MultiBytes(PARITY, &UsartParam.Parity, 1);
              }
            break;
            
          case 0x33:
              break;
            
          case 0x34://补偿使能
              if(2 > RcvBuf[8 + index])
              {
                ProductParam.CompenEn = RcvBuf[8 + index];
                Eeprom_Write3T_MultiBytes(COMPENSATE, &ProductParam.CompenEn, 1);
              }
            break;
            
          case 0x35://滤波系数
              if(0x0A > RcvBuf[8 + index])
              {
                UserParam.Filterfactor = RcvBuf[8 + index];
                Eeprom_Write3T_MultiBytes(FILTER, &UserParam.Filterfactor, 1);
                SwitchCurFilter(UserParam.Filterfactor);
              }
            break;
            
          case 0x36://自动上传周期
            if((RcvBuf[8 + index] >= 1) && (RcvBuf[8 + index] <= 4))
            {
                UsartParam.AutoUpload = RcvBuf[8 + index];
                Eeprom_Write3T_MultiBytes(AUTO_UPLOAD, &UsartParam.AutoUpload, 1);
            }           
            break;
            
          case 0x37:
            UserParam.Correct_K = (((uint16_t)RcvBuf[7 + index] << 8) + RcvBuf[8 + index]) / 100.0f;    
            Eeprom_Write3T_MultiBytes(CORRECT_K, &RcvBuf[7 + index], 2);
            break;
            
          case 0x38:
             UserParam.Correct_B = (((uint16_t)RcvBuf[7 + index] << 8) + RcvBuf[8 + index]);
             Eeprom_Write3T_MultiBytes(CORRECT_B, &RcvBuf[7 + index], 2);
            break;
            
          case 0x39:
            break;
            
          case 0x3A:
            UserParam.Temper_K1 = (((uint16_t)RcvBuf[7 + index] << 8) + RcvBuf[8 + index]) / 100.0f;
            Eeprom_Write3T_MultiBytes(TEMPER_K1, &RcvBuf[7 + index], 2);
            break; 
            
          case 0x3B:
            UserParam.Temper_B1 = (((uint16_t)RcvBuf[7 + index] << 8) + RcvBuf[8 + index]) / 100.0f;
            Eeprom_Write3T_MultiBytes(TEMPER_B1, &RcvBuf[7 + index], 2);
            break;
            
          case 0x3C:
            break;
            
          case 0x3D:
            break;
            
          case 0x3E:
            break;             
            
          case 0x3F:
            ProductParam.HeightRange = (((uint16_t)RcvBuf[7 + index] << 8) + RcvBuf[8 + index]);
            Eeprom_Write3T_MultiBytes(HEIGHTRANGE, &RcvBuf[7 + index], 2);
            break; 
            
          case 0x40:                    
            break;
            
          case 0x41:
            if((RcvBuf[8 + index] >= 1) && (RcvBuf[8 + index] <= 5))          
            {
                UserParam.LiquidType = RcvBuf[8 + index];
                Eeprom_Write3T_MultiBytes(LIQUIDTYPE, &UserParam.LiquidType, 1);
                UserParam.FilterStart = 0;                                      //发送了修改液体类型，要重新开始滤波统计
                AddrOffset = (UserParam.LiquidType - 1) * 8;                    //要读出EEPROM中的值来计算液位              
                Eeprom_Read_MultiBytes(CAPMIN1 + AddrOffset, (uint8_t *)&Cur_Param[CAPMIN1 + AddrOffset], 8);
                ProductParam.CapMin = HexToUlong(&Cur_Param[CAPMIN1 + AddrOffset]);
                ProductParam.CapMax = HexToUlong(&Cur_Param[CAPMAX1 + AddrOffset]);
                ProductParam.CapRange = ProductParam.CapMax - ProductParam.CapMin;
            }
            break;                
            
          default:
            break;
          }
          
          index += 2;
        }
        
        ProductParam.EepromWrEn = EEPROM_WR_DISABLE;
        
        for(i = 0; i < 6; i++)
        {
          SenBuf[i] = RcvBuf[i];
        }
        
        MBASCII_SendMsg(RcvBuf,6);
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun25()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: Modbus 25功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun25(void)
{
    uint8_t ReadAdrH;
    uint8_t ReadAdrL;
    uint8_t i;
    ReadAdrH = RcvBuf[2];
    ReadAdrL = RcvBuf[3];
    
    SendLen = 0;
    SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;
    SenBuf[SendLen++] = 0x25;

    //如果操作范围溢出

    if ((ReadAdrL > ASCII_SPECICAL_COIL_REGION1_END)
        && ((ReadAdrL < ASCII_SPECICAL_COIL_REGION2_BGEIN) || (ReadAdrL > ASCII_SPECICAL_COIL_REGION2_END))
        ||(ReadAdrH != UsartParam.DeviceAddr))
    {
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    if (!(((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
            || ((RcvBuf[4] == 0x00) && (RcvBuf[5] == 0x00))))
    {
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    if(ProductParam.EepromWrEn == EEPROM_WR_DISABLE)
    {
        MBASCII_SendErr(MB_EX_SLAVE_DEVICE_FAILURE);
        return;
    }

    switch(ReadAdrL)
    {
    case 0x00:
      if((RcvBuf[4] == 0x00) && (RcvBuf[5] == 0x00))  
      {
        RunVarParam.EmptyCap = RunVarParam.CapBak;
        for(i = 0; i < 4; i ++)
        {                                                           //放入缓存
            Cur_Param[CAP_EMPTY + i] = (uint8_t)(RunVarParam.EmptyCap >> (3 - i) * 8);
        }                                                           //写进EEPROM，获得电容量程
        Eeprom_Write3T_MultiBytes(CAP_EMPTY, (uint8_t const *)&Cur_Param[CAP_EMPTY], 4);        
      }
      break;
      
    case 0x01:
    //电容DA标定上下刻度线
    if(DAOUTCLIB_ENABLE == ProductParam.CapDA_ClibEn)
    {
      if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
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
      
    case 0x02:
    //电容DA标定使能
        if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
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
      
    case 0x03:
          //电容DA标定零点满量程
        if(DAOUTCLIB_ENABLE == ProductParam.CapDA_ClibEn)
        {
          if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
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
      
 case 0x04:
    //温度标定使能
    if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
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
    
  case 0x05:
    //温度标定
    if(DAOUTCLIB_ENABLE == ProductParam.TempDA_ClibEn)
    {
      if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
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
    
    case 0x06:
        break;
    
  case 0x07:
    //电容AD标定使能
    if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
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
    
  case 0x08:
    //电容AD标定上下刻度线
    if(ADCLIB_ENABLE == ProductParam.CapAD_ClibEn)
    {
      if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
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
    
  case 0x09:
    //电容AD标定零点满量程
    if(ADCLIB_ENABLE == ProductParam.CapAD_ClibEn)
    {
      if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))
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
    
    MBASCII_SendMsg(RcvBuf, 6);
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun26()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: Modbus 26功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun26(void)
{
    uint8_t i;
    uint8_t ReadAdrH;
    uint8_t ReadAdrL;
    uint32_t DataBuf;
    float fBuf = 0.0f;
    
    ReadAdrH = RcvBuf[2];
    ReadAdrL = RcvBuf[3];
    
    uint16_t Register_Num = (uint16_t)RcvBuf[4] * 256 + RcvBuf[5];
    
    SendLen = 0;
    SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;
    SenBuf[SendLen++] = 0x26;
    SenBuf[SendLen++] = Register_Num * 2;      //数据长度

    if (!(((ReadAdrL <= ASCII_DB_HOLD_REG_REGION1_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION1_END + 2))
               || ((ReadAdrL >= ASCII_DB_HOLD_REG_REGION2_BEGIN) && (ReadAdrL <= ASCII_DB_HOLD_REG_REGION2_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION2_END + 2))
               || ((ReadAdrL >= ASCII_DB_HOLD_REG_REGION3_BEGIN) &&(ReadAdrL <= ASCII_DB_HOLD_REG_REGION3_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION3_END + 2)))
               || (0 == Register_Num) || (0 != (Register_Num & 0x01)) || (0 != (ReadAdrL & 0x01)) || (ReadAdrH != UsartParam.DeviceAddr))
    {
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }
    
    for (long k = 0; k < Register_Num; ReadAdrL += 2, k += 2)
    {
        switch(ReadAdrL)
        {
        case 0x80:
          //空
          DataBuf = 0;
          break;
        
        case 0x82:
          //电容零点
          fBuf = (float)ProductParam.CapMin;
          DataBuf = *(uint32_t *)&fBuf;
          break;
        
        case 0x84:
          //电容满量程
          fBuf = (float)ProductParam.CapMax;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0x86:
          //空
          DataBuf = 0;
          break;
        
        case 0x90:
          //环境温度修正系数K1
          fBuf = UserParam.Temper_K1;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0x92:
          //环境温度修正系数B1
          fBuf = UserParam.Temper_B1;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0x94:
          break;
          
        case 0x96:
          break;
          
        case 0xB6:
          //电容DA标定值
          fBuf = (float)RunVarParam.CapDA_ClibValue;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xB8:
          //环境温度DA标定值
          fBuf = (float)RunVarParam.TempDA_ClibValue;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xBA:
          //电容AD标定值
          fBuf = (float)RunVarParam.CapAD_ClibValue;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xBC:
          //电容下刻度标定高度值
          fBuf = (float)ProductParam.CapLowHeight;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xBE:
          //电容上刻度标定高度值
          fBuf = (float)ProductParam.CapHighHeight;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xC0:
          //电容下刻度标定电容值
          fBuf = (float)ProductParam.CapLow;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xC2:
          //电容上刻度标定电容值
          fBuf = (float)ProductParam.CapHigh;
          DataBuf = *(uint32_t *)&fBuf;
          break;                  
          
        default:
          break;
        }
        
        for(i = 4; i > 0; i--)
        {
          SenBuf[SendLen++] = (uint8_t)(DataBuf >> ((i - 1)*8));
        }
    }

    MBASCII_SendMsg(SenBuf, SendLen);
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun27()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: Modbus 27功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun27(void)
{
    uint8_t AddrOffset;
    uint8_t ReadAdrH;
    uint8_t ReadAdrL;
    
    uint16_t Register_Num = (uint16_t)RcvBuf[4] * 256 + RcvBuf[5];
    uint16_t DataBuf;
    uint32_t index = 0;
    
    float fbuf = 0.0f;

    ReadAdrH = RcvBuf[2];
    ReadAdrL = RcvBuf[3];
    
    SendLen = 0;
    SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;
    SenBuf[SendLen++] = 0x27;
    SenBuf[SendLen++] = Register_Num * 2;

    if (!(((ReadAdrL <= ASCII_DB_HOLD_REG_REGION1_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION1_END + 2))
               || ((ReadAdrL >= ASCII_DB_HOLD_REG_REGION2_BEGIN) && (ReadAdrL <= ASCII_DB_HOLD_REG_REGION2_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION2_END + 2))
               || ((ReadAdrL >= ASCII_DB_HOLD_REG_REGION3_BEGIN) && (ReadAdrL <= ASCII_DB_HOLD_REG_REGION3_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION3_END + 2)))
             ||(0 != (Register_Num & 0x01))||(0 == Register_Num)||(0 != (ReadAdrL & 0x01))||((Register_Num * 2) != RcvBuf[6])||(ReadAdrH != UsartParam.DeviceAddr))
    {
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    if (ProductParam.EepromWrEn == EEPROM_WR_DISABLE)
    {
        MBASCII_SendErr(MB_EX_SLAVE_DEVICE_FAILURE);
        return;
    }

    for (long k = 0; k < Register_Num; ReadAdrL += 2, k += 2)
    {
      switch(ReadAdrL)
      {
    case 0x80://保留
      //fbuf = HexToFloat(&RcvBuf[7 + index]);
      //ProductParam.HeightRange = (uint16_t)(fbuf * 100);
      //Cur_Param[HEIGHTRANGE] = (unsigned char)(ProductParam.HeightRange >> 8);
      //Cur_Param[HEIGHTRANGE + 1] = (unsigned char)ProductParam.HeightRange;
      //Eeprom_Write3T_MultiBytes(HEIGHTRANGE, (uint8_t const *)&Cur_Param[HEIGHTRANGE], 2);
      break;
      
    case 0x82:
      //空
      break;
      
    case 0x84:
      //空
      break;
      
    case 0x90:
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
      
    case 0x92:
      //环境温度修正系数B1
      UserParam.Temper_B1 = HexToFloat(&RcvBuf[7 + index]);
      DataBuf = (uint16_t)(UserParam.Temper_B1 * 100);
      Cur_Param[TEMPER_B1] = (unsigned char)(DataBuf >> 8);
      Cur_Param[TEMPER_B1 + 1] = (unsigned char)DataBuf;
      Eeprom_Write3T_MultiBytes(TEMPER_B1, (uint8_t const *)&Cur_Param[TEMPER_B1], 2);
      break;
      
    case 0x94:
      break;
      
    case 0x96:
      break;
      
    case 0xB6:
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
      
    case 0xB8:
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
      
    case 0xBA:
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
      
    case 0xBC:
      //电容下刻度标定高度
        fbuf = HexToFloat(&RcvBuf[7 + index]);
        ProductParam.CapLowHeight = (uint16_t)fbuf;
        AddrOffset = (UserParam.LiquidType - 1) * 4;                //地址随液体类型偏移
        Cur_Param[CAPLOWHEIGHT1 + AddrOffset] = (uint16_t)fbuf >> 8;
        Cur_Param[CAPLOWHEIGHT1 + AddrOffset + 1] = (uint16_t)fbuf & 0xFF;
        Eeprom_Write3T_MultiBytes(CAPLOWHEIGHT1 + AddrOffset, 
                                  (uint8_t const *)&Cur_Param[CAPLOWHEIGHT1 + AddrOffset], 2);
      break;
      
    case 0xBE:
      //电容上刻度标定高度
        fbuf = HexToFloat(&RcvBuf[7 + index]);
        ProductParam.CapHighHeight = (uint16_t)fbuf;
        AddrOffset = (UserParam.LiquidType - 1) * 4;                //地址随液体类型偏移
        Cur_Param[CAPHIGHHEIGHT1 + AddrOffset] = (uint16_t)fbuf >> 8;
        Cur_Param[CAPHIGHHEIGHT1 + AddrOffset + 1] = (uint16_t)fbuf & 0xFF;
        Eeprom_Write3T_MultiBytes(CAPHIGHHEIGHT1 + AddrOffset, 
                                  (uint8_t const *)&Cur_Param[CAPHIGHHEIGHT1 + AddrOffset], 2);
      break;
      
    default:
      break;
      
      
    }
    index += 4;
    }

    MBASCII_SendMsg(RcvBuf, 6);
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun2A()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: 写版本信息
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun2A(void)
{
    uint8_t i,j,objlen;
    uint8_t ReadAdr = RcvBuf[3];
    uint8_t Register_Num = RcvBuf[5];
    SendLen = 0;

    SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;
    SenBuf[SendLen++] = 0x2A;
    SenBuf[SendLen++] = RcvBuf[2];
    if(!((RcvBuf[2] == UsartParam.DeviceAddr)
       &&(ReadAdr >= ASCII_PRODUCTION_INF_BEGIN)
       &&(ReadAdr <= ASCII_PRODUCTION_INF_END)
       &&((ReadAdr + Register_Num) <= (ASCII_PRODUCTION_INF_END + 1))
       &&(0 != Register_Num)&&(0x00 == RcvBuf[4])))
    {
      MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
      return;
    }
    j = 6;
    ProductParam.EepromWrEn = EEPROM_WR_ENABLE;
    for(i = 0; i < Register_Num; i++)
    {
      switch(ReadAdr)
      {
      //机构名称
    case 0xE0:
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
    case 0xE1:
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
    case 0xE2:
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
    case 0xE3:
      objlen = RcvBuf[j] + 1;
      //                                                                                                    
      j += objlen;
      break;
    //设备ID    
    case 0xE4:
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
    case 0xE5:
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
    }
    ProductParam.EepromWrEn = EEPROM_WR_DISABLE;
    
    MBASCII_SendMsg(RcvBuf, 6);
}


//**************************************************************************************************
// 名称         	: MBASCII_Fun2B()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: 读版本信息
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun2B(void)
{
  uint8_t i,objlen;
  uint8_t ReadAdr = RcvBuf[3];
  uint8_t Register_Num = RcvBuf[5];
  uint8_t SendLen2B = 0;

  SenBuf[SendLen2B++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;
  SenBuf[SendLen2B++] = 0x2B;
  SenBuf[SendLen2B++] = RcvBuf[4];
  SenBuf[SendLen2B++] = RcvBuf[5];
  if(!((RcvBuf[2] == UsartParam.DeviceAddr)
       &&(ReadAdr >= ASCII_PRODUCTION_INF_BEGIN)
       &&(ReadAdr <= ASCII_PRODUCTION_INF_END)
       &&((ReadAdr + Register_Num) <= (ASCII_PRODUCTION_INF_END + 1))
       &&(0 != Register_Num)&&(0x00 == RcvBuf[4])))
  {
    MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
    return;
  }
  for(i = 0; i < Register_Num; i++)
  {
    switch(ReadAdr)
    {
        
    //机构名称  
    case 0x00E0:
      SenBuf[SendLen2B++] = 0xE0;
      objlen = Eeprom_Read_OneByte(ORGANIZATION);                                                       
      SenBuf[SendLen2B++] = objlen;
      if((objlen > (USART1_MAX_DATALEN/3))||(0 == objlen))
      {
        SenBuf[SendLen2B - 1] = 1;
        SenBuf[SendLen2B++] = 0;
        break;
      }
      Eeprom_Read_MultiBytes((ORGANIZATION + 1), &SenBuf[SendLen2B], objlen);
      SendLen2B += objlen;
      break;
    //产品代号  
    case 0x00E1:
      SenBuf[SendLen2B++] = 0xE1;
      objlen = Eeprom_Read_OneByte(PRODUCTION);                                                  
      SenBuf[SendLen2B++] = objlen;
      if((objlen > (USART1_MAX_DATALEN/3))||(0 == objlen))
      {
        SenBuf[SendLen2B - 1] = 1;
        SenBuf[SendLen2B++] = 0;
        break;
      }
      Eeprom_Read_MultiBytes((PRODUCTION + 1), &SenBuf[SendLen2B], objlen);
      SendLen2B += objlen;
      break;
    //硬件版本  
    case 0x00E2:
      SenBuf[SendLen2B++] = 0xE2;
      objlen = Eeprom_Read_OneByte(HARDWAREVER);                                                     
      SenBuf[SendLen2B++] = objlen;
      if((objlen > (USART1_MAX_DATALEN/3))||(0 == objlen))
      {
        SenBuf[SendLen2B - 1] = 1;
        SenBuf[SendLen2B++] = 0;
        break;
      }
      Eeprom_Read_MultiBytes((HARDWAREVER + 1), &SenBuf[SendLen2B], objlen);
      SendLen2B += objlen;
      break;
    //软件版本 
    case 0x00E3:
      SenBuf[SendLen2B++] = 0xE3;                                                     
      SenBuf[SendLen2B++] = 0x07;
      
      for(objlen = 0; objlen < 7; objlen++)                                                        
      {
        SenBuf[SendLen2B++] = SensorSoftVersion[objlen + 1];
      }
      break;
    //设备ID  
    case 0x00E4:
      SenBuf[SendLen2B++] = 0xE4;
      objlen = Eeprom_Read_OneByte(DEVICENUM);                                                        
      SenBuf[SendLen2B++] = objlen;
      if((objlen > (USART1_MAX_DATALEN/3))||(0 == objlen))
      {
        SenBuf[SendLen2B - 1] = 1;
        SenBuf[SendLen2B++] = 0;
        break;
      }
      Eeprom_Read_MultiBytes((DEVICENUM + 1), &SenBuf[SendLen2B], objlen);
      SendLen2B += objlen;
      break;
    //客户编码  
    case 0x00E5:
      SenBuf[SendLen2B++] = 0xE5;
      objlen = Eeprom_Read_OneByte(CUSTOMER);                                                  
      SenBuf[SendLen2B++] = objlen;
      if((objlen > (USART1_MAX_DATALEN/3))||(0 == objlen))
      {
        SenBuf[SendLen2B - 1] = 1;
        SenBuf[SendLen2B++] = 0;
        break;
      }
      Eeprom_Read_MultiBytes((CUSTOMER + 1), &SenBuf[SendLen2B], objlen);
      SendLen2B += objlen;
      break;
      
    default:
      break;
    } 
    ReadAdr++;
  }
  
  MBASCII_SendMsg(SenBuf, SendLen2B);

}

//**************************************************************************************************
// 名称         	: MBASCII_Fun41()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: 在线升级功能
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************
#define ONLINEUPGRADE 2047
void MBASCII_Fun41(void)
{
  uint16_t ReadAdr = (uint16_t)RcvBuf[2] * 256 + RcvBuf[3];
  uint16_t DataLen = (uint16_t)RcvBuf[4] * 256 + RcvBuf[5];
  uint8_t Onlineflg;
  
  SendLen = 0;
  SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;
  SenBuf[SendLen++] = 0x41;
  SenBuf[SendLen++] = DataLen;
  
  if(0x0001 != ReadAdr)
  {
    MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
    return;
  }
  RcvBuf[5] = 0x01;
  
  if(0x0000 != DataLen)
  {
    RcvBuf[6] = 0x01;
    MBASCII_SendMsg(RcvBuf, 7);
    return;
  }
  
  Onlineflg = 0x0C;
  ProductParam.EepromWrEn = EEPROM_WR_ENABLE;
  Eeprom_Write_OneByte(ONLINEUPGRADE, 0x0C);
  //Eeprom_Write_MultiBytes(DEVICE_ADDR, &UsartParam.DeviceAddr, 1);
  ProductParam.EepromWrEn = EEPROM_WR_DISABLE;
  Onlineflg = Eeprom_Read_OneByte(ONLINEUPGRADE);
  if(0x0C == Onlineflg)
  {
    RcvBuf[6] = 0x00;
    MBASCII_SendMsg(RcvBuf, 7);
    Delay_us(100000);
    NVIC_SystemReset();
  }
}
//**************************************************************************************************
// 名称         	: MBASCII_Function()
// 创建日期   	        : 2016-09-01
// 作者         	: 庄明群
// 功能         	: ModbusASCII码解析
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Function(void)
{
    uint8_t RecvLen = 0;
    if(2 == MODBUS_ASCII_RecvData(RcvBuf, &RecvLen))//校验错误
    {
      return;
    }
    if(RecvLen <= 0)
    {
      return;
    }
    else if((UsartParam.DeviceAddr != MBASCII_GetSlaveAddr(RcvBuf))&&
            (MB_ADDRESS_BROADCAST != MBASCII_GetSlaveAddr(RcvBuf)))
    {
      return;
    }
    else
    {
        switch (MBASCII_GetFunCode(RcvBuf))
        {
        case MB_FUNC_READ_HOLDING_REGISTER:	//读单个寄存器0x03
          MBASCII_Fun03();
          break;
            
        case MB_FUNC_READ_INPUT_REGISTER:		//读两个寄存器0x04
          MBASCII_Fun04();
          break;
            
        case MB_FUNC_WRITE_SINGLE_COIL:		//写单个线圈
          MBASCII_Fun05();
          break;
            
        case MB_FUNC_WRITE_MULTIPLE_REGISTERS://写多个寄存器0x10
          if((RecvLen - 7) != RcvBuf[6])
          {
            return ;
          }
          MBASCII_Fun10();
          break;
          
        case 0x25:
          MBASCII_Fun25();
          break;
          
        case 0x26:
          MBASCII_Fun26();
          break;
          
        case 0x27:
          MBASCII_Fun27();
          break;
          
        case 0x2A:
          MBASCII_Fun2A();
          break;
          
        case 0x2B:
          MBASCII_Fun2B();
          break;
        case 0x41:
          MBASCII_Fun41();
          break;
          
	default:
          SendLen = 0;
          SenBuf[SendLen++] = MBASCII_GetSlaveAddr(RcvBuf);
          SenBuf[SendLen++] = 0x80 | (uint8_t)MBASCII_GetFunCode(RcvBuf);
          SenBuf[SendLen++] = MB_EX_ILLEGAL_FUNCTION;
          MBASCII_SendMsg(SenBuf, SendLen);
	  break;
        }
    }  
}




//****************************************************************************************************************************************************************
// 名称               :ModbusAutoUpload()
// 创建日期           :2017-05-13
// 作者               :MMX
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
    SenBuf[SenBufLen++] = 0x14;

    fbuf = (float)(RunVarParam.LiquidHeight * 10.0f + 0.5f);                    //液位高度
    DataBuf = (uint32_t)fbuf;
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }

    DataBuf = (uint32_t)RunVarParam.ErrStatus;                                  //错误状态
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }
  
    DataBuf = (uint32_t)UserParam.LiquidType;                                   //液体类型
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }
    
    fbuf = (float)RunVarParam.TemperInAir;                                      //环境温度
    DataBuf = (uint32_t)((fbuf + 273.1f) * 10.0f + 0.5f);
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }
    
    DataBuf = RunVarParam.CapBak;                                               //原始电容值
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }    
      if(cnt<4)
      {
          //Modbus发送消息      
          if( RunVarParam.CapBak <= 0x00FF0000 )    //复位3次以内，不发送满值
          {
             MBASCII_SendMsg(SenBuf,SenBufLen);
          }
             if(DataError >=8)
            {   
              DataError = 0;                           
              cnt+=1;    //复位计数加1
              ProductParam.EepromWrEn = EEPROM_WR_ENABLE;//EE使能
                Eeprom_Write_OneByte(RESET_CNT, cnt);   //写次数到EE
              ProductParam.EepromWrEn = EEPROM_WR_DISABLE;
              
              //系统复位
              NVIC_SystemReset();
            }  
      }
      else
      {
 
          if(cnt>5) 
          {
            cnt = 0;
            ProductParam.EepromWrEn = EEPROM_WR_ENABLE;
            Eeprom_Write_OneByte(RESET_CNT, cnt);
            ProductParam.EepromWrEn = EEPROM_WR_DISABLE;
          }
        MBASCII_SendMsg(SenBuf,SenBufLen);     //复位3次以上，发送满值
               cnt+=1;            
        //系统复位
             // NVIC_SystemReset();
      }
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
    if((1 != UsartParam.AutoUpload) && (UsartParam.AutoUploadCn >= AutoUploadTimeConvert(UsartParam.AutoUpload) * 10))
    {   
        if(LiquidStableFlag == 1)                                               //每秒液位和滤波液位相差不大时开始上传数据（只针对上电那段时间）
        {
            ModbusAutoUpload();                                                 //Modbus帧自动上传
        }
        UsartParam.AutoUploadCn = 0;
    }
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
