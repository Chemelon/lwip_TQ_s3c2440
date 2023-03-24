#ifndef __TIMER_H
#define __TIMER_H

#include "s3c24xx.h"

void tim4_init(void);
void tim4_handler(void);
void HAL_Delay(uint32_t Delay);

#endif