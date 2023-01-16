#include "s3c24xx.h"
#include "usart.h"
/*
 * LED1-4对应GPB5、GPB6、GPB7、GPB8
 */
#define GPB5_out (1 << (5 * 2))
#define GPB6_out (1 << (6 * 2))
#define GPB7_out (1 << (7 * 2))
#define GPB8_out (1 << (8 * 2))
int main(void)
{
    // printf("hello,world!");
    // LED1-LED4对应的4根引脚设为输出
    uart0_init();
    GPBCON = GPB5_out | GPB6_out | GPB7_out | GPB8_out;
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
        _write(0, "hello!", 7);
    }
    return 0;
}
