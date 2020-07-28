#ifndef __TYPECONVERT_H
#define __TYPECONVERT_H

#include "stdint.h"

float HexToFloat(uint8_t *pData);
uint32_t HexToUlong(uint8_t *pData);
void floatToHexArray(float fSource, uint8_t *pu8Temp, uint16_t num);
uint32_t GetDelExtremeAndAverage(uint32_t Array[], uint32_t ArraySize, uint32_t SortHeadSize, uint32_t SortTailSize);
uint32_t GetFilterAverage(uint32_t* Array, uint32_t ArraySize);
#endif
