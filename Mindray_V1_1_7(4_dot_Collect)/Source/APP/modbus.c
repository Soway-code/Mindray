#include "stm32l0_eeprom_app.h"
#include "modbus.h"
#include "Softspi.h"

//uint32_t Calib_CapMin;                                                        //�궨�������ֵ
//uint32_t Calib_CapMax;                                                        //�궨����������
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

extern uint8_t LiquidStableFlag;                                                //Һλ�ȶ���־
extern uint8_t RcvBuf[USART1_MAX_DATALEN];                                      //���ڽ��ջ���
extern uint8_t SenBuf[USART1_MAX_DATALEN];
extern uint8_t const SensorSoftVersion[8];                                      //����汾
extern uint8_t const User_Default_Param[PRO_DEFAULT_LEN];                       //�û�ȱʡ����
uint8_t SendLen;                                                                //�������ݳ���

extern   uint8_t  cnt ;
extern uint8_t  DataError;
//**************************************************************************************************
// ����         	: MBASCII_GetSlaveAddr()
// ��������   	        : 2015-10-29
// ����         	: wang
// ����         	:����豸��ַ
// �������     	:���յĻ�������(u8 *u8Msg)
// �������     	: �豸��ַ
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
//**************************************************************************************************

uint32_t MBASCII_GetSlaveAddr(uint8_t *u8Msg)
{
    return(u8Msg[0]);
}

//**************************************************************************************************
// ����         	: MBASCII_GetFunCode()
// ��������   	        : 2015-10-29
// ����         	: wang
// ����         	:��ù�����
// �������     	:���յĻ�������(u8 *u8Msg)
// �������     	: ������
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
//**************************************************************************************************

uint32_t MBASCII_GetFunCode(uint8_t *u8Msg)
{
    return(u8Msg[1]);
}

//**************************************************************************************************
// ����         	: MBASCII_SendMsg()
// ��������   	        : 2015-10-29
// ����         	: wang
// ����         	:����ָ��
// �������     	:���ͻ�������(uint8_t *u8Msg),���ݳ���(uint32_t u8MsgLen)
// �������     	: �豸��ַ
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
//**************************************************************************************************

void MBASCII_SendMsg(uint8_t *u8Msg, uint32_t u8MsgLen)
{
    if(MB_ADDRESS_BROADCAST != MBASCII_GetSlaveAddr(u8Msg))
    {
        MODBUS_ASCII_SendData(u8Msg, u8MsgLen );	
    }
}


//**************************************************************************************************
// ����         	: MBASCII_SendErr()
// ��������     	: 2015-07-13
// ����        	 	: ��С��
// ����         	: ����MODBUS ����
// �������     	: u8 ErrCode
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
//**************************************************************************************************

void MBASCII_SendErr(uint8_t ErrCode)
{
    SendLen = 1;
    SenBuf[SendLen++] |= 0x80;
    SenBuf[SendLen++] = ErrCode;

    MBASCII_SendMsg(SenBuf,SendLen);
}

//**************************************************************************************************
// ����         	: MBASCII_SendMsg_NoLimit()
// ��������   	        : 2015-10-29
// ����         	: wang
// ����         	:����ָ��
// �������     	:���ͻ�������(uint8_t *u8Msg),���ݳ���(uint32_t u8MsgLen)
// �������     	: �豸��ַ
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
//**************************************************************************************************

void MBASCII_SendMsg_NoLimit(uint8_t *u8Msg, uint32_t u8MsgLen)
{
    if (UserParam.Freeze == FREEZE_DISABLE)
    {
        MODBUS_ASCII_SendData(u8Msg, u8MsgLen );
    }
}

//**************************************************************************************************
// ����         	: MBASCII_Fun03()
// ��������     	: 2016-09-01
// ����        	 	: ׯ��Ⱥ
// ����         	: Modbus 03���ܴ���
// �������     	: ��
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
//**************************************************************************************************

