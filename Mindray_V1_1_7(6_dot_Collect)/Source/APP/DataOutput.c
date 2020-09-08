/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        DataOutput.c
** ����:          ׯ��Ⱥ
** �汾:          V1.0.0
** ����:          2017-05-22
** ����:          �����������
** ����:          �����˲�������ת�����������
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#include "DataOutput.h"
#include "TypeConvert.h"
#include "math.h" 
#include "string.h"
#include "stm32l0_eeprom_app.h"
#include "stm32l0_usart_app.h"

uint8_t LiquidStableFlag;                                                       //Һλ�ȶ���־
uint8_t TimerCycleFlag;                                                         //��ʱ�����ڱ�־
uint8_t PCapDataOutFlag;                                                        //PCap���������־
uint8_t LFilterCnt;                                                             //���˲�����
uint32_t PCapData_OneSec;                                                       //1��PCap����
uint32_t PCapAvg_FrontOneSec;                                                   //ǰһ��1s����ƽ��ֵ
uint32_t PCapFilterData;                                                        //�˲�����
uint32_t PCapHFilterBuf[12];                                                    //�߽��˲�����
uint32_t PCapLFilterBuf[80];                                                    //�ͽ��˲�����
uint32_t PCapHFilterBufBak[12];                                                 //�߽��˲�������ʱ����
uint32_t PCapLFilterBufBak[80];                                                 //�ͽ��˲�������ʱ����                                                //�ͽ��˲�������ʱ����
uint32_t PCapResBuf[10];   

extern   uint8_t  cnt ,temp ;
uint8_t  DataError = 0;                                                       //���ݳ������
//****************************************************************************************************************************************************************
// ����               :GetPCapResult()
// ��������           :2017-05-22
// ����               :ׯ��Ⱥ
// ����               :��ȡPCapԭʼ�ɼ�ֵ
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :
// �޸�����           :
//****************************************************************************************************************************************************************

void GetPCapResult(void)
{
  uint32_t PCap_Status;
  //��ʱʱ�䵽�ɼ�����ֵ
  if(TIMER_CYCLE_FLAG == TimerCycleFlag)
  {
    //��ȡPCap״̬
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
                    
      //PCap���ݸ�����Ч
      PCapDataOutFlag = PCAP_FRESH_FLAG;
      
      RunVarParam.ErrStatus &= 0xfff7;                                          //�޲ɼ����ϣ�bit3��0
    }
    else
    {
        RunVarParam.ErrStatus = 0x0008;                                         //�ɼ����ֹ��ϣ�bit3��1(�ɼ����ֹ����򲻴��ڳ�����Χ)
    }
    
    PCap_Measure();
    TimerCycleFlag = 0;
    __HAL_TIM_CLEAR_IT(&TIM2_HandleInitStruct, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(&TIM2_HandleInitStruct);
  }
}

//****************************************************************************************************************************************************************
// ����               :DataFilterAndOutput()
// ��������           :2017-05-22
// ����               :ׯ��Ⱥ
// ����               :PCap�ɼ�ֵ�˲������
// �������           :��
// �������           :��
// ���ؽ��           :��
// ע���˵��         :
// �޸�����           :
//****************************************************************************************************************************************************************

////////////////////////////////////////////////////////////////////////////////////////////////////
  
  //sPCap_Diference��(L��ʾ)         �˲�����ͷ����ֵ�뵱ǰÿ�����ֵ�Ĳ�ֵ
  
  //UserParam.FactorCycle��(T��ʾ)   �˲����ڱ�ʾҺλ�ӽ����������ָ���ʵˮƽ������ʱ��
  
  //RunVarParam.CapVarfactor��(t��ʾ)���ݱ仯�������˲����ڵ��г̱仯����ӳ�����仯���˲����ڵ�һ������
  
  //sPCap_VaryMin: (s��ʾ)           ��λ�˲����ڵĵ��ݱ仯��
  
  //���㹫ʽ��s = L / (T - t + 1)    �ӹ�ʽ�п��Եó�Һλ�ȶ����˲������ǵȷ��𲽱ƽ���ʵҺλֵ��

  //��һ���˲���s1 = L1 / (T - 1 + 1)     -> L2 = L1 - s1
  //                 ...
  //�ڶ����˲���s2 = L2 / (T - 2 + 1)     -> L3 = L2 - s2
  //                 ...
  //�������˲���s3 = L3 / (T - 3 + 1)     -> L4 = L3 - s3
  //                 ...
  //һֱ�����       ...
/////////////////////////////////////////////////////////////////////////////////////////////////////

