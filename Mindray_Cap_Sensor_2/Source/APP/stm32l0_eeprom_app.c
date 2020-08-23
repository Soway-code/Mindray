/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        stm32l0_eeprom_app.c
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-03
** 描述:          从STM32L072的EEPROM中读取系统内部参数
** 功能:          系统参数读取与处理
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#include "stm32l0_eeprom_app.h"

uint8_t Cur_Param[PRO_DEFAULT_LEN] = {0};                                  //系统参数

const uint8_t SensorSoftVersion[8] = {0x07, 'S', 'V', '1', '.', '1', '.', '7'};//软件版本号 格式:版本号长度，版本号内容

const uint8_t User_Default_Param[PRO_DEFAULT_LEN] = 
{
    0x01,                                                                  //参数初始化标志位
    0x41,                                                                  //设备地址
    0x03,                                                                  //波特率(9600)
    0x03,                                                                  //奇偶校验(无校验)
    0x00,                                                                  //滤波系数(1s)
    0x02,                                                                  //自动发送周期
    0x00,                                                                  //补偿使能
    0x00,                                                                  //是否冻结设备
    0x00,                                                                  //输出方式
    
    0x01,                                                                  //麻药类型     
    0x00,0x7A,                                                             //量程默认122(mm)
    
    
    0x00,0x00,0x00,0x00,                                                   //零点电容值1
    0x00,0x07,0xA1,0x20,                                                   //下刻度电容值1
    0x00,0x0F,0x42,0x40,                                                   //上刻度电容值1
    0xFF,0xFF,0xFF,0xFF,                                                   //满量程电容值1
    0x00,0x00,0x00,0x00,                                                   //零点电容值2
    0x00,0x07,0xA1,0x20,                                                   //下刻度电容值2
    0x00,0x0F,0x42,0x40,                                                   //上刻度电容值2
    0xFF,0xFF,0xFF,0xFF,                                                   //满量程电容值2
    0x00,0x00,0x00,0x00,                                                   //零点电容值3
    0x00,0x07,0xA1,0x20,                                                   //下刻度电容值3
    0x00,0x0F,0x42,0x40,                                                   //上刻度电容值3
    0xFF,0xFF,0xFF,0xFF,                                                   //满量程电容值3
    0x00,0x00,0x00,0x00,                                                   //零点电容值4
    0x00,0x07,0xA1,0x20,                                                   //下刻度电容值4
    0x00,0x0F,0x42,0x40,                                                   //上刻度电容值4
    0xFF,0xFF,0xFF,0xFF,                                                   //满量程电容值4  
    0x00,0x00,0x00,0x00,                                                   //零点电容值5
    0x00,0x07,0xA1,0x20,                                                   //下刻度电容值5
    0x00,0x0F,0x42,0x40,                                                   //上刻度电容值5
    0xFF,0xFF,0xFF,0xFF,                                                   //满量程电容值5 

    0x00,0x00,                                                             //电容零点AD值
    0x00,0x00,                                                             //电容下刻度AD值
    0x00,0x00,                                                             //电容上刻度AD值
    0xFF,0xFF,                                                             //电容满量程AD值

    0x00,0x00,                                                             //电容零点DA值
    0x00,0x00,                                                             //电容下刻度DA值
    0x00,0x00,                                                             //电容上刻度DA值
    0x0F,0xFF,                                                             //电容满度DA值
    0x00,0x00,                                                             //温度零点DA值
    0x0F,0xFF,                                                             //温度满度DA值

    0x00,0x64,                                                             //电容修正系数K
    0x00,0x64,                                                             //电容修正系数B
    0x00,0x64,                                                             //温度1修正系数K1
    0x00,0x64,                                                             //温度1修正系数B1
    
    0x00,0x0A,                                                             //电容下刻度标定高度1
    0x00,0x70,                                                             //电容上刻度标定高度1
    0x00,0x0A,                                                             //电容下刻度标定高度2
    0x00,0x70,                                                             //电容上刻度标定高度2
    0x00,0x0A,                                                             //电容下刻度标定高度3
    0x00,0x70,                                                             //电容上刻度标定高度3
    0x00,0x0A,                                                             //电容下刻度标定高度4
    0x00,0x70,                                                             //电容上刻度标定高度4
    0x00,0x0A,                                                             //电容下刻度标定高度5
    0x00,0x70,                                                             //电容上刻度标定高度5
    
    0x00,0x64,0x00,0x64,                                                    //空电容（没有放入液体时的电容）                                                         
    0x00                                                                   //满值报错复位次数
};

