#include "s3c24xx.h"

void nand_select(void)
{
    NFCONT &= ~(1 << 1);
}

void nand_deselect(void)
{
    NFCONT |= (1 << 1);
}

void nand_cmd(unsigned char cmd)
{
    volatile int i;
    NFCMMD = cmd;
    for (i = 0; i < 10; i++)
    {
        /* code */
    }
}

void nand_addr(unsigned int addr)
{
    unsigned int col = addr % 2048; 
    unsigned int page = addr / 2048;
    volatile int i;

    NFADDR = col & 0xff;
    for (i = 0; i < 10; i++)
        ;
    NFADDR = (col >> 8) & 0x0f;
    for (i = 0; i < 10; i++)
        ;
    NFADDR = page & 0xff;
    for (i = 0; i < 10; i++)
        ;
    NFADDR = (page >> 8) & 0xff;
    for (i = 0; i < 10; i++)
        ;
    NFADDR = (page >> 16) & 0x01;
    for (i = 0; i < 10; i++)
        ;
}

void nand_wait_ready(void)
{
    while (!(NFSTAT & 0x01))
    {
        /* code */
    }
}

unsigned char nand_data(void)
{
    return NFDATA;
}

void nand_init(void)
{
#define TACLS 1  //发出 CLE/ALE 之后过多长时间才能发出写信号 12ns
#define TWRPH0 3 // WR 持续时间 12ns
#define TWRPH1 1 //变为高电平后要维持的时间 5ns
    /* 设置时序 */
    NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);
    /* 使能NAND Flash控制器, 初始化ECC, 禁止片选 */
    NFCONT = (1 << 4) | (1 << 1) | (1 << 0);
}

#define NAND_SECTOR_SIZE 2048
#define NAND_BLOCK_MASK (NAND_SECTOR_SIZE - 1)
void nand_read(unsigned int addr, unsigned char *buf, unsigned int len)
{
    int col = addr % 2048;
    int i = 0;
    // if ((addr & NAND_BLOCK_MASK) || (len & NAND_BLOCK_MASK))
    // {
        // while(1){
        //     for(i=0;i<0x30000;i++);
        //     if(GPBDAT & 0x01)
        //         GPBDAT &= ~0x01;
        //     else 
        //         GPBDAT |= 0x01;
        // }
    //     /* 地址或长度不对齐 */
    // }

    /*选中*/
    nand_select();

    while (i < len)
    {
        /*发出读命令 00h*/
        nand_cmd(0x00);
        /*发出地址 5次*/
        nand_addr(addr);
        /*发出 30h*/
        nand_cmd(0x30);
        /*判断状态*/
        nand_wait_ready();
        /*读数据*/
        for (;(col <2048) && (i < len );col++)
        {
            buf[i] = nand_data();
            i++;
            addr++;
        }
        col = 0;
    }
    /*取消选中*/
    nand_deselect();
    return;
}
