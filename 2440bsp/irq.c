#include "irq.h"
#include "exti.h"
#include "s3c24xx.h"
#include "timer.h"
#include <stdio.h>
#include <strings.h>
#include <sys/_stdint.h>

void (*p_irqList[32])() = {exti0_handler, exti1_handler, exti2_handler, NULL, exti4_7_handler,
                           NULL,          NULL,          NULL,          NULL, NULL,
                           NULL,          NULL,          NULL,          NULL, tim4_handler,
                           NULL,          NULL,          NULL,          NULL, NULL,
                           NULL,          NULL,          NULL,          NULL, NULL,
                           NULL,          NULL,          NULL,          NULL, NULL,
                           NULL,          NULL};

void irq_init(void) { __asm__ __volatile__("msr cpsr_c, #0x5f"); }


void irq_handle(void)
{
    // uint32_t irqNumber = ffs(INTPND) - 1;
    uint32_t irqNumber = INTOFFSET;
    //printf("%d ", (int)irqNumber);

    /* 根据偏移量从数组中调用对应中断服务函数 */
    p_irqList[INTOFFSET]();

    /* 清除中断标志位 */
    SRCPND = 1 << irqNumber;
    INTPND = 1 << irqNumber;
}