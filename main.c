#include "dm9000.h"
#include "exti.h"
#include "irq.h"
#include "lwip/err.h"
#include "lwip/init.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "s3c24xx.h"
#include "usart.h"
#include <stdio.h>

/*
 * LED1-4对应GPB5、GPB6、GPB7、GPB8
 */
#define GPB5_out (1 << (5 * 2))
#define GPB6_out (1 << (6 * 2))
#define GPB7_out (1 << (7 * 2))
#define GPB8_out (1 << (8 * 2))
/* 在 ethernetif.c中 */
void dm9k_netif_init(void);

int main(void)
{
    uart0_init();
    irq_init();
    exti_init();
    // LED1-LED4对应的4根引脚设为输出
    GPBCON = GPB5_out | GPB6_out | GPB7_out | GPB8_out;

    dm9k_init();
    // lwip_init();
    // dm9k_netif_init();

    while (1)
    {
        for (int i = 0; i < 1000; i++)
        {
            for (int j = 0; j < 1000; j++)
            {
            }
        }
        GPBDAT = (GPBDAT & (1 << 5)) ? (GPBDAT & ~(1 << 5)) : (GPBDAT | (1 << 5));
        GPBDAT = (GPBDAT & (1 << 6)) ? (GPBDAT & ~(1 << 6)) : (GPBDAT | (1 << 6));
        //printf("hello\r\n");
    }
    return 0;
}
