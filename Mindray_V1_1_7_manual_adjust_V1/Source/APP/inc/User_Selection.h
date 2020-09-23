#ifndef __USER_SELECTION_H
#define __USER_SELECTION_H

#include "stm32l0xx_hal_iwdg.h"

#define IWDG_EN              1

#if IWDG_EN > 0

void User_Iwdg_Init(void);
void User_Iwdg_Feed(void);

#endif

#endif