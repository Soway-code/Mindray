/****************************************************************************************************************************************************************
** ��Ȩ:          2017-2027,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        stm32l0_eeprom.c
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

#include "stm32l0_eeprom.h"

UserParaTypeDef UserParam;                                                      //�û�����
UsartParaTypeDef UsartParam;                                                    //����Usart����
RunVarParaTypeDef RunVarParam;                                                  //ϵͳ���в���
ProductParaTypeDef ProductParam;                                                //��Ʒ����
//****************************************************************************************************************************************************************
// ����               :Eeprom_Write_OneByte()
// ��������           :2017-05-03
// ����               :ׯ��Ⱥ
// ����               :��STM32L072xx�ڲ�EEPROMָ��λ��д��һ�ֽ�����
// �������           :EepAddr��д���ַ����WrData��д�����ݣ�
// �������           :��
// ���ؽ��           :OP_SUCCESS��д��ɹ�����OP_FAILED��д��ʧ�ܣ�
// ע���˵��         :д����ǰ���EEPROMд����ʹ�ܣ���ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

uint8_t Eeprom_Write_OneByte(uint16_t EepAddr, uint8_t WrData)
{
  uint32_t WrAddr;
  HAL_StatusTypeDef Halstatus;
  
  if((EEPROM_END < EepAddr) ||(EEPROM_WR_ENABLE != ProductParam.EepromWrEn))
  {
    return OP_FAILED;
  }
  WrAddr = 0x08080000;
  WrAddr |= EepAddr;
  assert_param(IS_FLASH_DATA_ADDRESS(WrAddr));
  HAL_FLASHEx_DATAEEPROM_Unlock();
  Halstatus = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
  if(HAL_OK != Halstatus)
  {
   return OP_FAILED;
  }
  __disable_irq();
  Halstatus = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, WrAddr, WrData);
  Halstatus = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
  HAL_FLASHEx_DATAEEPROM_Lock();
  Halstatus = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
  __enable_irq();
  if(HAL_OK != Halstatus)
  {
   return OP_FAILED;
  }
  
  return OP_SUCCESS;
}

//****************************************************************************************************************************************************************
// ����               :Eeprom_Read_OneByte()
// ��������           :2017-05-03
// ����               :ׯ��Ⱥ
// ����               :��STM32L072xx�ڲ�EEPROMָ��λ�ö�ȡһ�ֽ�����
// �������           :EepAddr����ȡ��ַ��
// �������           :��
// ���ؽ��           :�����ֽ�
// ע���˵��         :��ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

uint8_t Eeprom_Read_OneByte(uint16_t EepAddr)
{
  uint8_t RdData;
  uint32_t RdAddr;
  
  RdData = 0;
  RdAddr = 0x08080000;
  RdAddr |= EepAddr;
  assert_param(IS_FLASH_DATA_ADDRESS(RdAddr));
  RdData = *(__IO uint8_t *)RdAddr;
  return RdData;
}

//****************************************************************************************************************************************************************
// ����               :Eeprom_EraseOneByte()
// ��������           :2017-05-03
// ����               :ׯ��Ⱥ
// ����               :��STM32L072xx�ڲ�EEPROMָ��λ�ò���һ�ֽ�
// �������           :EepAddr��������ַ��
// �������           :��
// ���ؽ��           :OP_SUCCESS(�����ɹ�)��OP_FAILED(����ʧ��)
// ע���˵��         :��������ǰ���EEPROMд����ʹ�ܣ���ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

uint8_t Eeprom_EraseOneByte(uint16_t EepAddr)
{
  uint32_t EraseAddr;
  HAL_StatusTypeDef status = HAL_OK;
  
  if((EEPROM_END < EepAddr) || (EEPROM_WR_ENABLE != ProductParam.EepromWrEn))
  {
    return OP_FAILED;
  }
  EraseAddr = 0x08080000;
  EraseAddr |= EepAddr;
  HAL_FLASHEx_DATAEEPROM_Unlock();
  assert_param(IS_FLASH_DATA_ADDRESS(EraseAddr));
  status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
  if(status == HAL_OK)
  {
    ProcFlash.ErrorCode = HAL_FLASH_ERROR_NONE;
    *(__IO uint8_t *) EraseAddr = 0x00;
    status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
  }
  HAL_FLASHEx_DATAEEPROM_Lock();
  if(HAL_OK != status)
  {
    return OP_FAILED;
  }
  
  return OP_SUCCESS;
}

//****************************************************************************************************************************************************************
// ����               :Eeprom_Write_MultiBytes()
// ��������           :2017-05-03
// ����               :ׯ��Ⱥ
// ����               :��STM32L072xx�ڲ�EEPROMָ��λ��д����ֽ�
// �������           :EepAddr(д��ʼ��ַ)��pWrbuf(�̶������ڴ�),Wrlen(д���ݳ���)
// �������           :��
// ���ؽ��           :OP_SUCCESS(д��ɹ�)��OP_FAILED(д��ʧ��)
// ע���˵��         :д����ǰ���EEPROMд����ʹ�ܣ���ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

uint8_t Eeprom_Write_MultiBytes(uint16_t EepAddr, uint8_t const *pWrbuf, uint16_t Wrlen)
{
  uint8_t Wrsta;
  uint16_t EpCnt;
  uint16_t EepAddress;
  
  Wrsta = OP_SUCCESS;
  EepAddress = EepAddr;
  for(EpCnt = 0; EpCnt < Wrlen; EpCnt++)
  {
    Wrsta = Eeprom_Write_OneByte(EepAddress, *(pWrbuf + EpCnt));
    if(OP_SUCCESS != Wrsta)
    {
      return OP_FAILED;
    }
    EepAddress++;
  }
  return OP_SUCCESS;
}

//****************************************************************************************************************************************************************
// ����               :Eeprom_Read_MultiBytes()
// ��������           :2017-05-03
// ����               :ׯ��Ⱥ
// ����               :��STM32L072xx�ڲ�EEPROMָ��λ�ö�����ֽ�
// �������           :EepAddr(����ʼ��ַ)��Rdlen(�����ݳ���)
// �������           :pRdbuf(���ݶ��ڴ�)
// ���ؽ��           :��
// ע���˵��         :��ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

void Eeprom_Read_MultiBytes(uint16_t EepAddr, uint8_t *pRdbuf, uint16_t Rdlen)
{
  uint16_t EpCnt;
  uint16_t EepAddress;
  
  EepAddress = EepAddr;
  for(EpCnt = 0; EpCnt < Rdlen; EpCnt++)
  {
    *(pRdbuf + EpCnt) = Eeprom_Read_OneByte(EepAddress);
    
    EepAddress++;
  }
}

//****************************************************************************************************************************************************************
// ����               :Eeprom_Erase_MultiBytes()
// ��������           :2017-05-03
// ����               :ׯ��Ⱥ
// ����               :��STM32L072xx�ڲ�EEPROMָ��λ�ò������ֽ�
// �������           :EepAddr(������ʼ��ַ)��Eslen(��������)
// �������           :��
// ���ؽ��           :OP_SUCCESS(�����ɹ�)��OP_FAILED(����ʧ��)
// ע���˵��         :��������ǰ���EEPROMд����ʹ�ܣ���ַ����Խ��
// �޸�����           :
//****************************************************************************************************************************************************************

uint8_t Eeprom_Erase_MultiBytes(uint16_t EepAddr, uint16_t Eslen)
{
  uint8_t Err;
  uint8_t Esta;
  uint16_t EpCnt;
  uint16_t EepAddress;
  
  Err = OP_SUCCESS;
  Esta = OP_SUCCESS;
  EepAddress = EepAddr;
  for(EpCnt = 0; EpCnt < Eslen; EpCnt++)
  {
    Esta = Eeprom_EraseOneByte(EepAddress);
    if(OP_SUCCESS != Esta)
    {
      Err = OP_FAILED;
    }
    EepAddress++;
  }
  
  return Err;
}