//****************************************************************************************************************************************************************
// 名称               :SwitchCurFilter()
// 创建日期           :2017-05-10
// 作者               :庄明群
// 功能               :切换滤波水平函数
// 输入参数           :filtfactor(滤波系数)
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :滤波水平分9级，数字从低到高对应滤波深度从低到高
// 修改内容           :
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
  if(UserParam.FilDivideFactory > 12)        //不能超过滤波数组的长度
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
// 名称               :Eeprom_Write3T_MultiBytes()
// 创建日期           :2017-05-03
// 作者               :庄明群
// 功能               :向STM32L072xx内部EEPROM指定位置写多个字节且备份
// 输入参数           :EepAddr(写起始地址)，pWrbuf(固定数据内存),Wrlen(写数据长度)
// 输出参数           :无
// 返回结果           :OP_SUCCESS(写入成功)，OP_FAILED(写入失败)
// 注意和说明         :写参数前请打开EEPROM写参数使能；地址不能越界
// 修改内容           :
//****************************************************************************************************************************************************************

uint8_t Eeprom_Write3T_MultiBytes(uint16_t EepAddr, uint8_t const *pWrbuf, uint16_t Wrlen)
{
  //错误状态
  uint8_t Err;
  //写状态
  uint8_t Wrsta;
  //EEPROM偏移
  uint16_t EpCnt;
  //写起始地址
  uint16_t EepAddress;
  
  Err = OP_SUCCESS;
  Wrsta = OP_SUCCESS;
  EepAddress = EepAddr;
  
  if(EEPROM_WR_ENABLE != ProductParam.EepromWrEn)
  {
    return OP_FAILED;
  }
  
  //待写数据长度
  for(EpCnt = 0; EpCnt < Wrlen; EpCnt++)
  {
    //系统参数存储区
    Wrsta = Eeprom_Write_OneByte((EepAddress + EpCnt), *(pWrbuf + EpCnt));
    if(OP_SUCCESS != Wrsta)
    {
      Err = OP_FAILED;
    }
    //系统参数备份区1
    Wrsta = Eeprom_Write_OneByte((SYSTEM_PARAM_BAK1 + EepAddress + EpCnt), *(pWrbuf + EpCnt));
    if(OP_SUCCESS != Wrsta)
    {
      Err = OP_FAILED;
    }
    //系统参数备份区2
    Wrsta = Eeprom_Write_OneByte((SYSTEM_PARAM_BAK2 + EepAddress + EpCnt), *(pWrbuf + EpCnt));
    if(OP_SUCCESS != Wrsta)
    {
      Err = OP_FAILED;
    }
  }
  return Err;
}

//****************************************************************************************************************************************************************
// 名称               :Eeprom_SystemParam_Check()
// 创建日期           :2017-05-03
// 作者               :庄明群
// 功能               :EEPROM系统参数检查
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :写参数前请打开EEPROM写参数使能；地址不能越界
// 修改内容           :
//****************************************************************************************************************************************************************

