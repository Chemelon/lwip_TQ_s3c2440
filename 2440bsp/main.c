#include "dm9000.h"
#include "exti.h"
#include "irq.h"
// #include "lwip/err.h"
// #include "lwip/init.h"
// #include "lwip/ip_addr.h"
// #include "lwip/netif.h"
#include "s3c24xx.h"
#include "timer.h"
#include "usart.h"
#include <stdio.h>

#include "FreeRTOS.h"
#include "portmacro.h"
#include "projdefs.h"
#include "task.h"

#include "lwip/init.h"
#include "lwip/timers.h"

void task1(void *p)
{
    for (;;)
    {
        vTaskDelay(1000);
        printf("task1\r\n");
        GPBDAT = (GPBDAT & (1 << 5)) ? (GPBDAT & ~(1 << 5)) : (GPBDAT | (1 << 5));
    }
}

void task2(void *p)
{
    for (;;)
    {
        //vTaskDelay(500);
        printf("task2\r\n");
        GPBDAT = (GPBDAT & (1 << 6)) ? (GPBDAT & ~(1 << 6)) : (GPBDAT | (1 << 6));
        sys_check_timeouts();
    }
}

/*
 * LED1-4对应GPB5、GPB6、GPB7、GPB8
 */
#define GPB5_out (1 << (5 * 2))
#define GPB6_out (1 << (6 * 2))
#define GPB7_out (1 << (7 * 2))
#define GPB8_out (1 << (8 * 2))

int main(void)
{
    TaskHandle_t task1_handle, task2_handle;
    BaseType_t result = pdFALSE;
    extern void dm9k_netif_init(void);
    // LED1-LED4对应的4根引脚设为输出
    GPBCON = GPB5_out | GPB6_out | GPB7_out | GPB8_out;
    uart0_init();
    /* 打印编译时间 */
    printf("\r\nBuild date:%s %s\r\n",__TIME__,__DATE__);
    exti_init();
    tim0_init();
    irq_init();

    lwip_init();
    dm9k_netif_init();

    //result = xTaskCreate(task1, "task1", 100, NULL, 2, &task1_handle);
    if (result != pdTRUE)
    {
        printf("task1 create error!\r\n");
    }

    //result = xTaskCreate(task2, "task2", 100, NULL, 2, &task2_handle);
    if (result != pdTRUE)
    {
        printf("task2 create error!\r\n");
    }

    if (result == pdTRUE)
    {
        vTaskStartScheduler();
    }

    while (1)
    {
        HAL_Delay(100);
        sys_check_timeouts();
        //printf("running\r\n");
        GPBDAT = (GPBDAT & (1 << 6)) ? (GPBDAT & ~(1 << 6)) : (GPBDAT | (1 << 6));
    }
    return 0;
}
