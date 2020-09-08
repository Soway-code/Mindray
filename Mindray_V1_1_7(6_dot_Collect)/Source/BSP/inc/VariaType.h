/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        VariaType.h
** ����:          ׯ��Ⱥ
** �汾:          V1.0.0
** ����:          2017-05-16
** ����:          ��������
** ����:          ģ�黯����
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#ifndef __VARIATYPE_H
#define __VARIATYPE_H

#define  OP_SUCCESS                  0x00                                       //�����ɹ�
#define  OP_FAILED                   0x01                                       //����ʧ��

#define  FRESH_NK                    0x00                                       //û�и���
#define  FRESH_OK                    0x01                                       //�������

#define  CNT_MAX                     0x09                                       //������
#define  INIT_FLAG                   0x80                                       //��ʼ������

#define  CALIB_CLEAR                 0x00                                       //�궨���

#define  COMPENSATE_ENABLE           0x01                                       //����ʹ��
#define  COMPENSATE_DISABLE          0x00                                       //������ֹ

#define  PCAP_FRESH_FLAG             0x0F                                       //PCAP��������

//����ͨ�Ų����ṹ��
typedef struct
{
  uint8_t DeviceAddr;                                                           //�豸��ַ
  uint8_t BaudRate;                                                             //������
  uint8_t Parity;                                                               //��żУ��
  uint8_t AutoUpload;                                                           //�Զ��ϴ����ڴ���
  uint16_t AutoUploadCn;                                                        //�Զ��ϴ�����
  //uint16_t AutoUploadCycle;                                                   //�Զ��ϴ�����
}UsartParaTypeDef;

//�û������ṹ��
typedef struct
{
  uint8_t LiquidType;                                                           //Һ������(��ҩ����)
  uint8_t Freeze;                                                               //�Ƿ񶳽�
  uint8_t FilterStart;                                                          //�˲���ʼ
  uint8_t Filterfactor;                                                         //�˲�ϵ��
  uint8_t OutputMode;                                                           //�����ʽ
  uint16_t FilterBufMax;                                                        //�˲��������ֵ
  uint16_t FactorCycle;                                                         //�˲�����
  uint16_t FilDivideFactory;                                                    //��������
  
  float Correct_K;                                                             //��������ϵ��K
  float Correct_B;                                                             //��������ϵ��B
  float Temper_K1;                                                             //�¶�1����ϵ��K1
  float Temper_B1;                                                             //�¶�1����ϵ��B1
  //float Temper_K2;                                                             //�¶�2����ϵ��K2
  //float Temper_B2;                                                             //�¶�2����ϵ��B2
}UserParaTypeDef;

//��Ʒ�����ṹ��
typedef struct
{
  uint8_t CompenEn;                                                             //����ʹ��
  uint8_t EepromWrEn;                                                           //EEPROMдʹ��
  uint8_t CapAD_ClibEn;                                                         //����AD�궨ʹ��
  uint8_t CapDA_ClibEn;                                                         //����DA�궨ʹ��
  uint8_t TempDA_ClibEn;                                                        //�¶�DA�궨ʹ��
  
  uint16_t CapADMin;                                                            //����ADֵ���
  uint16_t CapADLow;                                                            //����ADֵ�¿̶�
  uint16_t CapADHih;                                                            //����ADֵ�Ͽ̶�
  uint16_t CapADMax;                                                            //����ADֵ������
  uint16_t CapADRange;                                                          //����ADֵ����
  
  uint16_t CapDAMin;                                                            //����DAֵ���
  uint16_t CapDALow;                                                            //����DAֵ�¿̶�
  uint16_t CapDAHih;                                                            //����DAֵ�Ͽ̶�
  uint16_t CapDAMax;                                                            //����DAֵ������
  uint16_t CapDARange;                                                          //����DAֵ����
  uint16_t TempDAMin;                                                           //�¶�DAֵ���
  uint16_t TempDAMax;                                                           //�¶�DAֵ������
  uint16_t TempDARange;                                                         //�¶�DAֵ����
  
  uint32_t CapMin;                                                              //����1���
  //uint32_t CapMinBak;                                                         //������㱸��
  uint32_t CapMax;                                                              //����    ������
  //uint32_t CapMaxBak;                                                         //���������̱���
  uint32_t CapRange;                                                            //��������
  //uint32_t CapWaveLimit;                                                      //���ݲ����޶�ֵ
  uint32_t CapLow;                                                              //�����¿̶�
  uint32_t CapHigh;                                                             //�����Ͽ̶�
  uint32_t Cap40mm;                                                             //����40mm�̶�
  uint32_t Cap80mm;                                                             //����80mm�̶�
  
  uint32_t CapLowHeight;                                                        //�����¿̶ȱ궨�߶�
  uint32_t CapHighHeight;                                                       //�����Ͽ̶ȱ궨�߶�
  uint32_t Cap40mmHeight;                                                       //����40mm�̶ȱ궨�߶�
  uint32_t Cap80mmHeight;                                                       //����80mm�̶ȱ궨�߶�
  
  uint16_t HeightRange;                                                         //�߶�����
  
}ProductParaTypeDef;

//ϵͳ���в����ṹ��
typedef struct
{
  uint8_t CalibFlag;                                                            //���ݱ궨��־
  uint8_t CapAD_ClibFlag;                                                       //����AD�궨��־
  uint8_t CapDA_ClibFlag;                                                       //����DA�궨��־
  uint8_t TempDA_ClibFlag;                                                      //�¶�DA�궨��־
  uint8_t CapDA_Fresh_Flag;                                                     //����DA���±�־
  uint8_t Temper_Fresh_Flag;                                                    //�¶�ֵ���±�־
  
  uint16_t CapVarfactor;                                                        //���ݱ仯ϵ��
  uint16_t CapAD_ClibValue;                                                     //����AD�궨ֵ
  uint16_t CapDA_ClibValue;                                                     //����DA�궨ֵ
  uint16_t TempDA_ClibValue;                                                    //�¶�DA�궨ֵ
  uint16_t CapDA_OutValue;                                                      //����DA���ֵ
  uint16_t LiquidHeightAD;                                                      //Һλ�߶�ADֵ
  uint16_t TemperInAirAD;                                                       //�����¶�ADֵ
  uint16_t TemperInLiquidAD;                                                    //Һ���¶�ADֵ
  
  uint32_t CapRes;                                                              //����ԭʼֵ
  uint32_t CapBak;                                                              //����ԭʼֵ����
  uint32_t EmptyCap;                                                            //�յ���ֵ��û�з���Һ��ʱ�ĵ���ֵ��
  uint32_t ErrStatus;                                                           //����״̬
  float LiquidHeight;                                                          //Һλ�߶�
  float LiquidHeightRate;                                                      //Һλ�ٷֱ�
  float TemperInAir;                                                           //�����¶�
  float TemperInLiquid;                                                        //Һ���¶�
  
  float OneSecLiquidHeightRate;                                                //ÿ��Һλ�ٷֱ�
  float OneSecLiquidHeight;                                                    //ÿ��Һλ�߶�
}RunVarParaTypeDef;

#endif
