/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        stm32l0_eeprom_app.c
** ����:          ׯ��Ⱥ
** �汾:          V1.0.0
** ����:          2017-05-03
** ����:          ��STM32L072��EEPROM�ж�ȡϵͳ�ڲ�����
** ����:          ϵͳ������ȡ�봦��
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#include "stm32l0_eeprom_app.h"

uint8_t Cur_Param[PRO_DEFAULT_LEN] = {0};                                  //ϵͳ����

const uint8_t SensorSoftVersion[8] = {0x07, 'S', 'V', '1', '.', '1', '.', '7'};//����汾�� ��ʽ:�汾�ų��ȣ��汾������

const uint8_t User_Default_Param[PRO_DEFAULT_LEN] = 
{
    0x01,                                                                  //������ʼ����־λ
    0x41,                                                                  //�豸��ַ
    0x03,                                                                  //������(9600)
    0x03,                                                                  //��żУ��(��У��)
    0x00,                                                                  //�˲�ϵ��(1s)
    0x02,                                                                  //�Զ���������
    0x00,                                                                  //����ʹ��
    0x00,                                                                  //�Ƿ񶳽��豸
    0x00,                                                                  //�����ʽ
    
    0x01,                                                                  //��ҩ����     
    0x00,0x7A,                                                             //����Ĭ��122(mm)
    
    
    0x00,0x00,0x00,0x00,                                                   //������ֵ1
    0x00,0x07,0xA1,0x20,                                                   //�¿̶ȵ���ֵ1
    0x00,0x0F,0x42,0x40,                                                   //�Ͽ̶ȵ���ֵ1
    0xFF,0xFF,0xFF,0xFF,                                                   //�����̵���ֵ1
    0x00,0x00,0x00,0x00,                                                   //������ֵ2
    0x00,0x07,0xA1,0x20,                                                   //�¿̶ȵ���ֵ2
    0x00,0x0F,0x42,0x40,                                                   //�Ͽ̶ȵ���ֵ2
    0xFF,0xFF,0xFF,0xFF,                                                   //�����̵���ֵ2
    0x00,0x00,0x00,0x00,                                                   //������ֵ3
    0x00,0x07,0xA1,0x20,                                                   //�¿̶ȵ���ֵ3
    0x00,0x0F,0x42,0x40,                                                   //�Ͽ̶ȵ���ֵ3
    0xFF,0xFF,0xFF,0xFF,                                                   //�����̵���ֵ3
    0x00,0x00,0x00,0x00,                                                   //������ֵ4
    0x00,0x07,0xA1,0x20,                                                   //�¿̶ȵ���ֵ4
    0x00,0x0F,0x42,0x40,                                                   //�Ͽ̶ȵ���ֵ4
    0xFF,0xFF,0xFF,0xFF,                                                   //�����̵���ֵ4  
    0x00,0x00,0x00,0x00,                                                   //������ֵ5
    0x00,0x07,0xA1,0x20,                                                   //�¿̶ȵ���ֵ5
    0x00,0x0F,0x42,0x40,                                                   //�Ͽ̶ȵ���ֵ5
    0xFF,0xFF,0xFF,0xFF,                                                   //�����̵���ֵ5 

    0x00,0x00,                                                             //�������ADֵ
    0x00,0x00,                                                             //�����¿̶�ADֵ
    0x00,0x00,                                                             //�����Ͽ̶�ADֵ
    0xFF,0xFF,                                                             //����������ADֵ

    0x00,0x00,                                                             //�������DAֵ
    0x00,0x00,                                                             //�����¿̶�DAֵ
    0x00,0x00,                                                             //�����Ͽ̶�DAֵ
    0x0F,0xFF,                                                             //��������DAֵ
    0x00,0x00,                                                             //�¶����DAֵ
    0x0F,0xFF,                                                             //�¶�����DAֵ

    0x00,0x64,                                                             //��������ϵ��K
    0x00,0x64,                                                             //��������ϵ��B
    0x00,0x64,                                                             //�¶�1����ϵ��K1
    0x00,0x64,                                                             //�¶�1����ϵ��B1
    
    0x00,0x0A,                                                             //�����¿̶ȱ궨�߶�1
    0x00,0x70,                                                             //�����Ͽ̶ȱ궨�߶�1
    0x00,0x0A,                                                             //�����¿̶ȱ궨�߶�2
    0x00,0x70,                                                             //�����Ͽ̶ȱ궨�߶�2
    0x00,0x0A,                                                             //�����¿̶ȱ궨�߶�3
    0x00,0x70,                                                             //�����Ͽ̶ȱ궨�߶�3
    0x00,0x0A,                                                             //�����¿̶ȱ궨�߶�4
    0x00,0x70,                                                             //�����Ͽ̶ȱ궨�߶�4
    0x00,0x0A,                                                             //�����¿̶ȱ궨�߶�5
    0x00,0x70,                                                             //�����Ͽ̶ȱ궨�߶�5
    
    0x00,0x64,0x00,0x64,                                                    //�յ��ݣ�û�з���Һ��ʱ�ĵ��ݣ�                                                         
    0x00                                                                   //��ֵ����λ����
};

