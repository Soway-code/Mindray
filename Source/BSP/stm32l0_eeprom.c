/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        stm32l0_eeprom.c
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

#include "stm32l0_eeprom.h"

UserParaTypeDef UserParam;                                                      //用户参数
UsartParaTypeDef UsartParam;                                                    //串口Usart参数
RunVarParaTypeDef RunVarParam;                                                  //系统运行参数
ProductParaTypeDef ProductParam;                                                //产品参数
//****************************************************************************************************************************************************************
// 名称               :Eeprom_Write_OneByte()
// 创建日期           :2017-05-03
// 作者               :庄明群
// 功能               :向STM32L072xx内部EEPROM指定位置写入一字节数据
// 输入参数           :EepAddr（写入地址），WrData（写入数据）
// 输出参数           :无
// 返回结果           :OP_SUCCESS（写入成功），OP_FAILED（写入失败）
// 注意和说明         :写参数前请打开EEPROM写参数使能；地址不能越界
// 修改内容           :
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
// 名称               :Eeprom_Read_OneByte()
// 创建日期           :2017-05-03
// 作者               :庄明群
// 功能               :从STM32L072xx内部EEPROM指定位置读取一字节数据
// 输入参数           :EepAddr（读取地址）
// 输出参数           :无
// 返回结果           :返回字节
// 注意和说明         :地址不能越界
// 修改内容           :
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
// 名称               :Eeprom_EraseOneByte()
// 创建日期           :2017-05-03
// 作者               :庄明群
// 功能               :向STM32L072xx内部EEPROM指定位置擦除一字节
// 输入参数           :EepAddr（擦除地址）
// 输出参数           :无
// 返回结果           :OP_SUCCESS(擦除成功)，OP_FAILED(擦除失败)
// 注意和说明         :擦除参数前请打开EEPROM写参数使能；地址不能越界
// 修改内容           :
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
// 名称               :Eeprom_Write_MultiBytes()
// 创建日期           :2017-05-03
// 作者               :庄明群
// 功能               :向STM32L072xx内部EEPROM指定位置写多个字节
// 输入参数           :EepAddr(写起始地址)，pWrbuf(固定数据内存),Wrlen(写数据长度)
// 输出参数           :无
// 返回结果           :OP_SUCCESS(写入成功)，OP_FAILED(写入失败)
// 注意和说明         :写参数前请打开EEPROM写参数使能；地址不能越界
// 修改内容           :
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
// 名称               :Eeprom_Read_MultiBytes()
// 创建日期           :2017-05-03
// 作者               :庄明群
// 功能               :从STM32L072xx内部EEPROM指定位置读多个字节
// 输入参数           :EepAddr(读起始地址)，Rdlen(读数据长度)
// 输出参数           :pRdbuf(数据读内存)
// 返回结果           :无
// 注意和说明         :地址不能越界
// 修改内容           :
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
// 名称               :Eeprom_Erase_MultiBytes()
// 创建日期           :2017-05-03
// 作者               :庄明群
// 功能               :向STM32L072xx内部EEPROM指定位置擦除多字节
// 输入参数           :EepAddr(擦除起始地址)，Eslen(擦除长度)
// 输出参数           :无
// 返回结果           :OP_SUCCESS(擦除成功)，OP_FAILED(擦除失败)
// 注意和说明         :擦除参数前请打开EEPROM写参数使能；地址不能越界
// 修改内容           :
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
