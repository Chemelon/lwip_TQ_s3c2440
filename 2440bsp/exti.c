#include "exti.h"
#include "s3c24xx.h"
#include <stdio.h>


void exti_init(void)
{
    /* 配置GPIO为外部中断 */
    GPFCON |= (0x2 << (0 * 2)) | (0x2 << (1 * 2)) | (0x2 << (2 * 2)) | (0x2 << (4 * 2));
    /* 下降沿触发 */
    EXTI->EXTINT0 |= 0x02 << 0 | 0x02 << 4 | 0x02 << 8 | 0x02 << 16;

    EXTI->EINTMASK &= ~(0x01 << 4);
    /* 暂时不改变优先级 */
    // PRIORITY =
    INTMSK &= ~(0x00000017);
}

void exti0_handler(void)
{
    EXTI->EINTPEND |= 0x01 << 0;
    printf("exti0\r\n");
}

void exti1_handler(void)
{
    EXTI->EINTPEND |= 0x01 << 1;
    printf("exti1\r\n");
}

void exti2_handler(void)
{
    EXTI->EINTPEND |= 0x01 << 2;
    printf("exti2\r\n");
}

void exti4_7_handler(void)
{
    if (EXTI->EINTPEND & (0x01 << 4))
    {
        EXTI->EINTPEND |= 0x01 << 4;
        printf("exti4\r\n");
    }
    else if (EXTI->EINTPEND & (0x01 << 7))
    {
        EXTI->EINTPEND |= 0x01 << 4;
        printf("exti7\r\n");
    }
}
