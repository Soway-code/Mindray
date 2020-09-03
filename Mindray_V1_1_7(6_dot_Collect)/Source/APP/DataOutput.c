/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        DataOutput.c
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-22
** 描述:          电容数据输出
** 功能:          电容滤波、数据转换、数据输出
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#include "DataOutput.h"
#include "TypeConvert.h"
#include "math.h" 
#include "string.h"
#include "stm32l0_eeprom_app.h"
#include "stm32l0_usart_app.h"

uint8_t LiquidStableFlag;                                                       //液位稳定标志
uint8_t TimerCycleFlag;                                                         //定时器周期标志
uint8_t PCapDataOutFlag;                                                        //PCap数据输出标志
uint8_t LFilterCnt;                                                             //低滤波次数
uint32_t PCapData_OneSec;                                                       //1秒PCap数据
uint32_t PCapAvg_FrontOneSec;                                                   //前一个1s电容平均值
uint32_t PCapFilterData;                                                        //滤波数据
uint32_t PCapHFilterBuf[12];                                                    //高阶滤波缓存
uint32_t PCapLFilterBuf[80];                                                    //低阶滤波缓存
uint32_t PCapHFilterBufBak[12];                                                 //高阶滤波缓存临时备份
uint32_t PCapLFilterBufBak[80];                                                 //低阶滤波缓存临时备份                                                //低阶滤波缓存临时备份
uint32_t PCapResBuf[10];   

extern   uint8_t  cnt ,temp ;
uint8_t  DataError = 0;                                                       //数据出错计数
//****************************************************************************************************************************************************************
// 名称               :GetPCapResult()
// 创建日期           :2017-05-22
// 作者               :庄明群
// 功能               :获取PCap原始采集值
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :
// 修改内容           :
//****************************************************************************************************************************************************************

