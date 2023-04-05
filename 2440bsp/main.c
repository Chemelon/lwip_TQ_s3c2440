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
        // vTaskDelay(500);
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

int main(void)
{
    TaskHandle_t task1_handle, task2_handle;
    //BaseType_t result = pdFALSE;
    // LED1-LED4对应的4根引脚设为输出
    GPBCON = GPB5_out | GPB6_out | GPB7_out | GPB8_out;
    uart0_init();
    /* 打印编译时间 */
    printf("\r\nBuild date:%s %s\r\n", __TIME__, __DATE__);
    exti_init();
    tim0_init();
    irq_init();

    /* 从uboot中拷贝的初始化函数 */
    //eth_init();
    lwip_init();
    dm9k_netif_init();

    while (1)
    {
        //HAL_Delay(100);
        //eth_send(arpsendbuf,42);
        //printf("running\r\n");
        ethernetif_input(&dm9k_netif);
        sys_check_timeouts();
        GPBDAT = (GPBDAT & (1 << 6)) ? (GPBDAT & ~(1 << 6)) : (GPBDAT | (1 << 6));
    }
    return 0;
}