void DataFilterAndOutput(void)
{
    float Rate;
    uint8_t Cnt;                                                                          
    uint32_t PCapAvg_OneSec;                                                    //ÿ�����ƽ��ֵ
    uint32_t PCapLFilterData;
    //uint64_t PCapFilterCalcuSum;                                              //�˲�����������ۼƺ�
    uint32_t PCapFilterOutPut;                                                  //�˲����е������ֵ
    static uint8_t Ms100_Cycle;                                                 //100ms���ڼ���
    if(PCAP_FRESH_FLAG != PCapDataOutFlag)                                      //��PCap�����¼�
    {
        return;
    }

    PCapDataOutFlag = 0;
    RunVarParam.CapRes = RunVarParam.CapBak;  
    if(!((RunVarParam.CapRes >= ProductParam.CapMin) && (RunVarParam.CapRes <= ProductParam.CapMax)))
    {
        RunVarParam.ErrStatus |= 0x0080;                                        //����������Χ��bit7��1   
    
        if(ProductParam.CapMin > RunVarParam.CapRes)                            //����PCapֵ�����̷�Χ
        {
            RunVarParam.CapRes = ProductParam.CapMin;
            RunVarParam.ErrStatus &= 0xffbf;                                    //������С����bit5λ��0 
            RunVarParam.ErrStatus |= 0x0010;                                    //������С����bit4λ��1
        }
        else if(ProductParam.CapMax < RunVarParam.CapRes)
        {
            RunVarParam.CapRes = ProductParam.CapMax;
            RunVarParam.ErrStatus &= 0xffef;                                    //������󣬰�bit4λ��0 
            RunVarParam.ErrStatus |= 0x0020;                                    //������󣬰�bit5λ��1
        }
    }
    else
    {
        RunVarParam.ErrStatus &= 0xff4f;                                        //�ڲ�����Χ�ڣ�bit7��0    
    }

    if(INIT_FLAG != UserParam.FilterStart)                                      //�˲�δ��ʼ�ȳ�ʼ���˲����鼰��������       
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
    if(Ms100_Cycle > CNT_MAX)                                                   //1��ʱ�䵽����PCapƽ��ֵ
    {
        PCapAvg_OneSec = GetDelExtremeAndAverage(PCapResBuf, Ms100_Cycle,2,5);
        Ms100_Cycle = 0;
    }
    else
    {
        return;
    }
                                                               
    if(UserParam.Filterfactor == 0)                                             //������˲���ֱ�Ӹ���ÿ��PCapƽ��ֵ                                         
    {
        PCapFilterData = PCapAvg_OneSec;
    }
    else                                                                        //���˲�
    {  
        memcpy((uint8_t*)PCapLFilterBuf, (uint8_t*)PCapLFilterBuf + 4, (UserParam.FilterBufMax - 1) * 4);   
        *(PCapLFilterBuf + UserParam.FilterBufMax - 1) = PCapAvg_OneSec;         //�����ݷ��ڶ�β
        //memcpy((uint8_t*)PCapLFilterBufBak, (uint8_t*)PCapLFilterBuf, UserParam.FilterBufMax * 4);//����ֱ����ԭ����������
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
    if(COMPENSATE_ENABLE == ProductParam.CompenEn)                              //����ʹ��������K,Bֵ
    {
        PCapFilterOutPut = (uint32_t)(PCapFilterData * UserParam.Correct_K + (UserParam.Correct_B - 100));
    }
    else
    {
        PCapFilterOutPut = PCapFilterData;
    }
                                                                                //������ݱ���
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
    if(1.0 < RunVarParam.LiquidHeightRate)                                      //�˲����ݱ�����λ
    {
        RunVarParam.LiquidHeightRate = 1.0;
    }
    else if(0.0 > RunVarParam.LiquidHeightRate)
    {
        RunVarParam.LiquidHeightRate = 0.0;
    }
    if(1.0 < RunVarParam.OneSecLiquidHeightRate)                                //ÿ����ݱ�����λ                     
    {
        RunVarParam.OneSecLiquidHeightRate = 1.0;
    }
    else if(0.0 > RunVarParam.OneSecLiquidHeightRate)
    {
        RunVarParam.OneSecLiquidHeightRate = 0.0;
    }    
                                                                                //ת����0--65535��Χ�ڵ�ADֵ
    RunVarParam.LiquidHeightAD = (uint16_t)(RunVarParam.LiquidHeightRate * 65535);
                                                                                //ת����Һλ�߶�
    if(ProductParam.CapLow == 0 || ProductParam.CapHigh == 0)
    {
        RunVarParam.LiquidHeight = (float)(RunVarParam.LiquidHeightRate * ProductParam.HeightRange);
        RunVarParam.OneSecLiquidHeight = (float)(RunVarParam.OneSecLiquidHeightRate * ProductParam.HeightRange);
    }
    
    if(fabs(RunVarParam.LiquidHeight - RunVarParam.OneSecLiquidHeight) < 1.5f)  //ÿ���Һλ�߶Ⱥ�����Һλ�߶Ȳ�3mm���ڿ�ʼ�ϴ�����
    {
        LiquidStableFlag = 1;                                                   //Һλ�ȶ��������Զ��ϴ�����
    }
                                                                                
    if((0x0000 == ProductParam.CapADHih) || (0x0000 == ProductParam.CapDALow))
    {
        RunVarParam.CapDA_OutValue = (uint16_t)(RunVarParam.LiquidHeightRate * ProductParam.CapDARange) + ProductParam.CapDAMin;
    }
    else
    {                                                  
        if(RunVarParam.LiquidHeightAD < ProductParam.CapADMin)                  //ADֵ�����AD����     
        {
            RunVarParam.CapDA_OutValue = ProductParam.CapDAMin;
        }
                                                                                //ADֵ�����AD���¿̶�AD֮��
        else if((ProductParam.CapADLow > RunVarParam.LiquidHeightAD) && (ProductParam.CapADMin <= RunVarParam.LiquidHeightAD))
        {
            Rate = (float)(RunVarParam.LiquidHeightAD - ProductParam.CapADMin) / (float)(ProductParam.CapADLow - ProductParam.CapADMin);
            RunVarParam.CapDA_OutValue = ProductParam.CapDALow - ProductParam.CapDAMin;
            RunVarParam.CapDA_OutValue = (uint16_t)(Rate * RunVarParam.CapDA_OutValue) + ProductParam.CapDAMin;
        }
                                                                                //ADֵ���¿̶�ADֵ���Ͽ̶�ADֵ֮��
        else if((ProductParam.CapADHih > RunVarParam.LiquidHeightAD) && (ProductParam.CapADLow <= RunVarParam.LiquidHeightAD))
        {
            Rate = (float)(RunVarParam.LiquidHeightAD - ProductParam.CapADLow) / (float)(ProductParam.CapADHih - ProductParam.CapADLow);
            RunVarParam.CapDA_OutValue = ProductParam.CapDAHih - ProductParam.CapDALow;
            RunVarParam.CapDA_OutValue = (uint16_t)(Rate * RunVarParam.CapDA_OutValue) + ProductParam.CapDALow;
        }
                                                                                //ADֵ���Ͽ̶�ADֵ��������ADֵ֮��
        else if((ProductParam.CapADMax > RunVarParam.LiquidHeightAD) && (ProductParam.CapADHih <= RunVarParam.LiquidHeightAD))
        {
            Rate = (float)(RunVarParam.LiquidHeightAD - ProductParam.CapADHih) / (float)(ProductParam.CapADMax - ProductParam.CapADHih);
            RunVarParam.CapDA_OutValue = ProductParam.CapDAMax - ProductParam.CapDAHih;
            RunVarParam.CapDA_OutValue = (uint16_t)(Rate * RunVarParam.CapDA_OutValue) + ProductParam.CapDAHih;
        }                                                            
        else                                                                    //ADֵ������������          
        {
            RunVarParam.CapDA_OutValue = ProductParam.CapDAMax;
        }
    }
    RunVarParam.CapDA_Fresh_Flag = FRESH_OK;
}

//****************************************************************************************************************************************************************
// ����               :HAL_TIM_PeriodElapsedCallback()
// ��������           :2017-05-22
// ����               :ׯ��Ⱥ
// ����               :��ʱ������ѭ���ص�����
// �������           :TIM_HandleTypeDef(��ʱ����������ṹ��)
// �������           :��
// ���ؽ��           :��
// ע���˵��         :
// �޸�����           :
//****************************************************************************************************************************************************************

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(TIM2 == htim->Instance)            
  {
    TimerCycleFlag = TIMER_CYCLE_FLAG;//����ѭ����־��100ms������������ִ��Pcap�ɼ�����
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
    UsartRecvOk = USART_RECV_TIMER_OK;//�������ݽ�����ɱ�־
    HAL_TIM_Base_Stop_IT(&TIM3_HandleInitStruct);
    __HAL_TIM_CLEAR_IT(&TIM3_HandleInitStruct, TIM_IT_UPDATE);
  }*/
}
