#include "User_Selection.h"

#if IWDG_EN > 0
IWDG_HandleTypeDef  IWDG_HandleStructure;


//初始化看门狗
void User_Iwdg_Init(void)
{
  IWDG_HandleStructure.Init.Prescaler = IWDG_PRESCALER_32;
  IWDG_HandleStructure.Init.Reload = 0x0FA0;
  IWDG_HandleStructure.Init.Window = 0x0FA0;
  IWDG_HandleStructure.Instance = IWDG;
  IWDG_HandleStructure.Lock = HAL_UNLOCKED;
  IWDG_HandleStructure.State = HAL_IWDG_STATE_RESET;
  HAL_IWDG_Init(&IWDG_HandleStructure);
  HAL_IWDG_Start(&IWDG_HandleStructure);
}

void User_Iwdg_Feed(void)
{
  HAL_IWDG_Refresh(&IWDG_HandleStructure);
}

#endif