//****************************************************************************************************************************************************************
// ����               :SwitchCurFilter()
// ��������           :2017-05-10
// ����               :ׯ��Ⱥ
// ����               :�л��˲�ˮƽ����
// �������           :filtfactor(�˲�ϵ��)
// �������           :��
// ���ؽ��           :��
// ע���˵��         :�˲�ˮƽ��9�������ִӵ͵��߶�Ӧ�˲���ȴӵ͵���
// �޸�����           :
//****************************************************************************************************************************************************************

void SwitchCurFilter(uint8_t filtfactor)
{
  uint8_t filtval;
  
  filtval = filtfactor;
  
  switch(filtval)
  {
  case 0:
    UserParam.FilterBufMax = 1;
    UserParam.FactorCycle = 1;
    break;
    
  case 1:
    UserParam.FilterBufMax = 1;
    UserParam.FactorCycle = 12;
    break;
    
  case 2:
    UserParam.FilterBufMax = 2;
    UserParam.FactorCycle = 24;
    break;
    
  case 3:
    UserParam.FilterBufMax = 3;
    UserParam.FactorCycle = 36;
    break;
    
  case 4:
    UserParam.FilterBufMax = 5;
    UserParam.FactorCycle = 60;
    break;
    
  case 5:
    UserParam.FilterBufMax = 10;
    UserParam.FactorCycle = 120;
    break;
    
  case 6:
    UserParam.FilterBufMax = 15;
    UserParam.FactorCycle = 180;
    break;
    
  case 7:
    UserParam.FilterBufMax = 20;
    UserParam.FactorCycle = 240;
    break;
    
  case 8:
    UserParam.FilterBufMax = 40;
    UserParam.FactorCycle = 480;
    break;
    
  case 9:
    UserParam.FilterBufMax = 80;
    UserParam.FactorCycle = 960;
    break;
    
  default:
    UserParam.FilterBufMax = 1;
    UserParam.FactorCycle = 12;
    break;
  }
  UserParam.FilDivideFactory = UserParam.FactorCycle / UserParam.FilterBufMax;
  if(UserParam.FilDivideFactory > 12)        //���ܳ����˲�����ĳ���
  {
        UserParam.FilDivideFactory = 12;
  }
  if(UserParam.FilterBufMax > 80)
  {
        UserParam.FilterBufMax = 80;
  }
  UserParam.FilterStart = 0;
}

//****************************************************************************************************************************************************************
// ����               :Eeprom_Write3T_MultiBytes()
// ��������           :2017-05-03
// ����               :ׯ��Ⱥ
// ����               :��STM32L072xx�ڲ�EEPROMָ��λ��д����ֽ��ұ���
// �������           :EepAddr(д��ʼ��ַ)��pWrbuf(�̶������ڴ�),Wrlen(д���ݳ���)
// �������           :��
// ���ؽ��           :OP_SUCCESS(д��ɹ�)��OP_FAILED(д��ʧ��)
// ע���˵��         :д����ǰ���EEPROMд����ʹ�ܣ���ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

uint8_t Eeprom_Write3T_MultiBytes(uint16_t EepAddr, uint8_t const *pWrbuf, uint16_t Wrlen)
{
  //����״̬
  uint8_t Err;
  //д״̬
  uint8_t Wrsta;
  //EEPROMƫ��
  uint16_t EpCnt;
  //д��ʼ��ַ
  uint16_t EepAddress;
  
  Err = OP_SUCCESS;
  Wrsta = OP_SUCCESS;
  EepAddress = EepAddr;
  
  if(EEPROM_WR_ENABLE != ProductParam.EepromWrEn)
  {
    return OP_FAILED;
  }
  
  //��д���ݳ���
  for(EpCnt = 0; EpCnt < Wrlen; EpCnt++)
  {
    //ϵͳ�����洢��
    Wrsta = Eeprom_Write_OneByte((EepAddress + EpCnt), *(pWrbuf + EpCnt));
    if(OP_SUCCESS != Wrsta)
    {
      Err = OP_FAILED;
    }
    //ϵͳ����������1
    Wrsta = Eeprom_Write_OneByte((SYSTEM_PARAM_BAK1 + EepAddress + EpCnt), *(pWrbuf + EpCnt));
    if(OP_SUCCESS != Wrsta)
    {
      Err = OP_FAILED;
    }
    //ϵͳ����������2
    Wrsta = Eeprom_Write_OneByte((SYSTEM_PARAM_BAK2 + EepAddress + EpCnt), *(pWrbuf + EpCnt));
    if(OP_SUCCESS != Wrsta)
    {
      Err = OP_FAILED;
    }
  }
  return Err;
}

