#ifndef __EXTI_H
#define __EXTI_H
#include "s3c24xx.h"

void exti_init(void);
void exti0_handler(void);
void exti1_handler(void);
void exti2_handler(void);
void exti4_7_handler(void);

#endif