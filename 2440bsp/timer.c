#include "timer.h"
#include "s3c24xx.h"
#include <stdio.h>

volatile uint32_t uwTick;

void tim4_init(void)
{
    /* PCLK = 50Mhz */
    /* 50分频 计数周期1us devider 默认为2分频*/
    TIMER->TCFG0 |= 24 << 8;

    TIMER->TCON |= 1 << 22;

    TIMER->TCNTB4 = 1000;
    /* mannul update */
    TIMER->TCON |= 1 << 21;

    TIMER->TCON &= ~(1 << 21);
    /* 开启中断 */
    INTMSK &= ~(1 << 14);
    /* 开启定时器 */
    TIMER->TCON |= 1 << 20;
}

void tim4_handler(void)
{
    uwTick += 1;
    //printf("timer irq\r\n");
}


uint32_t HAL_GetTick(void) { return uwTick; }
/**
 * @brief This function provides minimum delay (in milliseconds) based
 *        on variable incremented.
 * @note In the default implementation , SysTick timer is the source of time base.
 *       It is used to generate interrupts at regular time intervals where uwTick
 *       is incremented.
 * @param Delay specifies the delay time length, in milliseconds.
 * @retval None
 */
 #define HAL_MAX_DELAY      0xFFFFFFFFU
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