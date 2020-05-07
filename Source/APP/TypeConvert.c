#include "TypeConvert.h"

float HexToFloat(uint8_t *pData)
{
  uint32_t Data;
  Data = pData[0];
  Data <<= 8;
  Data += pData[1];
  Data <<= 8;
  Data += pData[2];
  Data <<= 8;
  Data += pData[3];
  
  return *(float*)&Data;
}

uint32_t HexToUlong(uint8_t *pData)
{
  uint32_t Data;
  Data = pData[0];
  Data <<= 8;
  Data += pData[1];
  Data <<= 8;
  Data += pData[2];
  Data <<= 8;
  Data += pData[3];
  
  return Data;
}

void floatToHexArray(float fSource, uint8_t *pu8Temp, uint16_t num)
{
  uint16_t i;
  uint32_t u32Source = *(uint32_t *)&fSource;
  
  for(i = 0; i < num; i++)
  {
    pu8Temp[i] = (u32Source >> (8 * (3 - i))) & 0xFF;
  }
}



void SortArrayExtreme(uint32_t Array[], uint32_t ArraySize, uint32_t SortHeadSize, uint32_t SortTailSize)
{
    uint32_t i, j;
    uint32_t temp;

    for (i = 0; i < SortTailSize; i++)
    {
        for (j = 0; j < ArraySize - i - 1; j++)
        {
            if (Array[j] > Array[j+1])
            {
                temp = Array[j];
                Array[j] = Array[j+1];
                Array[j+1] = temp;
            }
        }
    }

    for (i = 0; i < SortHeadSize; i++)
    {
        for (j = ArraySize - SortTailSize - 1 ; j > i; j--)
        {
            if (Array[j - 1] > Array[j])
            {
                temp = Array[j - 1];
                Array[j - 1] = Array[j];
                Array[j] = temp;
            }
        }
    }
}


uint32_t GetAverage(uint32_t Array[], uint32_t ArraySize, uint32_t DelHeadSize, uint32_t DelTailSize)
{
    uint64_t sum = 0;

    if ((DelHeadSize + DelTailSize) >= ArraySize)
    {
        return 0;
    }

    for (long i = DelHeadSize; i < ArraySize - DelTailSize; i++)
    {
        sum += Array[i];
    }

    return(sum / (ArraySize - DelHeadSize - DelTailSize));
}


uint32_t GetDelExtremeAndAverage(uint32_t Array[], uint32_t ArraySize, uint32_t SortHeadSize, uint32_t SortTailSize)
{  
    SortArrayExtreme(Array, ArraySize, SortHeadSize, SortTailSize);
    return(GetAverage(Array, ArraySize, SortHeadSize, SortTailSize));
}


uint32_t GetFilterAverage(uint32_t* Array, uint32_t ArraySize)
{
    uint64_t sum = 0;
    for(long i = 0; i < ArraySize; i++)
    {
        sum += Array[i];
    }
    return(sum / ArraySize);    
}