//****************************************************************************************************************************************************************
// ����               :Eeprom_SystemParam_Check()
// ��������           :2017-05-03
// ����               :ׯ��Ⱥ
// ����               :EEPROMϵͳ�������
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :д����ǰ���EEPROMд����ʹ�ܣ���ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

void Eeprom_SystemParam_Check(void)
{
  uint16_t Cnt;
  uint8_t Check_Sta;
  uint8_t Cur_Param_Bak1;
  uint8_t Cur_Param_Bak2;
  
  for(Cnt = 0; Cnt < PRO_DEFAULT_LEN; Cnt++)
  {
    //���״̬
    Check_Sta = 0;
    //����1
    Cur_Param_Bak1 = Eeprom_Read_OneByte((SYSTEM_PARAM_BAK1 + Cnt));
    //����2
    Cur_Param_Bak2 = Eeprom_Read_OneByte((SYSTEM_PARAM_BAK2 + Cnt));
    //��ǰ�����뱸��1��ͬ
    if(Cur_Param[Cnt] != Cur_Param_Bak1)
    {
      Check_Sta |= 0x01;
    }
    //����1�뱸��2��ͬ
    if(Cur_Param_Bak1 != Cur_Param_Bak2)
    {
      Check_Sta |= 0x02;
    }
    //��ǰ�����뱸��2��ͬ
    if(Cur_Param[Cnt] != Cur_Param_Bak2)
    {
      Check_Sta |= 0x04;
    }
    //����2����
    if(0x06 == Check_Sta)
    {
      Eeprom_Write_OneByte((SYSTEM_PARAM_BAK2 + Cnt), Cur_Param[Cnt]);
    }
    //����1����
    else if(0x03 == Check_Sta)
    {
      Eeprom_Write_OneByte((SYSTEM_PARAM_BAK1 + Cnt), Cur_Param[Cnt]);
    }
    //��ǰ��������
    else if(0x05 == Check_Sta)
    {
      Eeprom_Write_OneByte((RUN_ADDR_BASE + Cnt), Cur_Param_Bak1);
    }
    else 
    {
      if(0x00 != Check_Sta)
      {
        break;
      }
    }
  }
  //�����������ȫ���Ծ�д��ȱʡϵͳ����
  if((0x01 == Check_Sta) || (0x02 == Check_Sta) || (0x04 == Check_Sta) || (0x07 == Check_Sta))
  {
    //д��ϵͳ����ȱʡֵ�����ݵ�ַ��
    Eeprom_Write3T_MultiBytes(RUN_ADDR_BASE, User_Default_Param, PRO_DEFAULT_LEN);
    //������ǰϵͳ����
    Eeprom_Read_MultiBytes(RUN_ADDR_BASE, Cur_Param, PRO_DEFAULT_LEN);
  }
}


//****************************************************************************************************************************************************************
// ����               :Read_Device_Param()
// ��������           :2017-05-10
// ����               :ׯ��Ⱥ
// ����               :��ȡ�豸����
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :
// �޸�����           :
//****************************************************************************************************************************************************************

