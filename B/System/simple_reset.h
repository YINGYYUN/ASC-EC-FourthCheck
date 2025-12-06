#ifndef __SIMPLE_RESET_H
#define __SIMPLE_RESET_H

#include "stm32f10x.h"                  // Device header

void SimpleReset_Init(uint16_t delay_ms);
void SimpleReset_FeedDog(void);
void SimpleReset_Now(void);

#endif
