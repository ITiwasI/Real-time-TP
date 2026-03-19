#ifndef SYSTICK_H
#define SYSTICK_H

#include "stm32f10x.h"


void SysTick_Init(void);
void SysTick_Int_Enable(void);
void SysTick_Handler(void);


#endif