/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        VariaType.h
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-16
** 描述:          变量类型
** 功能:          模块化变量
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#ifndef __VARIATYPE_H
#define __VARIATYPE_H

#define  OP_SUCCESS                  0x00                                       //操作成功
#define  OP_FAILED                   0x01                                       //操作失败

#define  FRESH_NK                    0x00                                       //没有更新
#define  FRESH_OK                    0x01                                       //更新完成

#define  CNT_MAX                     0x09                                       //最大计数
#define  INIT_FLAG                   0x80                                       //初始化设置

#define  CALIB_CLEAR                 0x00                                       //标定清除

#define  COMPENSATE_ENABLE           0x01                                       //补偿使能
#define  COMPENSATE_DISABLE          0x00                                       //补偿禁止

#define  PCAP_FRESH_FLAG             0x0F                                       //PCAP更新数据

//串口通信参数结构体
typedef struct
{
  uint8_t DeviceAddr;                                                           //设备地址
  uint8_t BaudRate;                                                             //波特率
  uint8_t Parity;                                                               //奇偶校验
  uint8_t AutoUpload;                                                           //自动上传周期代号
  uint16_t AutoUploadCn;                                                        //自动上传计数
  //uint16_t AutoUploadCycle;                                                   //自动上传周期
}UsartParaTypeDef;

//用户参数结构体
typedef struct
{
  uint8_t LiquidType;                                                           //液体类型(麻药类型)
  uint8_t Freeze;                                                               //是否冻结
  uint8_t FilterStart;                                                          //滤波开始
  uint8_t Filterfactor;                                                         //滤波系数
  uint8_t OutputMode;                                                           //输出方式
  uint16_t FilterBufMax;                                                        //滤波缓存最大值
  uint16_t FactorCycle;                                                         //滤波周期
  uint16_t FilDivideFactory;                                                    //比例因子
  
  float Correct_K;                                                             //电容修正系数K
  float Correct_B;                                                             //电容修正系数B
  float Temper_K1;                                                             //温度1修正系数K1
  float Temper_B1;                                                             //温度1修正系数B1
  //float Temper_K2;                                                             //温度2修正系数K2
  //float Temper_B2;                                                             //温度2修正系数B2
}UserParaTypeDef;

//产品参数结构体
typedef struct
{
  uint8_t CompenEn;                                                             //补偿使能
  uint8_t EepromWrEn;                                                           //EEPROM写使能
  uint8_t CapAD_ClibEn;                                                         //电容AD标定使能
  uint8_t CapDA_ClibEn;                                                         //电容DA标定使能
  uint8_t TempDA_ClibEn;                                                        //温度DA标定使能
  
  uint16_t CapADMin;                                                            //电容AD值零点
  uint16_t CapADLow;                                                            //电容AD值下刻度
  uint16_t CapADHih;                                                            //电容AD值上刻度
  uint16_t CapADMax;                                                            //电容AD值满量程
  uint16_t CapADRange;                                                          //电容AD值量程
  
  uint16_t CapDAMin;                                                            //电容DA值零点
  uint16_t CapDALow;                                                            //电容DA值下刻度
  uint16_t CapDAHih;                                                            //电容DA值上刻度
  uint16_t CapDAMax;                                                            //电容DA值满量程
  uint16_t CapDARange;                                                          //电容DA值量程
  uint16_t TempDAMin;                                                           //温度DA值零点
  uint16_t TempDAMax;                                                           //温度DA值满量程
  uint16_t TempDARange;                                                         //温度DA值量程
  
  uint32_t CapMin;                                                              //电容1零点
  //uint32_t CapMinBak;                                                         //电容零点备份
  uint32_t CapMax;                                                              //电容    满量程
  //uint32_t CapMaxBak;                                                         //电容满量程备份
  uint32_t CapRange;                                                            //电容量程
  //uint32_t CapWaveLimit;                                                      //电容波动限定值
  uint32_t CapLow;                                                              //电容下刻度
  uint32_t CapHigh;                                                             //电容上刻度
  uint32_t Cap40mm;                                                             //电容40mm刻度
  uint32_t Cap80mm;                                                             //电容80mm刻度
  
  uint32_t CapLowHeight;                                                        //电容下刻度标定高度
  uint32_t CapHighHeight;                                                       //电容上刻度标定高度
  uint32_t Cap40mmHeight;                                                       //电容40mm刻度标定高度
  uint32_t Cap80mmHeight;                                                       //电容80mm刻度标定高度
  
  uint16_t HeightRange;                                                         //高度量程
  
}ProductParaTypeDef;

//系统运行参数结构体
typedef struct
{
  uint8_t CalibFlag;                                                            //电容标定标志
  uint8_t CapAD_ClibFlag;                                                       //电容AD标定标志
  uint8_t CapDA_ClibFlag;                                                       //电容DA标定标志
  uint8_t TempDA_ClibFlag;                                                      //温度DA标定标志
  uint8_t CapDA_Fresh_Flag;                                                     //电容DA更新标志
  uint8_t Temper_Fresh_Flag;                                                    //温度值更新标志
  
  uint16_t CapVarfactor;                                                        //电容变化系数
  uint16_t CapAD_ClibValue;                                                     //电容AD标定值
  uint16_t CapDA_ClibValue;                                                     //电容DA标定值
  uint16_t TempDA_ClibValue;                                                    //温度DA标定值
  uint16_t CapDA_OutValue;                                                      //电容DA输出值
  uint16_t LiquidHeightAD;                                                      //液位高度AD值
  uint16_t TemperInAirAD;                                                       //环境温度AD值
  uint16_t TemperInLiquidAD;                                                    //液体温度AD值
  
  uint32_t CapRes;                                                              //电容原始值
  uint32_t CapBak;                                                              //电容原始值备份
  uint32_t EmptyCap;                                                            //空电容值（没有放入液体时的电容值）
  uint32_t ErrStatus;                                                           //错误状态
  float LiquidHeight;                                                          //液位高度
  float LiquidHeightRate;                                                      //液位百分比
  float TemperInAir;                                                           //环境温度
  float TemperInLiquid;                                                        //液体温度
  
  float OneSecLiquidHeightRate;                                                //每秒液位百分比
  float OneSecLiquidHeight;                                                    //每秒液位高度
}RunVarParaTypeDef;

#endif
