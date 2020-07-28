/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        stm32l0_eeprom_app.h
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

#ifndef __STM32L0_EEPROM_APP_H
#define __STM32L0_EEPROM_APP_H

#include "stm32l0_eeprom.h"
#include "TypeConvert.h"

#define RUN_ADDR_BASE                     0x00                                  //ϵͳ������ʼ����־
#define DEVICE_ADDR                       (RUN_ADDR_BASE + 0x01)                //�豸��ַ
#define BAUDRATE                          (DEVICE_ADDR   + 0x01)                //������
#define PARITY                            (BAUDRATE      + 0x01)                //��żУ��
#define FILTER                            (PARITY        + 0x01)                //�˲�ϵ��
#define AUTO_UPLOAD                       (FILTER        + 0x01)                //�Զ��ϴ�����
#define COMPENSATE                        (AUTO_UPLOAD   + 0x01)                //����ʹ��
#define FREEZE                            (COMPENSATE    + 0x01)                //�Ƿ񶳽��豸
#define OUTPUTMODE                        (FREEZE        + 0x01)                //�����ʽ

#define USER_DEFAULT_LEN                  (OUTPUTMODE    + 0x01)                //Ĭ���û����ݳ���

#define LIQUIDTYPE                        (OUTPUTMODE    + 0x01)                //���ʣ���ҩ���ͣ�    
#define HEIGHTRANGE                       (LIQUIDTYPE    + 0x01)                //����
#define CAPMIN1                           (HEIGHTRANGE   + 0x02)                //������
#define CAPMAX1                           (CAPMIN1       + 0x04)                //�����̵���
#define CAPMIN2                           (CAPMAX1       + 0x04)                //������
#define CAPMAX2                           (CAPMIN2       + 0x04)                //�����̵���
#define CAPMIN3                           (CAPMAX2       + 0x04)                //������
#define CAPMAX3                           (CAPMIN3       + 0x04)                //�����̵���
#define CAPMIN4                           (CAPMAX3       + 0x04)                //������
#define CAPMAX4                           (CAPMIN4       + 0x04)                //�����̵���
#define CAPMIN5                           (CAPMAX4       + 0x04)                //������
#define CAPMAX5                           (CAPMIN5       + 0x04)                //�����̵���
#define CAPADMIN                          (CAPMAX5       + 0x04)                //�������ADֵ
#define CAPADLOW                          (CAPADMIN      + 0x02)                //�����¿̶�ADֵ
#define CAPADHIH                          (CAPADLOW      + 0x02)                //�����Ͽ̶�ADֵ
#define CAPADMAX                          (CAPADHIH      + 0x02)                //����������ADֵ
#define CAPDAMIN                          (CAPADMAX      + 0x02)                //�������DAֵ
#define CAPDALOW                          (CAPDAMIN      + 0x02)                //�����¿̶�DAֵ
#define CAPDAHIH                          (CAPDALOW      + 0x02)                //�����Ͽ̶�DAֵ
#define CAPDAMAX                          (CAPDAHIH      + 0x02)                //����������DAֵ
#define TEMPDAMIN                         (CAPDAMAX      + 0x02)                //�¶����DAֵ
#define TEMPDAMAX                         (TEMPDAMIN     + 0x02)                //�¶�������DAֵ
#define CORRECT_K                         (TEMPDAMAX     + 0x02)                //��������ϵ��K
#define CORRECT_B                         (CORRECT_K     + 0x02)                //��������ϵ��B
#define TEMPER_K1                         (CORRECT_B     + 0x02)                //�¶�1����ϵ��K1
#define TEMPER_B1                         (TEMPER_K1     + 0x02)                //�¶�1����ϵ��B1
#define CAP_EMPTY                         (TEMPER_B1     + 0x04)                //�յ���              

#define RESET_CNT                          (CAP_EMPTY     + 0x04)                //��ֵ����λ����

#define PRO_DEFAULT_LEN                   (RESET_CNT - RUN_ADDR_BASE + 0x01)    //ϵͳ�������� 58 bytes

#define SYSTEM_PARAM_BAK1                 (RUN_ADDR_BASE     + 0x100)           //ϵͳ��������1
#define SYSTEM_PARAM_BAK2                 (SYSTEM_PARAM_BAK1 + 0x100)           //ϵͳ��������2

#define ORGANIZATION                       0x0400                               //��֯��������
#define PRODUCTION                        (ORGANIZATION  + 0x30)                //��Ʒ����
#define HARDWAREVER                       (PRODUCTION    + 0x30)                //Ӳ���汾
#define SOFTWAREVER                       (HARDWAREVER   + 0x30)                //����汾
#define DEVICENUM                         (SOFTWAREVER   + 0x20)                //�豸ID
#define CUSTOMER                          (DEVICENUM     + 0x30)                //�ͻ�����



extern UserParaTypeDef UserParam;                                              //�û�����
extern UsartParaTypeDef UsartParam;                                            //����Usart����
extern RunVarParaTypeDef RunVarParam;                                          //ϵͳ���в���
extern ProductParaTypeDef ProductParam;                                        //��Ʒ����
extern FLASH_ProcessTypeDef ProcFlash;                                         //Flash����ṹ��
extern uint8_t Cur_Param[PRO_DEFAULT_LEN];                                     //ϵͳ����
extern const uint8_t SensorSoftVersion[8];                                    //����汾

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

void SwitchCurFilter(uint8_t filtfactor);


//****************************************************************************************************************************************************************
// ����               :Eeprom_Write3T_MultiBytes()
// ��������           :2017-05-03
// ����               :ׯ��Ⱥ
// ����               :��STM32L072xx�ڲ�EEPROMָ��λ��д����ֽ��ұ���
// �������           :EepAddr(д��ʼ��ַ)��pWrbuf(�̶������ڴ�),Wrlen(д���ݳ���)
// �������           :��
// ���ؽ��           :0(д��ɹ�)��1(д��ʧ��)
// ע���˵��         :д����ǰ���EEPROMд����ʹ�ܣ���ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

uint8_t Eeprom_Write3T_MultiBytes(uint16_t EepAddr, uint8_t const *pWrbuf, uint16_t Wrlen);

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

void Eeprom_SystemParam_Check(void);

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

void Read_Device_Param(void);

#endif