void MBASCII_Fun03(void)
{
    uint8_t i;
    //uint8_t Addrflg;                                                          //�Ĵ�����ַ��־ ���ڱ�ʶֻ��һ���ӻ���ַ��Ϣʱ��������֡�Ĵӻ���ַ
    uint8_t ReadAdrH;                                                           //�Ĵ�����ַ��8λ
    uint8_t ReadAdrL;                                                           //�Ĵ�����ַ��8λ
    uint16_t Data_Buf;                                                          //��������
    uint16_t RegisterNum = (uint16_t)RcvBuf[4]*256 + RcvBuf[5];                 //�Ĵ�������

    ReadAdrH = RcvBuf[2];
    ReadAdrL = RcvBuf[3];

    SendLen = 0;
    SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;
    SenBuf[SendLen++] = MB_FUNC_READ_HOLDING_REGISTER;
    SenBuf[SendLen++] = RegisterNum * 2;	   
   
    //�жϷ��ʼĴ�����ַ�Ƿ�����Ч��Χ��
    if(!(((ReadAdrL >= ASCII_HOLDING_REG_REGION1_BGEIN) && (ReadAdrL <= ASCII_HOLDING_REG_REGION1_END)          //�򿪹㲥��ַ���豸��ַ
    && (ReadAdrL + RegisterNum <= (ASCII_HOLDING_REG_REGION1_END + 1))&&((ReadAdrH == UsartParam.DeviceAddr) || (ReadAdrH == MB_ADDRESS_BROADCAST)))
    ||((RegisterNum * 2 == RcvBuf[6])&&(ReadAdrH == UsartParam.DeviceAddr))
    ||((ASCII_HOLDING_REG_REGION1_BGEIN == ReadAdrL)&&(1 == RegisterNum)))||(0 == RegisterNum))
    {
        //���طǷ����ݵ�ַ
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    for(long k = 0; k <  RegisterNum; ReadAdrL++, k++)
    {
        switch(ReadAdrL)
        {
        case 0x30://�ӻ���ַ
            Data_Buf = UsartParam.DeviceAddr;
            break;

        case 0x31://������
            Data_Buf = UsartParam.BaudRate;
            break;

        case 0x32://��żУ��
            Data_Buf = UsartParam.Parity;
            break;
            
        case 0x33:
            Data_Buf = 0;
            break;            

        case 0x34://����ʹ��
            Data_Buf = ProductParam.CompenEn;
            break;

        case 0x35://�˲��ȼ�
            Data_Buf = UserParam.Filterfactor;
            break;

        case 0x36://�Զ��ϴ�����
            Data_Buf = UsartParam.AutoUpload;
            break;

        case 0x37://��������ϵ��K
            Data_Buf = (uint32_t)(UserParam.Correct_K * 100);
            break;

        case 0x38://��������ϵ��B
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

        case 0x41://��ҩ����
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
// ����         	: MBASCII_Fun04()
// ��������     	: 2016-09-01
// ����        	 	: ׯ��Ⱥ
// ����         	: Modbus 04���ܴ���
// �������     	: ��
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
//**************************************************************************************************

void MBASCII_Fun04(void)
{
    uint8_t i;
    uint8_t ReadAdrH;//�Ĵ�����ַ��8λ
    uint8_t ReadAdrL;//�Ĵ�����ַ��8λ
    uint32_t Data_Buf;//��������
    uint16_t RegisterNum = (uint16_t)RcvBuf[4] * 256 + RcvBuf[5];//�Ĵ�������

    ReadAdrH = RcvBuf[2];
    ReadAdrL = RcvBuf[3];

    SendLen = 0;
    SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;//�ӻ���ַ
    SenBuf[SendLen++] = MB_FUNC_READ_INPUT_REGISTER;//������
    SenBuf[SendLen++] = RegisterNum * 2;//���ݳ���

    //�����ȡ��Χ���
    if(!(((ReadAdrL <= ASCII_INPUT_REG_REGION1_END)
    && ((ReadAdrL + RegisterNum) <= (ASCII_INPUT_REG_REGION1_END + 2))))
    ||(0 == RegisterNum)||(0 != (RegisterNum & 0x01))|| (0 != (ReadAdrL & 0x01))||(ReadAdrH != UsartParam.DeviceAddr))
    {
        //���طǷ����ʵ�ַ
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    for(long k = 0; k < RegisterNum; ReadAdrL += 2, k += 2)
    {
        switch(ReadAdrL)
        {
        case 0x00:
            Data_Buf = (uint32_t)(RunVarParam.LiquidHeight *10.0f + 0.5f);      //Һλ�߶�
            break;

        case 0x02:
            Data_Buf = (uint32_t)RunVarParam.ErrStatus;                         //����״̬
            break;

        case 0x04:
            Data_Buf = (uint32_t)UserParam.LiquidType;                          //��������
            break;

        case 0x06:                                                              //�����¶�
            Data_Buf = (uint32_t)((RunVarParam.TemperInAir +273.1f) * 10.0f + 0.5f);                
            break;
            
        case 0x08:
            Data_Buf = (uint32_t)RunVarParam.CapBak;                            //��ǰ����ֵ              
            break;

        case 0x0A:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMIN1]);               //����1������            
            break;            

        case 0x0C:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMAX1]);               //����1�������
            break;  
            
        case 0x0E:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMIN2]);           //����2������     
            break; 
            
        case 0x10:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMAX2]);           //����2�������              
            break;
            
        case 0x12:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMIN3]);           //����3������     
            break; 
            
        case 0x14:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMAX3]);           //����3�������              
            break;
            
        case 0x16:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMIN4]);           //����4������     
            break; 
            
        case 0x18:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMAX4]);           //����4�������              
            break;

        case 0x1A:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMIN5]);           //����5������     
            break; 
            
        case 0x1C:
            Data_Buf = (uint32_t)HexToUlong(&Cur_Param[CAPMAX5]);           //����5�������              
            break;                  
            
        case 0x1E:
            Data_Buf = RunVarParam.EmptyCap;                                    //�յ���              
            break;            

        default:
            Data_Buf = 0;
            break;
        }
        //����ֵװ�ص�����      
        for(i = 4; i > 0; i--)
        {
            SenBuf[SendLen++] = (uint8_t)(Data_Buf >> ((i - 1) * 8));
        }
    }
    
           MBASCII_SendMsg(SenBuf,SendLen);
 
}

