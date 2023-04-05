#include "timer.h"
#include "lwip/sys.h"
#include "s3c24xx.h"
#include <stdio.h>

volatile uint32_t uwTick;

void tim4_init(void)
{
    /* PCLK = 50Mhz */
    /* 50分频 计数周期1us devider 默认为2分频*/
    TIMER->TCFG0 |= ((50 / 2) - 1) << 8;

    TIMER->TCON |= 1 << 22;

    TIMER->TCNTB4 = 1000;
    /* mannul update */
    TIMER->TCON |= 1 << 21;

    TIMER->TCON &= ~(1 << 21);
    /* 开启中断 */
    INTMSK &= ~(1 << 14);
    /* 设置为FIQ */
    INTMOD |= 1 << 14;
    /* 开启定时器 */
    TIMER->TCON |= 1 << 20;
}

void tim0_init(void)
{
    /* 分频器 50分频 TIM0 TIM1 */
    TIMER->TCFG0 = ((50 / 2) - 1) << 0;
    /* auto reload */
    TIMER->TCON |= 1 << 3;

    TIMER->TCNTB0 = 1000;
    /* mannul update */
    TIMER->TCON |= 1 << 1;

    TIMER->TCON &= ~(1 << 1);

    /* 开启中断 */
    INTMSK &= ~(1 << 10);

    /* 开启定时器 */
    TIMER->TCON |= 1 << 0;
}

void tim4_handler(void)
{
    uwTick += 1;
    /* FIQ 不影响INTOFFSET */
    // SRCPND = 1 << 14;
    // printf("timer irq\r\n");
}

/* TODO: 系统调度的时候会屏蔽中断,那么计时将不准确*/
void tim0_handler(void)
{
    uwTick += 1;
    /* FIQ 不影响INTOFFSET */
    // SRCPND = 1 << 14;
    // printf("timer irq\r\n");
}


uint32_t HAL_GetTick(void)
{
    //printf("tick:%d", uwTick);
    return uwTick;
}
/**
 * @brief This function provides minimum delay (in milliseconds) based
 *        on variable incremented.
 * @note In the default implementation , SysTick timer is the source of time base.
 *       It is used to generate interrupts at regular time intervals where uwTick
 *       is incremented.
 * @param Delay specifies the delay time length, in milliseconds.
 * @retval None
 */
#define HAL_MAX_DELAY 0xFFFFFFFFU
void HAL_Delay(uint32_t Delay)
{
    uint32_t tickstart = HAL_GetTick();
    uint32_t wait = Delay;

    /* Add a freq to guarantee minimum wait */
    if (wait < HAL_MAX_DELAY)
    {
        wait += (uint32_t)(1);
    }

    while ((HAL_GetTick() - tickstart) < wait)
    {
    }
}

u32_t sys_now(void) { return HAL_GetTick(); }