void GetPCapResult(void)
{
  uint32_t PCap_Status;
  //定时时间到采集电容值
  if(TIMER_CYCLE_FLAG == TimerCycleFlag)
  {
    //读取PCap状态
    PCap_Status = PCap_Res_Sta();
    if((PCap_Status & 0x100000))
    {
      RunVarParam.CapBak = PCap_Res_Capacitance();
                      if(RunVarParam.CapBak == 0x00FFFFFF)
            {   
                DataError++;
            }
            else
            {
                DataError = 0;           
            }
                    
      //PCap数据更新有效
      PCapDataOutFlag = PCAP_FRESH_FLAG;
      
      RunVarParam.ErrStatus &= 0xfff7;                                          //无采集故障，bit3置0
    }
    else
    {
        RunVarParam.ErrStatus = 0x0008;                                         //采集出现故障，bit3置1(采集出现故障则不存在超出范围)
    }
    
    PCap_Measure();
    TimerCycleFlag = 0;
    __HAL_TIM_CLEAR_IT(&TIM2_HandleInitStruct, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(&TIM2_HandleInitStruct);
  }
}

//****************************************************************************************************************************************************************
// 名称               :DataFilterAndOutput()
// 创建日期           :2017-05-22
// 作者               :庄明群
// 功能               :PCap采集值滤波与输出
// 输入参数           :无
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :
// 修改内容           :
//****************************************************************************************************************************************************************

////////////////////////////////////////////////////////////////////////////////////////////////////
  
  //sPCap_Diference：(L表示)         滤波队列头电容值与当前每秒电容值的差值
  
  //UserParam.FactorCycle：(T表示)   滤波周期表示液位从结束波动到恢复真实水平经历的时间
  
  //RunVarParam.CapVarfactor：(t表示)电容变化因子是滤波周期的行程变化量反映单调变化到滤波周期的一个过程
  
  //sPCap_VaryMin: (s表示)           单位滤波周期的电容变化量
  
  //计算公式：s = L / (T - t + 1)    从公式中可以得出液位稳定后滤波过程是等分逐步逼近真实液位值的

  //第一次滤波：s1 = L1 / (T - 1 + 1)     -> L2 = L1 - s1
  //                 ...
  //第二次滤波：s2 = L2 / (T - 2 + 1)     -> L3 = L2 - s2
  //                 ...
  //第三次滤波：s3 = L3 / (T - 3 + 1)     -> L4 = L3 - s3
  //                 ...
  //一直到最后       ...
/////////////////////////////////////////////////////////////////////////////////////////////////////

void DataFilterAndOutput(void)
{
    float Rate;
    uint8_t Cnt;                                                                          
    uint32_t PCapAvg_OneSec;                                                    //每秒电容平均值
    uint32_t PCapLFilterData;
    //uint64_t PCapFilterCalcuSum;                                              //滤波序列组电容累计和
    uint32_t PCapFilterOutPut;                                                  //滤波序列电容输出值
    static uint8_t Ms100_Cycle;                                                 //100ms周期计数
    if(PCAP_FRESH_FLAG != PCapDataOutFlag)                                      //有PCap数据事件
    {
        return;
    }

    PCapDataOutFlag = 0;
    RunVarParam.CapRes = RunVarParam.CapBak;  
    if(!((RunVarParam.CapRes >= ProductParam.CapMin) && (RunVarParam.CapRes <= ProductParam.CapMax)))
    {
        RunVarParam.ErrStatus |= 0x0080;                                        //超出测量范围，bit7置1   
    
        if(ProductParam.CapMin > RunVarParam.CapRes)                            //限制PCap值在量程范围
        {
            RunVarParam.CapRes = ProductParam.CapMin;
            RunVarParam.ErrStatus &= 0xffbf;                                    //低于最小，把bit5位置0 
            RunVarParam.ErrStatus |= 0x0010;                                    //低于最小，把bit4位置1
        }
        else if(ProductParam.CapMax < RunVarParam.CapRes)
        {
            RunVarParam.CapRes = ProductParam.CapMax;
            RunVarParam.ErrStatus &= 0xffef;                                    //高于最大，把bit4位置0 
            RunVarParam.ErrStatus |= 0x0020;                                    //高于最大，把bit5位置1
        }
    }
    else
    {
        RunVarParam.ErrStatus &= 0xff4f;                                        //在测量范围内，bit7置0    
    }

    if(INIT_FLAG != UserParam.FilterStart)                                      //滤波未开始先初始化滤波数组及其它参数       
    {
        Ms100_Cycle = 0;
        PCapData_OneSec = 0;
        PCapAvg_FrontOneSec = RunVarParam.CapRes;
        PCapFilterData = RunVarParam.CapRes;
        for(Cnt = 0; Cnt < 80; Cnt++)
        {
            PCapLFilterBuf[Cnt] = RunVarParam.CapRes;
        }
        for(Cnt = 0; Cnt < 12; Cnt++)
        {
            PCapHFilterBuf[Cnt] = RunVarParam.CapRes;
        }
        UserParam.FilterStart = INIT_FLAG;
        return;
    }

    PCapResBuf[Ms100_Cycle++] = RunVarParam.CapRes;
    if(Ms100_Cycle > CNT_MAX)                                                   //1秒时间到计算PCap平均值
    {
        PCapAvg_OneSec = GetDelExtremeAndAverage(PCapResBuf, Ms100_Cycle,2,5);
        Ms100_Cycle = 0;
    }
    else
    {
        return;
    }
                                                               
    if(UserParam.Filterfactor == 0)                                             //如果无滤波则直接给予每秒PCap平均值                                         
    {
        PCapFilterData = PCapAvg_OneSec;
    }
    else                                                                        //有滤波
    {  
        memcpy((uint8_t*)PCapLFilterBuf, (uint8_t*)PCapLFilterBuf + 4, (UserParam.FilterBufMax - 1) * 4);   
        *(PCapLFilterBuf + UserParam.FilterBufMax - 1) = PCapAvg_OneSec;         //新数据放在队尾
        //memcpy((uint8_t*)PCapLFilterBufBak, (uint8_t*)PCapLFilterBuf, UserParam.FilterBufMax * 4);//不能直接在原数组中排序
        //PCapFilterData = GetDelExtremeAndAverage(PCapLFilterBufBak, UserParam.FilterBufMax, 
                                                 //UserParam.FilterBufMax / 3, UserParam.FilterBufMax / 3);        
        PCapLFilterData = GetFilterAverage(PCapLFilterBuf, UserParam.FilterBufMax);
        if(LFilterCnt++ >= UserParam.FilterBufMax - 1)
        {
            LFilterCnt = 0;
            memcpy((uint8_t*)PCapHFilterBuf, (uint8_t*)PCapHFilterBuf + 4, (UserParam.FilDivideFactory - 1) * 4);
            
            *(PCapHFilterBuf + UserParam.FilDivideFactory - 1) = PCapLFilterData;
            //memcpy((uint8_t*)PCapHFilterBufBak, (uint8_t*)PCapHFilterBuf, UserParam.FilDivideFactory * 4);
            //PCapFilterData = GetDelExtremeAndAverage(PCapHFilterBufBak, UserParam.FilDivideFactory, 
                                                     //UserParam.FilDivideFactory / 3, UserParam.FilDivideFactory / 3);
        } 
        PCapFilterData = GetFilterAverage(PCapHFilterBuf, UserParam.FilDivideFactory);
    }
    
    //uprintf("PCapFilterData=%d\r\n",PCapFilterData);
    if(COMPENSATE_ENABLE == ProductParam.CompenEn)                              //补偿使能下修正K,B值
    {
        PCapFilterOutPut = (uint32_t)(PCapFilterData * UserParam.Correct_K + (UserParam.Correct_B - 100));
    }
    else
    {
        PCapFilterOutPut = PCapFilterData;
    }
                                                                                //计算电容比率
    if(ProductParam.CapLow == 0 || ProductParam.Cap40mm == 0 || ProductParam.Cap80mm == 0 || ProductParam.CapHigh == 0)
    {
        RunVarParam.LiquidHeightRate = (float)(PCapFilterOutPut - ProductParam.CapMin) / (float)ProductParam.CapRange;
        RunVarParam.OneSecLiquidHeightRate = (float)(PCapAvg_OneSec - ProductParam.CapMin) / (float)ProductParam.CapRange;
    }
    else
    {
        if(ProductParam.CapMin > PCapFilterOutPut)
        {
            RunVarParam.LiquidHeight = 0;
            RunVarParam.OneSecLiquidHeightRate = 0;
            RunVarParam.LiquidHeightRate = 0.0;
            RunVarParam.OneSecLiquidHeightRate = 0.0;
        }
        else if(ProductParam.CapMin <= PCapFilterOutPut && ProductParam.CapLow > PCapFilterOutPut)
        {
            Rate = (float)(PCapFilterOutPut - ProductParam.CapMin) / (float)(ProductParam.CapLow - ProductParam.CapMin);            
            RunVarParam.LiquidHeight = Rate * ProductParam.CapLowHeight;
            Rate = (float)(PCapAvg_OneSec - ProductParam.CapMin) / (float)(ProductParam.CapLow - ProductParam.CapMin);
            RunVarParam.OneSecLiquidHeight = Rate * ProductParam.CapLowHeight;
            RunVarParam.LiquidHeightRate = (float)RunVarParam.LiquidHeight / (float)ProductParam.HeightRange;
            RunVarParam.OneSecLiquidHeightRate = (float)RunVarParam.OneSecLiquidHeight / (float)ProductParam.HeightRange;
        }
        else if(ProductParam.CapLow <= PCapFilterOutPut && ProductParam.Cap40mm > PCapFilterOutPut)
        {
            Rate = (float)(PCapFilterOutPut - ProductParam.CapLow) / (float)(ProductParam.Cap40mm - ProductParam.CapLow);            
            RunVarParam.LiquidHeight = Rate * (ProductParam.Cap40mmHeight - ProductParam.CapLowHeight) + ProductParam.CapLowHeight;
            Rate = (float)(PCapAvg_OneSec - ProductParam.CapLow) / (float)(ProductParam.Cap40mm - ProductParam.CapLow);
            RunVarParam.OneSecLiquidHeight = Rate * (ProductParam.Cap40mmHeight - ProductParam.CapLowHeight) + ProductParam.CapLowHeight;
            RunVarParam.LiquidHeightRate = (float)RunVarParam.LiquidHeight / (float)ProductParam.HeightRange;
            RunVarParam.OneSecLiquidHeightRate = (float)RunVarParam.OneSecLiquidHeight / (float)ProductParam.HeightRange;
        }
        else if(ProductParam.Cap40mm <= PCapFilterOutPut && ProductParam.Cap80mm > PCapFilterOutPut)
        {
            Rate = (float)(PCapFilterOutPut - ProductParam.Cap40mm) / (float)(ProductParam.Cap80mm - ProductParam.Cap40mm);            
            RunVarParam.LiquidHeight = Rate * (ProductParam.Cap80mmHeight - ProductParam.Cap40mmHeight) + ProductParam.Cap40mmHeight;
            Rate = (float)(PCapAvg_OneSec - ProductParam.Cap40mm) / (float)(ProductParam.Cap80mm - ProductParam.Cap40mm);
            RunVarParam.OneSecLiquidHeight = Rate * (ProductParam.Cap80mmHeight - ProductParam.Cap40mmHeight) + ProductParam.Cap40mmHeight;
            RunVarParam.LiquidHeightRate = (float)RunVarParam.LiquidHeight / (float)ProductParam.HeightRange;
            RunVarParam.OneSecLiquidHeightRate = (float)RunVarParam.OneSecLiquidHeight / (float)ProductParam.HeightRange;
        }
        else if(ProductParam.Cap80mm <= PCapFilterOutPut && ProductParam.CapHigh > PCapFilterOutPut)
        {
            Rate = (float)(PCapFilterOutPut - ProductParam.Cap80mm) / (float)(ProductParam.CapHigh - ProductParam.Cap80mm);            
            RunVarParam.LiquidHeight = Rate * (ProductParam.CapHighHeight - ProductParam.Cap80mmHeight) + ProductParam.Cap80mmHeight;
            Rate = (float)(PCapAvg_OneSec - ProductParam.Cap80mm) / (float)(ProductParam.CapHigh - ProductParam.Cap80mm);
            RunVarParam.OneSecLiquidHeight = Rate * (ProductParam.CapHighHeight - ProductParam.Cap80mmHeight) + ProductParam.Cap80mmHeight;
            RunVarParam.LiquidHeightRate = (float)RunVarParam.LiquidHeight / (float)ProductParam.HeightRange;
            RunVarParam.OneSecLiquidHeightRate = (float)RunVarParam.OneSecLiquidHeight / (float)ProductParam.HeightRange;
        }
        else if(ProductParam.CapHigh <= PCapFilterOutPut && ProductParam.CapMax > PCapFilterOutPut)
        {
            Rate = (float)(PCapFilterOutPut - ProductParam.CapHigh) / (float)(ProductParam.CapMax - ProductParam.CapHigh);            
            RunVarParam.LiquidHeight = Rate * (ProductParam.HeightRange - ProductParam.CapHighHeight) + ProductParam.CapHighHeight;
            Rate = (float)(PCapAvg_OneSec - ProductParam.CapHigh) / (float)(ProductParam.CapMax - ProductParam.CapHigh);
            RunVarParam.OneSecLiquidHeight = Rate * (ProductParam.HeightRange - ProductParam.CapHighHeight) + ProductParam.CapHighHeight;
            RunVarParam.LiquidHeightRate = (float)RunVarParam.LiquidHeight / (float)ProductParam.HeightRange;
            RunVarParam.OneSecLiquidHeightRate = (float)RunVarParam.OneSecLiquidHeight / (float)ProductParam.HeightRange;
        }
        else if(ProductParam.CapMax <= PCapFilterOutPut)
        {        
            RunVarParam.LiquidHeight = ProductParam.HeightRange;
            RunVarParam.OneSecLiquidHeight = ProductParam.HeightRange;
            RunVarParam.LiquidHeightRate = 1.0;
            RunVarParam.OneSecLiquidHeightRate = 1.0;
        }
    }
    if(1.0 < RunVarParam.LiquidHeightRate)                                      //滤波电容比率限位
    {
        RunVarParam.LiquidHeightRate = 1.0;
    }
    else if(0.0 > RunVarParam.LiquidHeightRate)
    {
        RunVarParam.LiquidHeightRate = 0.0;
    }
    if(1.0 < RunVarParam.OneSecLiquidHeightRate)                                //每秒电容比率限位                     
    {
        RunVarParam.OneSecLiquidHeightRate = 1.0;
    }
    else if(0.0 > RunVarParam.OneSecLiquidHeightRate)
    {
        RunVarParam.OneSecLiquidHeightRate = 0.0;
    }    
                                                                                //转换成0--65535范围内的AD值
    RunVarParam.LiquidHeightAD = (uint16_t)(RunVarParam.LiquidHeightRate * 65535);
                                                                                //转换成液位高度
    if(ProductParam.CapLow == 0 || ProductParam.CapHigh == 0)
    {
        RunVarParam.LiquidHeight = (float)(RunVarParam.LiquidHeightRate * ProductParam.HeightRange);
        RunVarParam.OneSecLiquidHeight = (float)(RunVarParam.OneSecLiquidHeightRate * ProductParam.HeightRange);
    }
    
    if(fabs(RunVarParam.LiquidHeight - RunVarParam.OneSecLiquidHeight) < 1.5f)  //每秒的液位高度和最终液位高度差3mm以内开始上传数据
    {
        LiquidStableFlag = 1;                                                   //液位稳定，可以自动上传数据
    }
                                                                                
    if((0x0000 == ProductParam.CapADHih) || (0x0000 == ProductParam.CapDALow))
    {
        RunVarParam.CapDA_OutValue = (uint16_t)(RunVarParam.LiquidHeightRate * ProductParam.CapDARange) + ProductParam.CapDAMin;
    }
    else
    {                                                  
        if(RunVarParam.LiquidHeightAD < ProductParam.CapADMin)                  //AD值在零点AD以下     
        {
            RunVarParam.CapDA_OutValue = ProductParam.CapDAMin;
        }
                                                                                //AD值在零点AD和下刻度AD之间
        else if((ProductParam.CapADLow > RunVarParam.LiquidHeightAD) && (ProductParam.CapADMin <= RunVarParam.LiquidHeightAD))
        {
            Rate = (float)(RunVarParam.LiquidHeightAD - ProductParam.CapADMin) / (float)(ProductParam.CapADLow - ProductParam.CapADMin);
            RunVarParam.CapDA_OutValue = ProductParam.CapDALow - ProductParam.CapDAMin;
            RunVarParam.CapDA_OutValue = (uint16_t)(Rate * RunVarParam.CapDA_OutValue) + ProductParam.CapDAMin;
        }
                                                                                //AD值在下刻度AD值和上刻度AD值之间
        else if((ProductParam.CapADHih > RunVarParam.LiquidHeightAD) && (ProductParam.CapADLow <= RunVarParam.LiquidHeightAD))
        {
            Rate = (float)(RunVarParam.LiquidHeightAD - ProductParam.CapADLow) / (float)(ProductParam.CapADHih - ProductParam.CapADLow);
            RunVarParam.CapDA_OutValue = ProductParam.CapDAHih - ProductParam.CapDALow;
            RunVarParam.CapDA_OutValue = (uint16_t)(Rate * RunVarParam.CapDA_OutValue) + ProductParam.CapDALow;
        }
                                                                                //AD值在上刻度AD值和满量程AD值之间
        else if((ProductParam.CapADMax > RunVarParam.LiquidHeightAD) && (ProductParam.CapADHih <= RunVarParam.LiquidHeightAD))
        {
            Rate = (float)(RunVarParam.LiquidHeightAD - ProductParam.CapADHih) / (float)(ProductParam.CapADMax - ProductParam.CapADHih);
            RunVarParam.CapDA_OutValue = ProductParam.CapDAMax - ProductParam.CapDAHih;
            RunVarParam.CapDA_OutValue = (uint16_t)(Rate * RunVarParam.CapDA_OutValue) + ProductParam.CapDAHih;
        }                                                            
        else                                                                    //AD值在满量程以上          
        {
            RunVarParam.CapDA_OutValue = ProductParam.CapDAMax;
        }
    }
    RunVarParam.CapDA_Fresh_Flag = FRESH_OK;
}

//****************************************************************************************************************************************************************
// 名称               :HAL_TIM_PeriodElapsedCallback()
// 创建日期           :2017-05-22
// 作者               :庄明群
// 功能               :定时器周期循环回调函数
// 输入参数           :TIM_HandleTypeDef(定时器处理参数结构体)
// 输出参数           :无
// 返回结果           :无
// 注意和说明         :
// 修改内容           :
//****************************************************************************************************************************************************************

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(TIM2 == htim->Instance)            
  {
    TimerCycleFlag = TIMER_CYCLE_FLAG;//周期循环标志，100ms，用于周期性执行Pcap采集函数
    HAL_TIM_Base_Stop_IT(&TIM2_HandleInitStruct);
    __HAL_TIM_CLEAR_IT(&TIM2_HandleInitStruct, TIM_IT_UPDATE);
    if(1 != UsartParam.AutoUpload)
    {
      UsartParam.AutoUploadCn++;
    }
    else
    {
      UsartParam.AutoUploadCn = 0;
    }
  }
  /*
  if(TIM3 == htim->Instance)
  {
    UsartRecvOk = USART_RECV_TIMER_OK;//串口数据接收完成标志
    HAL_TIM_Base_Stop_IT(&TIM3_HandleInitStruct);
    __HAL_TIM_CLEAR_IT(&TIM3_HandleInitStruct, TIM_IT_UPDATE);
  }*/
}