void Read_Device_Param(void)
{
    uint8_t EepInitFlg;
    uint16_t ParaTemp;

    ProductParam.EepromWrEn = EEPROM_WR_ENABLE;
    EepInitFlg = Eeprom_Read_OneByte(RUN_ADDR_BASE);
    if(User_Default_Param[RUN_ADDR_BASE] != EepInitFlg)
    {
        ParaTemp = 0;
        while(OP_SUCCESS != Eeprom_Write3T_MultiBytes(RUN_ADDR_BASE, User_Default_Param, USER_DEFAULT_LEN))
          //while(OP_SUCCESS != Eeprom_Write3T_MultiBytes(RUN_ADDR_BASE, User_Default_Param, PRO_DEFAULT_LEN))
        {
           ParaTemp++;
            if(WRITE_EEPROM_ERR_MAX < ParaTemp)
            {
                break;
            }
        }
    }
   
    ParaTemp = 0;
    Eeprom_Read_MultiBytes(RUN_ADDR_BASE, Cur_Param, PRO_DEFAULT_LEN);
    Eeprom_SystemParam_Check();
    UsartParam.DeviceAddr = Cur_Param[DEVICE_ADDR];

    if((0x01 > UsartParam.DeviceAddr) || (0xF7 < UsartParam.DeviceAddr))        //�����豸��ַ��Χ������Ϊ1
    {
        UsartParam.DeviceAddr = 0x01;
        Eeprom_Write3T_MultiBytes(DEVICE_ADDR, &UsartParam.DeviceAddr, 1);
    }
    UsartParam.BaudRate = Cur_Param[BAUDRATE];
    if((UsartParam.BaudRate == 0) || (UsartParam.BaudRate > 0x07))              //���ڲ���������Χ,������03
    {
        UsartParam.BaudRate = 0x03;
        Eeprom_Write3T_MultiBytes(BAUDRATE, &UsartParam.BaudRate, 1);
    }
    UsartParam.Parity = Cur_Param[PARITY];

    if((UsartParam.Parity == 0) || (UsartParam.Parity > 0x03))                  //��ЧУ�鷽ʽ������Ϊ0                                                
    {
        UsartParam.Parity = 0x03;
        Eeprom_Write3T_MultiBytes(PARITY, &UsartParam.Parity, 1);
    }

    UsartParam.AutoUpload = Cur_Param[AUTO_UPLOAD];                             //�Զ��ϴ����ڴ���

    if((UsartParam.AutoUpload == 0) || (UsartParam.AutoUpload > 4))
    {
        UsartParam.AutoUpload = 2;
    }

    UserParam.Filterfactor = Cur_Param[FILTER];                                 //�˲�ϵ��

    if(0x09 < UserParam.Filterfactor)                                           //��Ч�˲�ϵ��������1
    {
        UserParam.Filterfactor = 1;
        Eeprom_Write3T_MultiBytes(FILTER, &UserParam.Filterfactor, 1);
    }

    SwitchCurFilter(UserParam.Filterfactor);                                    //�л��˲�

    UserParam.Freeze = Cur_Param[FREEZE];
    if((0x0F != UserParam.Freeze) && (0x00 != UserParam.Freeze))
    {
        UserParam.Freeze = 0x00;
        Eeprom_Write3T_MultiBytes(FREEZE, &UserParam.Freeze, 1);
    }
    //�����ʽ����ʱ����
    UserParam.OutputMode = Cur_Param[OUTPUTMODE];
    UserParam.LiquidType = Cur_Param[LIQUIDTYPE];
    if(UserParam.LiquidType == 0 || UserParam.LiquidType > 5)
    {
        UserParam.LiquidType = 1;
    }

    //����ʹ��
    ProductParam.CompenEn = Cur_Param[COMPENSATE];
    //����ʹ��ֻ������0��1����Ч����0
    if(0x01 < ProductParam.CompenEn)
    {
        ProductParam.CompenEn = 0x00;
        Eeprom_Write3T_MultiBytes(COMPENSATE, &ProductParam.CompenEn, 1);
    }
    
    UserParam.Freeze = Cur_Param[FREEZE];
       
    //д������ֹ
    ProductParam.EepromWrEn = EEPROM_WR_DISABLE;

    ParaTemp = (uint16_t)(Cur_Param[CORRECT_K] << 8) | Cur_Param[CORRECT_K + 1];
    //��������ϵ��K
    UserParam.Correct_K = ParaTemp / 100.0;

    ParaTemp = (uint16_t)(Cur_Param[CORRECT_B] << 8) | Cur_Param[CORRECT_B + 1];
    //��������ϵ��B
    UserParam.Correct_B = ParaTemp;

    ParaTemp = (uint16_t)(Cur_Param[TEMPER_K1] << 8) | Cur_Param[TEMPER_K1 + 1];
    //�¶�1����ϵ��K1
    UserParam.Temper_K1 = ParaTemp / 100.0;

    ParaTemp = (uint16_t)(Cur_Param[TEMPER_B1] << 8) | Cur_Param[TEMPER_B1 + 1];
    //�¶�1����ϵ��B1
    UserParam.Temper_B1 = ParaTemp / 100.0;

    //ParaTemp = (uint16_t)(Cur_Param[TEMPER_K2] << 8) | Cur_Param[TEMPER_K2 + 1];
    //�¶�2����ϵ��K2
    //UserParam.Temper_K2 = ParaTemp / 100.0;

    //ParaTemp = (uint16_t)(Cur_Param[TEMPER_B2] << 8) | Cur_Param[TEMPER_B2 + 1];
    //�¶�2����ϵ��B2
    //UserParam.Temper_B2 = ParaTemp / 100.0;
    //����mm ����
    ProductParam.HeightRange = (uint16_t)(Cur_Param[HEIGHTRANGE] << 8) | Cur_Param[HEIGHTRANGE + 1];
    //����ADֵ���
    ProductParam.CapADMin = (uint16_t)(Cur_Param[CAPADMIN] << 8) | Cur_Param[CAPADMIN + 1];
    //����ADֵ�¿̶�
    ProductParam.CapADLow = (uint16_t)(Cur_Param[CAPADLOW] << 8) | Cur_Param[CAPADLOW + 1];
    //����ADֵ�Ͽ̶�
    ProductParam.CapADHih = (uint16_t)(Cur_Param[CAPADHIH] << 8) | Cur_Param[CAPADHIH + 1];
    //����ADֵ����
    ProductParam.CapADMax = (uint16_t)(Cur_Param[CAPADMAX] << 8) | Cur_Param[CAPADMAX + 1];
    //����ADֵ����
    ProductParam.CapADRange = ProductParam.CapADMax - ProductParam.CapADMin;
    //����DAֵ���
    ProductParam.CapDAMin = (uint16_t)(Cur_Param[CAPDAMIN] << 8) | Cur_Param[CAPDAMIN + 1];
    //����DAֵ�¿̶�
    ProductParam.CapDALow = (uint16_t)(Cur_Param[CAPDALOW] << 8) | Cur_Param[CAPDALOW + 1];
    //����DAֵ�Ͽ̶�
    ProductParam.CapDAHih = (uint16_t)(Cur_Param[CAPDAHIH] << 8) | Cur_Param[CAPDAHIH + 1];
    //����DAֵ����
    ProductParam.CapDAMax = (uint16_t)(Cur_Param[CAPDAMAX] << 8) | Cur_Param[CAPDAMAX + 1];
    //����DAֵ����
    ProductParam.CapDARange = ProductParam.CapDAMax - ProductParam.CapDAMin;
    //�¶�DAֵ���
    ProductParam.TempDAMin = (uint16_t)(Cur_Param[TEMPDAMIN] << 8) | Cur_Param[TEMPDAMIN + 1];
    //�¶�DAֵ����
    ProductParam.TempDAMax = (uint16_t)(Cur_Param[TEMPDAMAX] << 8) | Cur_Param[TEMPDAMAX + 1];
    //�¶�DAֵ����
    ProductParam.TempDARange = ProductParam.TempDAMax - ProductParam.TempDAMin;
    //�������
    
    ProductParam.CapMin = HexToUlong(&Cur_Param[CAPMIN1 + (UserParam.LiquidType - 1) * 16]);
    //�����¿̶�
    ProductParam.CapLow = HexToUlong(&Cur_Param[CAPLOW1 + (UserParam.LiquidType - 1) * 16]);
    //�����Ͽ̶�
    ProductParam.CapHigh = HexToUlong(&Cur_Param[CAPHIGH1 + (UserParam.LiquidType - 1) * 16]);
    //��������
    ProductParam.CapMax = HexToUlong(&Cur_Param[CAPMAX1 + (UserParam.LiquidType - 1) * 16]);
    
    //�����¿̶ȱ궨�߶�
    ProductParam.CapLowHeight = (uint16_t)(Cur_Param[CAPLOWHEIGHT1 + (UserParam.LiquidType - 1) * 4] << 8) 
                                | Cur_Param[CAPLOWHEIGHT1 + (UserParam.LiquidType - 1) * 4 + 1];
    //�����Ͽ̶ȱ궨�߶�
    ProductParam.CapHighHeight = (uint16_t)(Cur_Param[CAPHIGHHEIGHT1 + (UserParam.LiquidType - 1) * 4] << 8) 
                                | Cur_Param[CAPHIGHHEIGHT1 + (UserParam.LiquidType - 1) * 4 + 1];
    
    //������㱸��
    //ProductParam.CapMinBak = HexToUlong(&Cur_Param[CAPMINBAK]);
    //�������㱸��
    //ProductParam.CapMaxBak = HexToUlong(&Cur_Param[CAPMAXBAK]);
    //��������
    ProductParam.CapRange = ProductParam.CapMax - ProductParam.CapMin;
    //���ݲ����޶�ֵ
    //ProductParam.CapWaveLimit = (uint32_t)(ProductParam.CapRange * 0.03f);
    
    RunVarParam.EmptyCap = HexToUlong(&Cur_Param[CAP_EMPTY]);
}

