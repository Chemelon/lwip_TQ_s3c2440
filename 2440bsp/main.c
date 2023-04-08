#include <stdio.h>

#include "FreeRTOS.h"
#include "portable.h"
#include "portmacro.h"
#include "projdefs.h"
#include "semphr.h"
#include "task.h"

#include "dm9000.h"
#include "dm9000x.h"
#include "exti.h"
#include "irq.h"
#include "s3c24xx.h"
#include "timer.h"
#include "usart.h"

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timers.h"

/*
 * LED1-4对应GPB5、GPB6、GPB7、GPB8
 */
#define GPB5_out (1 << (5 * 2))
#define GPB6_out (1 << (6 * 2))
#define GPB7_out (1 << (7 * 2))
#define GPB8_out (1 << (8 * 2))

/* 测试ARP包的发送 */
unsigned char arpsendbuf[42] = {

    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //以太网目标地址，全1表示为广播地址
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, //以太网源地址
    0x08, 0x06,                         //帧类型：ARP帧

    0x00, 0x01, //硬件类型：以太网
    0x08, 0x00, //协议类型：IP协议
    0x06,       //硬件地址长度：6字节
    0x04,       //协议地址长度：4字节
    0x00, 0x01, //操作码：ARP请求

    0x00, 0x01, 0x02, 0x0f, 0x0e, 0x0d, //发送端以太网硬件地址
    192,  168,  123,  200,              //发送端IP协议地址
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //接收端以太网硬件地址
    192,  168,  123,  3                 //接收端IP协议地址
};


void LwipTimerTask(void *p)
{

    for (;;)
    {
        printf("LwipTimerTask running\r\n");
        //HAL_Delay(500);
        //vTaskDelay(500);
        // sys_check_timeouts();
    }
}

void LwipInputTask(void *p)
{
    SemaphoreHandle_t *pxEXTISemaphore = pvPortMalloc(sizeof(SemaphoreHandle_t));
    /* 创建外部中断的信号量 */
    *pxEXTISemaphore = xSemaphoreCreateBinary();

    dm9k_netif.state = *pxEXTISemaphore;
    /* 从uboot中拷贝的初始化函数 */
    // lwip_init();
    // dm9k_netif_init();
    for (;;)
    {
        /* 取得信号量 */
        if (xSemaphoreTake(*pxEXTISemaphore, portMAX_DELAY) == pdTRUE)
        {
            printf("LwipInputTask running\r\n");
            /* 接收数据包 */
            // ethernetif_input(&dm9k_netif);
            /* 接触外部中断的屏蔽 */
            EXTI->EINTMASK &= ~(1 << 7);
        }
    }
}

void LEDTask(void *p)
{
    // LED1-LED4对应的4根引脚设为输出
    GPBCON = GPB5_out | GPB6_out | GPB7_out | GPB8_out;
    for (;;)
    {
        printf("LEDTask running\r\n");
        //vTaskDelay(500);
        //HAL_Delay(500);
        GPBDAT = (GPBDAT & (1 << 5)) ? (GPBDAT & ~(1 << 5)) : (GPBDAT | (1 << 5));
        GPBDAT = (GPBDAT & (1 << 6)) ? (GPBDAT & ~(1 << 6)) : (GPBDAT | (1 << 6));
        GPBDAT = (GPBDAT & (1 << 7)) ? (GPBDAT & ~(1 << 7)) : (GPBDAT | (1 << 7));
        GPBDAT = (GPBDAT & (1 << 8)) ? (GPBDAT & ~(1 << 8)) : (GPBDAT | (1 << 8));
    }
}

BaseType_t Init_Task(void *p)
{
    TaskHandle_t LwipTimerTask_Handle, LwipInputTask_Handle, LEDTask_Handle;
    BaseType_t result = pdFALSE;
    result = xTaskCreate(LwipTimerTask, "LwipTimerTask", 100, NULL, 1, &LwipTimerTask_Handle);
    if (result != pdTRUE)
    {
        printf("LwipTimerTask create error!\r\n");
    }

    // result = xTaskCreate(LwipInputTask, "LwipInputTask", 200, NULL, 3, &LwipInputTask_Handle);
    if (result != pdTRUE)
    {
        printf("LwipInputTask create error!\r\n");
    }

    result = xTaskCreate(LEDTask, "LEDTask", 100, NULL, 1, &LEDTask_Handle);
    if (result != pdTRUE)
    {
        printf("LwipInputTask create error!\r\n");
    }
    // vTaskDelete(NULL);
    return result;
}

extern volatile uint8_t exti_status;
int main(void)
{
    TaskHandle_t Init_TaskHandle;
    BaseType_t result;
    uart0_init();
    /* 打印编译时间 */
    printf("\r\nBuild date:%s %s\r\n", __TIME__, __DATE__);
    exti_init();
    //tim0_init();
    // irq_init();

    // result = xTaskCreate(Init_Task, "InitTask", 100, NULL, 4, &Init_TaskHandle);

    if (Init_Task(NULL) == pdTRUE)
    {
        printf("starting scheduler\r\n");
        vTaskStartScheduler();
    }

    while (1)
    {
        if (exti_status == 1)
        {
            exti_status = 0;
            // printf("got status\r\n");
            ethernetif_input(&dm9k_netif);
            /* 取消屏蔽外部中断 */
            INTMSK &= ~(1 << 4);
        }
    }
    return 0;
}