//**************************************************************************************************
// ����         	: MBASCII_Fun05()
// ��������     	: 2016-09-01
// ����        	 	: ׯ��Ⱥ
// ����         	: Modbus 05���ܴ���
// �������     	: ��
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
//**************************************************************************************************

void MBASCII_Fun05(void)
{
    uint8_t i, AddrOffset;
    uint8_t ReadAdrH;                                                           //�Ĵ�����8λ��ַ
    uint8_t ReadAdrL;                                                           //�Ĵ�����8λ��ַ

    ReadAdrH = RcvBuf[2];
    ReadAdrL = RcvBuf[3];

    SendLen = 0;
    SenBuf[SendLen++] = (MBASCII_GetSlaveAddr(RcvBuf)) ? UsartParam.DeviceAddr : 0x00;//�ӻ���ַ
    SenBuf[SendLen++] = MB_FUNC_WRITE_SINGLE_COIL;//������
    //���������ַ���
    if((ReadAdrL < ASCII_SINGLE_COIL_ADDR_BGEIN) || (ReadAdrL > ASCII_SINGLE_COIL_ADDR_END) || (ReadAdrH != UsartParam.DeviceAddr))
    {
        //���طǷ����ݵ�ַ����
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }
    //���Flashд��ֹ����Flash���ز����޹�
    if((ReadAdrL != 0x53) && (ProductParam.EepromWrEn == EEPROM_WR_DISABLE))
    {
        //���ط����豸ʧ�ܴ���
        MBASCII_SendErr(MB_EX_SLAVE_DEVICE_FAILURE);
        return;
    }
    //�������������Ч
    if(!(((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00)) || ((RcvBuf[4] == 0x00) && (RcvBuf[5] == 0x00))))
    {
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    switch(ReadAdrL)
    {   
        case 0x50:
            if((RcvBuf[4] == 0x00) && (RcvBuf[5] == 0x00))                      //�궨��Һλ
            {
                ProductParam.CapMin = RunVarParam.CapBak;
                RunVarParam.CalibFlag |= CALIB_CAPMIN_FLAG;
            }
            else if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))                 //�궨��Һλ
            {
                ProductParam.CapMax = RunVarParam.CapBak;
                RunVarParam.CalibFlag |= CALIB_CAPMAX_FLAG;
            }
            if((CALIB_CAPMAX_FLAG | CALIB_CAPMIN_FLAG) == (RunVarParam.CalibFlag & (CALIB_CAPMAX_FLAG | CALIB_CAPMIN_FLAG)))                      //�ߵ�Һλֵ���õ���
            {
                if(ProductParam.CapMin < ProductParam.CapMax)
                {
                    AddrOffset = (UserParam.LiquidType - 1) * 16;                //��ַ��Һ������ƫ��
                    for(i = 0; i < 4; i ++)
                    {                                                           //���뻺��
                        Cur_Param[CAPMIN1 + AddrOffset + i] = (uint8_t)(ProductParam.CapMin >> (3 - i) * 8);
                        Cur_Param[CAPMAX1 + AddrOffset + i] = (uint8_t)(ProductParam.CapMax >> (3 - i) * 8);
                    }                                                           //д��EEPROM����õ�������
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
            //�ָ���������
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
                ProductParam.EepromWrEn = EEPROM_WR_ENABLE;//Flashдʹ��
            }
            else
            {
                ProductParam.EepromWrEn = EEPROM_WR_DISABLE;//Flashд��ֹ
            }
            break;
            
        case 0x54:
            if((RcvBuf[4] == 0x00) && (RcvBuf[5] == 0x00))                      //�궨�¿̶�Һλ
            {
                ProductParam.CapLow = RunVarParam.CapBak;
                RunVarParam.CalibFlag |= CALIB_CAPLOW_FLAG;
            }
            else if((RcvBuf[4] == 0xFF) && (RcvBuf[5] == 0x00))                 //�궨�Ͽ̶�Һλ
            {
                ProductParam.CapHigh = RunVarParam.CapBak;
                RunVarParam.CalibFlag |= CALIB_CAPHIGH_FLAG;
            }
            if((CALIB_CAPLOW_FLAG | CALIB_CAPHIGH_FLAG) == (RunVarParam.CalibFlag & (CALIB_CAPLOW_FLAG | CALIB_CAPHIGH_FLAG)))                      //��/�¿̶�Һλֵ���õ���
            {
                if(ProductParam.CapLow < ProductParam.CapHigh)
                {
                    AddrOffset = (UserParam.LiquidType - 1) * 16;                //��ַ��Һ������ƫ��
                    for(i = 0; i < 4; i ++)
                    {                                                           //���뻺��
                        Cur_Param[CAPLOW1 + AddrOffset + i] = (uint8_t)(ProductParam.CapLow >> (3 - i) * 8);
                        Cur_Param[CAPHIGH1 + AddrOffset + i] = (uint8_t)(ProductParam.CapHigh >> (3 - i) * 8);
                    }                                                           //д��EEPROM����õ�������
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
    //Modbus���Ͳ����ɹ�
    MBASCII_SendMsg(RcvBuf, 6);
}




//**************************************************************************************************
// ����         	: MBASCII_Fun10()
// ��������     	: 2016-09-01
// ����        	 	: ׯ��Ⱥ
// ����         	: Modbus 10���ܴ���
// �������     	: ��
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
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

        //�����ȡ��Χ���
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
          case 0x30://�豸��ַ
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
            
          case 0x31://������
              if(0x06 > RcvBuf[8 + index])
              {
                UsartParam.BaudRate = RcvBuf[8 + index];
                Eeprom_Write3T_MultiBytes(BAUDRATE, &UsartParam.BaudRate, 1);
              }
            break;
            
          case 0x32://��żУ��
              if(0x03 > RcvBuf[8 + index])
              {
                UsartParam.Parity = RcvBuf[8 + index];
                Eeprom_Write3T_MultiBytes(PARITY, &UsartParam.Parity, 1);
              }
            break;
            
          case 0x33:
              break;
            
          case 0x34://����ʹ��
              if(2 > RcvBuf[8 + index])
              {
                ProductParam.CompenEn = RcvBuf[8 + index];
                Eeprom_Write3T_MultiBytes(COMPENSATE, &ProductParam.CompenEn, 1);
              }
            break;
            
          case 0x35://�˲�ϵ��
              if(0x0A > RcvBuf[8 + index])
              {
                UserParam.Filterfactor = RcvBuf[8 + index];
                Eeprom_Write3T_MultiBytes(FILTER, &UserParam.Filterfactor, 1);
                SwitchCurFilter(UserParam.Filterfactor);
              }
            break;
            
          case 0x36://�Զ��ϴ�����
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
                UserParam.FilterStart = 0;                                      //�������޸�Һ�����ͣ�Ҫ���¿�ʼ�˲�ͳ��
                AddrOffset = (UserParam.LiquidType - 1) * 8;                    //Ҫ����EEPROM�е�ֵ������Һλ              
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
// ����         	: MBASCII_Fun25()
// ��������     	: 2016-10-20
// ����        	 	: ׯ��Ⱥ
// ����         	: Modbus 25���ܴ���
// �������     	: ��
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
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

    //���������Χ���

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
        {                                                           //���뻺��
            Cur_Param[CAP_EMPTY + i] = (uint8_t)(RunVarParam.EmptyCap >> (3 - i) * 8);
        }                                                           //д��EEPROM����õ�������
        Eeprom_Write3T_MultiBytes(CAP_EMPTY, (uint8_t const *)&Cur_Param[CAP_EMPTY], 4);        
      }
      break;
      
    case 0x01:
    //����DA�궨���¿̶���
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
    //����DA�궨ʹ��
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
          //����DA�궨���������
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
    //�¶ȱ궨ʹ��
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
    //�¶ȱ궨
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
    //����AD�궨ʹ��
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
    //����AD�궨���¿̶���
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
    //����AD�궨���������
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
// ����         	: MBASCII_Fun26()
// ��������     	: 2016-10-20
// ����        	 	: ׯ��Ⱥ
// ����         	: Modbus 26���ܴ���
// �������     	: ��
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
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
    SenBuf[SendLen++] = Register_Num * 2;      //���ݳ���

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
          //��
          DataBuf = 0;
          break;
        
        case 0x82:
          //�������
          fBuf = (float)ProductParam.CapMin;
          DataBuf = *(uint32_t *)&fBuf;
          break;
        
        case 0x84:
          //����������
          fBuf = (float)ProductParam.CapMax;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0x86:
          //��
          DataBuf = 0;
          break;
        
        case 0x90:
          //�����¶�����ϵ��K1
          fBuf = UserParam.Temper_K1;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0x92:
          //�����¶�����ϵ��B1
          fBuf = UserParam.Temper_B1;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0x94:
          break;
          
        case 0x96:
          break;
          
        case 0xB6:
          //����DA�궨ֵ
          fBuf = (float)RunVarParam.CapDA_ClibValue;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xB8:
          //�����¶�DA�궨ֵ
          fBuf = (float)RunVarParam.TempDA_ClibValue;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xBA:
          //����AD�궨ֵ
          fBuf = (float)RunVarParam.CapAD_ClibValue;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xBC:
          //�����¿̶ȱ궨�߶�ֵ
          fBuf = (float)ProductParam.CapLowHeight;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xBE:
          //�����Ͽ̶ȱ궨�߶�ֵ
          fBuf = (float)ProductParam.CapHighHeight;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xC0:
          //�����¿̶ȱ궨����ֵ
          fBuf = (float)ProductParam.CapLow;
          DataBuf = *(uint32_t *)&fBuf;
          break;
          
        case 0xC2:
          //�����Ͽ̶ȱ궨����ֵ
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
// ����         	: MBASCII_Fun27()
// ��������     	: 2016-10-20
// ����        	 	: ׯ��Ⱥ
// ����         	: Modbus 27���ܴ���
// �������     	: ��
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
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
    case 0x80://����
      //fbuf = HexToFloat(&RcvBuf[7 + index]);
      //ProductParam.HeightRange = (uint16_t)(fbuf * 100);
      //Cur_Param[HEIGHTRANGE] = (unsigned char)(ProductParam.HeightRange >> 8);
      //Cur_Param[HEIGHTRANGE + 1] = (unsigned char)ProductParam.HeightRange;
      //Eeprom_Write3T_MultiBytes(HEIGHTRANGE, (uint8_t const *)&Cur_Param[HEIGHTRANGE], 2);
      break;
      
    case 0x82:
      //��
      break;
      
    case 0x84:
      //��
      break;
      
    case 0x90:
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
      
    case 0x92:
      //�����¶�����ϵ��B1
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
      
    case 0xB8:
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
      
    case 0xBA:
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
      
    case 0xBC:
      //�����¿̶ȱ궨�߶�
        fbuf = HexToFloat(&RcvBuf[7 + index]);
        ProductParam.CapLowHeight = (uint16_t)fbuf;
        AddrOffset = (UserParam.LiquidType - 1) * 4;                //��ַ��Һ������ƫ��
        Cur_Param[CAPLOWHEIGHT1 + AddrOffset] = (uint16_t)fbuf >> 8;
        Cur_Param[CAPLOWHEIGHT1 + AddrOffset + 1] = (uint16_t)fbuf & 0xFF;
        Eeprom_Write3T_MultiBytes(CAPLOWHEIGHT1 + AddrOffset, 
                                  (uint8_t const *)&Cur_Param[CAPLOWHEIGHT1 + AddrOffset], 2);
      break;
      
    case 0xBE:
      //�����Ͽ̶ȱ궨�߶�
        fbuf = HexToFloat(&RcvBuf[7 + index]);
        ProductParam.CapHighHeight = (uint16_t)fbuf;
        AddrOffset = (UserParam.LiquidType - 1) * 4;                //��ַ��Һ������ƫ��
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
// ����         	: MBASCII_Fun2A()
// ��������     	: 2016-10-20
// ����        	 	: ׯ��Ⱥ
// ����         	: д�汾��Ϣ
// �������     	: ��
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
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
      //��������
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
    //��Ʒ����    
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
    //Ӳ���汾    
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
    //����汾    
    case 0xE3:
      objlen = RcvBuf[j] + 1;
      //                                                                                                    
      j += objlen;
      break;
    //�豸ID    
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
    //�ͻ�����    
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
// ����         	: MBASCII_Fun2B()
// ��������     	: 2016-10-20
// ����        	 	: ׯ��Ⱥ
// ����         	: ���汾��Ϣ
// �������     	: ��
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
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
        
    //��������  
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
    //��Ʒ����  
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
    //Ӳ���汾  
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
    //����汾 
    case 0x00E3:
      SenBuf[SendLen2B++] = 0xE3;                                                     
      SenBuf[SendLen2B++] = 0x07;
      
      for(objlen = 0; objlen < 7; objlen++)                                                        
      {
        SenBuf[SendLen2B++] = SensorSoftVersion[objlen + 1];
      }
      break;
    //�豸ID  
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
    //�ͻ�����  
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
// ����         	: MBASCII_Fun41()
// ��������     	: 2016-10-20
// ����        	 	: ׯ��Ⱥ
// ����         	: ������������
// �������     	: ��
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
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
// ����         	: MBASCII_Function()
// ��������   	        : 2016-09-01
// ����         	: ׯ��Ⱥ
// ����         	: ModbusASCII�����
// �������     	: ��
// �������     	: ��
// ���ؽ��     	: ��
// ע���˵��   	:
// �޸�����     	:
//**************************************************************************************************

void MBASCII_Function(void)
{
    uint8_t RecvLen = 0;
    if(2 == MODBUS_ASCII_RecvData(RcvBuf, &RecvLen))//У�����
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
        case MB_FUNC_READ_HOLDING_REGISTER:	//�������Ĵ���0x03
          MBASCII_Fun03();
          break;
            
        case MB_FUNC_READ_INPUT_REGISTER:		//�������Ĵ���0x04
          MBASCII_Fun04();
          break;
            
        case MB_FUNC_WRITE_SINGLE_COIL:		//д������Ȧ
          MBASCII_Fun05();
          break;
            
        case MB_FUNC_WRITE_MULTIPLE_REGISTERS://д����Ĵ���0x10
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
// ����               :ModbusAutoUpload()
// ��������           :2017-05-13
// ����               :MMX
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
    SenBuf[SenBufLen++] = 0x14;

    fbuf = (float)(RunVarParam.LiquidHeight * 10.0f + 0.5f);                    //Һλ�߶�
    DataBuf = (uint32_t)fbuf;
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }

    DataBuf = (uint32_t)RunVarParam.ErrStatus;                                  //����״̬
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }
  
    DataBuf = (uint32_t)UserParam.LiquidType;                                   //Һ������
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }
    
    fbuf = (float)RunVarParam.TemperInAir;                                      //�����¶�
    DataBuf = (uint32_t)((fbuf + 273.1f) * 10.0f + 0.5f);
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }
    
    DataBuf = RunVarParam.CapBak;                                               //ԭʼ����ֵ
    for(i = 4; i > 0; i--)
    {
        SenBuf[SenBufLen++] = (uint8_t)(DataBuf >> ((i - 1) * 8));
    }    
      if(cnt<4)
      {
          //Modbus������Ϣ      
          if( RunVarParam.CapBak <= 0x00FF0000 )    //��λ3�����ڣ���������ֵ
          {
             MBASCII_SendMsg(SenBuf,SenBufLen);
          }
             if(DataError >=8)
            {   
              DataError = 0;                           
              cnt+=1;    //��λ������1
              ProductParam.EepromWrEn = EEPROM_WR_ENABLE;//EEʹ��
                Eeprom_Write_OneByte(RESET_CNT, cnt);   //д������EE
              ProductParam.EepromWrEn = EEPROM_WR_DISABLE;
              
              //ϵͳ��λ
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
        MBASCII_SendMsg(SenBuf,SenBufLen);     //��λ3�����ϣ�������ֵ
               cnt+=1;            
        //ϵͳ��λ
             // NVIC_SystemReset();
      }
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
    if((1 != UsartParam.AutoUpload) && (UsartParam.AutoUploadCn >= AutoUploadTimeConvert(UsartParam.AutoUpload) * 10))
    {   
        if(LiquidStableFlag == 1)                                               //ÿ��Һλ���˲�Һλ����ʱ��ʼ�ϴ����ݣ�ֻ����ϵ��Ƕ�ʱ�䣩
        {
            ModbusAutoUpload();                                                 //Modbus֡�Զ��ϴ�
        }
        UsartParam.AutoUploadCn = 0;
    }
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