void Eeprom_SystemParam_Check(void)
{
  uint16_t Cnt;
  uint8_t Check_Sta;
  uint8_t Cur_Param_Bak1;
  uint8_t Cur_Param_Bak2;
  
  for(Cnt = 0; Cnt < PRO_DEFAULT_LEN; Cnt++)
  {
    //检查状态
    Check_Sta = 0;
    //备份1
    Cur_Param_Bak1 = Eeprom_Read_OneByte((SYSTEM_PARAM_BAK1 + Cnt));
    //备份2
    Cur_Param_Bak2 = Eeprom_Read_OneByte((SYSTEM_PARAM_BAK2 + Cnt));
    //当前参数与备份1不同
    if(Cur_Param[Cnt] != Cur_Param_Bak1)
    {
      Check_Sta |= 0x01;
    }
    //备份1与备份2不同
    if(Cur_Param_Bak1 != Cur_Param_Bak2)
    {
      Check_Sta |= 0x02;
    }
    //当前参数与备份2不同
    if(Cur_Param[Cnt] != Cur_Param_Bak2)
    {
      Check_Sta |= 0x04;
    }
    //备份2有误
    if(0x06 == Check_Sta)
    {
      Eeprom_Write_OneByte((SYSTEM_PARAM_BAK2 + Cnt), Cur_Param[Cnt]);
    }
    //备份1有误
    else if(0x03 == Check_Sta)
    {
      Eeprom_Write_OneByte((SYSTEM_PARAM_BAK1 + Cnt), Cur_Param[Cnt]);
    }
    //当前参数有误
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
  //如果三份数据全不对就写入缺省系统参数
  if((0x01 == Check_Sta) || (0x02 == Check_Sta) || (0x04 == Check_Sta) || (0x07 == Check_Sta))
  {
    //写入系统参数缺省值到三份地址区
    Eeprom_Write3T_MultiBytes(RUN_ADDR_BASE, User_Default_Param, PRO_DEFAULT_LEN);
    //读出当前系统参数
    Eeprom_Read_MultiBytes(RUN_ADDR_BASE, Cur_Param, PRO_DEFAULT_LEN);
  }
}


//****************************************************************************************************************************************************************
// 名称               :Read_Device_Param()
// 创建日期           :2017-05-10
// 作者               :庄明群
// 功能               :读取设备参数
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :
// 修改内容           :
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

    if((0x01 > UsartParam.DeviceAddr) || (0xF7 < UsartParam.DeviceAddr))        //不在设备地址范围则重置为1
    {
        UsartParam.DeviceAddr = 0x01;
        Eeprom_Write3T_MultiBytes(DEVICE_ADDR, &UsartParam.DeviceAddr, 1);
    }
    UsartParam.BaudRate = Cur_Param[BAUDRATE];
    if((UsartParam.BaudRate == 0) || (UsartParam.BaudRate > 0x07))              //不在波特率允许范围,则重置03
    {
        UsartParam.BaudRate = 0x03;
        Eeprom_Write3T_MultiBytes(BAUDRATE, &UsartParam.BaudRate, 1);
    }
    UsartParam.Parity = Cur_Param[PARITY];

    if((UsartParam.Parity == 0) || (UsartParam.Parity > 0x03))                  //无效校验方式则重置为0                                                
    {
        UsartParam.Parity = 0x03;
        Eeprom_Write3T_MultiBytes(PARITY, &UsartParam.Parity, 1);
    }

    UsartParam.AutoUpload = Cur_Param[AUTO_UPLOAD];                             //自动上传周期代号

    if((UsartParam.AutoUpload == 0) || (UsartParam.AutoUpload > 4))
    {
        UsartParam.AutoUpload = 2;
    }

    UserParam.Filterfactor = Cur_Param[FILTER];                                 //滤波系数

    if(0x09 < UserParam.Filterfactor)                                           //无效滤波系数则重置1
    {
        UserParam.Filterfactor = 1;
        Eeprom_Write3T_MultiBytes(FILTER, &UserParam.Filterfactor, 1);
    }

    SwitchCurFilter(UserParam.Filterfactor);                                    //切换滤波

    UserParam.Freeze = Cur_Param[FREEZE];
    if((0x0F != UserParam.Freeze) && (0x00 != UserParam.Freeze))
    {
        UserParam.Freeze = 0x00;
        Eeprom_Write3T_MultiBytes(FREEZE, &UserParam.Freeze, 1);
    }
    //输出方式，暂时保留
    UserParam.OutputMode = Cur_Param[OUTPUTMODE];
    UserParam.LiquidType = Cur_Param[LIQUIDTYPE];
    if(UserParam.LiquidType == 0 || UserParam.LiquidType > 5)
    {
        UserParam.LiquidType = 1;
    }

    //补偿使能
    ProductParam.CompenEn = Cur_Param[COMPENSATE];
    //补偿使能只有两种0和1，无效重置0
    if(0x01 < ProductParam.CompenEn)
    {
        ProductParam.CompenEn = 0x00;
        Eeprom_Write3T_MultiBytes(COMPENSATE, &ProductParam.CompenEn, 1);
    }
    
    UserParam.Freeze = Cur_Param[FREEZE];
       
    //写参数禁止
    ProductParam.EepromWrEn = EEPROM_WR_DISABLE;

    ParaTemp = (uint16_t)(Cur_Param[CORRECT_K] << 8) | Cur_Param[CORRECT_K + 1];
    //电容修正系数K
    UserParam.Correct_K = ParaTemp / 100.0;

    ParaTemp = (uint16_t)(Cur_Param[CORRECT_B] << 8) | Cur_Param[CORRECT_B + 1];
    //电容修正系数B
    UserParam.Correct_B = ParaTemp;

    ParaTemp = (uint16_t)(Cur_Param[TEMPER_K1] << 8) | Cur_Param[TEMPER_K1 + 1];
    //温度1修正系数K1
    UserParam.Temper_K1 = ParaTemp / 100.0;

    ParaTemp = (uint16_t)(Cur_Param[TEMPER_B1] << 8) | Cur_Param[TEMPER_B1 + 1];
    //温度1修正系数B1
    UserParam.Temper_B1 = ParaTemp / 100.0;

    //ParaTemp = (uint16_t)(Cur_Param[TEMPER_K2] << 8) | Cur_Param[TEMPER_K2 + 1];
    //温度2修正系数K2
    //UserParam.Temper_K2 = ParaTemp / 100.0;

    //ParaTemp = (uint16_t)(Cur_Param[TEMPER_B2] << 8) | Cur_Param[TEMPER_B2 + 1];
    //温度2修正系数B2
    //UserParam.Temper_B2 = ParaTemp / 100.0;
    //量程mm 保留
    ProductParam.HeightRange = (uint16_t)(Cur_Param[HEIGHTRANGE] << 8) | Cur_Param[HEIGHTRANGE + 1];
    //电容AD值零点
    ProductParam.CapADMin = (uint16_t)(Cur_Param[CAPADMIN] << 8) | Cur_Param[CAPADMIN + 1];
    //电容AD值下刻度
    ProductParam.CapADLow = (uint16_t)(Cur_Param[CAPADLOW] << 8) | Cur_Param[CAPADLOW + 1];
    //电容AD值上刻度
    ProductParam.CapADHih = (uint16_t)(Cur_Param[CAPADHIH] << 8) | Cur_Param[CAPADHIH + 1];
    //电容AD值满点
    ProductParam.CapADMax = (uint16_t)(Cur_Param[CAPADMAX] << 8) | Cur_Param[CAPADMAX + 1];
    //电容AD值量程
    ProductParam.CapADRange = ProductParam.CapADMax - ProductParam.CapADMin;
    //电容DA值零点
    ProductParam.CapDAMin = (uint16_t)(Cur_Param[CAPDAMIN] << 8) | Cur_Param[CAPDAMIN + 1];
    //电容DA值下刻度
    ProductParam.CapDALow = (uint16_t)(Cur_Param[CAPDALOW] << 8) | Cur_Param[CAPDALOW + 1];
    //电容DA值上刻度
    ProductParam.CapDAHih = (uint16_t)(Cur_Param[CAPDAHIH] << 8) | Cur_Param[CAPDAHIH + 1];
    //电容DA值满点
    ProductParam.CapDAMax = (uint16_t)(Cur_Param[CAPDAMAX] << 8) | Cur_Param[CAPDAMAX + 1];
    //电容DA值量程
    ProductParam.CapDARange = ProductParam.CapDAMax - ProductParam.CapDAMin;
    //温度DA值零点
    ProductParam.TempDAMin = (uint16_t)(Cur_Param[TEMPDAMIN] << 8) | Cur_Param[TEMPDAMIN + 1];
    //温度DA值满点
    ProductParam.TempDAMax = (uint16_t)(Cur_Param[TEMPDAMAX] << 8) | Cur_Param[TEMPDAMAX + 1];
    //温度DA值量程
    ProductParam.TempDARange = ProductParam.TempDAMax - ProductParam.TempDAMin;
    //电容零点
    
    ProductParam.CapMin = HexToUlong(&Cur_Param[CAPMIN1 + (UserParam.LiquidType - 1) * 16]);
    //电容下刻度
    ProductParam.CapLow = HexToUlong(&Cur_Param[CAPLOW1 + (UserParam.LiquidType - 1) * 16]);
    //电容上刻度
    ProductParam.CapHigh = HexToUlong(&Cur_Param[CAPHIGH1 + (UserParam.LiquidType - 1) * 16]);
    //电容满点
    ProductParam.CapMax = HexToUlong(&Cur_Param[CAPMAX1 + (UserParam.LiquidType - 1) * 16]);
    
    //电容下刻度标定高度
    ProductParam.CapLowHeight = (uint16_t)(Cur_Param[CAPLOWHEIGHT1 + (UserParam.LiquidType - 1) * 4] << 8) 
                                | Cur_Param[CAPLOWHEIGHT1 + (UserParam.LiquidType - 1) * 4 + 1];
    //电容上刻度标定高度
    ProductParam.CapHighHeight = (uint16_t)(Cur_Param[CAPHIGHHEIGHT1 + (UserParam.LiquidType - 1) * 4] << 8) 
                                | Cur_Param[CAPHIGHHEIGHT1 + (UserParam.LiquidType - 1) * 4 + 1];
    
    //电容零点备份
    //ProductParam.CapMinBak = HexToUlong(&Cur_Param[CAPMINBAK]);
    //电容满点备份
    //ProductParam.CapMaxBak = HexToUlong(&Cur_Param[CAPMAXBAK]);
    //电容量程
    ProductParam.CapRange = ProductParam.CapMax - ProductParam.CapMin;
    //电容波动限定值
    //ProductParam.CapWaveLimit = (uint32_t)(ProductParam.CapRange * 0.03f);
    
    RunVarParam.EmptyCap = HexToUlong(&Cur_Param[CAP_EMPTY]);
}

