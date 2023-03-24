#include "dm9000.h"
#include "dm9000reg.h"
#include "s3c24xx.h"
#include <stdio.h>

void dm9k_WR(uint16_t reg, uint16_t data)
{
    DM9K_CMD = reg;
    DM9K_DATA = data;
}

uint16_t dm9k_RD(uint16_t reg)
{
    DM9K_CMD = reg;
    return DM9K_DATA;
}

void dm9k_io_init(void)
{
    /* 内存控制器初始化 */
#define B4_Tacs 0x0 /*  0clk */
#define B4_Tcos 0x3 /*  4clk */
#define B4_Tacc 0x7 /* 14clk */
#define B4_Tcoh 0x1 /*  1clk */
#define B4_Tah  0x0 /*  0clk */
#define B4_Tacp 0x3 /*  6clk */
#define B4_PMC  0x0 /* normal */
    /* BANK4 */
    MEM_CTL->BWSCON &= ~(0x0f << 16);
    MEM_CTL->BWSCON |= (0x05 << 16);
    MEM_CTL->BANKCON4 = ((B4_Tacs << 13) | (B4_Tcos << 11) | (B4_Tacc << 8) | (B4_Tcoh << 6) | (B4_Tah << 4) |
                         (B4_Tacp << 2) | (B4_PMC << 0));

    /* 中断初始化 EINT7 */
    // 设置引脚复用为为中断
    GPFCON &= ~(0x3 << 14);
    GPFCON |= 0x2 << 14;

    // 设置中断触发方式
    EXTI->EXTINT0 &= ~(0x7 << 28);
    EXTI->EXTINT0 |= 0x1 << 28; /* 设置EINT7的信号触发方式，高电平 */

    // 中断清除
    EXTI->EINTPEND |= 1 << 7;    /* 向相应位置写1清除次级源挂起寄存器 */
    SRCPND |= BIT_EINT4_7; /* 向相应位置写1清除源挂起寄存器 */
    INTPND |= BIT_EINT4_7; /* 向相应位置写1清除挂起寄存器 */

    // 使能中断
    EXTI->EINTMASK &= ~(1 << 7);  /* 关闭外部中断屏蔽 */
    INTMSK &= ~BIT_EINT4_7; /* 关闭EINT4~7中断屏蔽，总中断  */
}

/**************************************************************************
 *
 *  Function  :  dm9000 芯片复位
 *
 *************************************************************************/
void dm9k_reset()
{
    // 设置GPIO控制寄存器  GPIO0设置为输出
    dm9k_WR(DM9000_GPCR, DM9000_GPCR_GPIO0_OUT);
    // 通过对GPIO0写入0为内部的PHY提供电源
    dm9k_WR(DM9000_GPR, 0);

    // 软件复位（自动清0），MAC内部回环模式
    dm9k_WR(DM9000_NCR, (DM9000_NCR_LBK_MAC_INTERNAL | DM9000_NCR_RST));
    // 对上一步的寄存器写入全0
    dm9k_WR(DM9000_NCR, 0);

    // 重复上面，用两次实现真正复位
    dm9k_WR(DM9000_NCR, (DM9000_NCR_LBK_MAC_INTERNAL | DM9000_NCR_RST));
    dm9k_WR(DM9000_NCR, 0);
}

/**************************************************************************
 *
 *  Function  :  dm9000 mac初始化
 *
 *************************************************************************/
void dm9k_mac_init()
{
    /* Program operating register, only internal phy supported */
    dm9k_WR(DM9000_NCR, 0x0);
    /* TX Polling clear */
    dm9k_WR(DM9000_TCR, 0);
    /* Less 3Kb, 200us */
    dm9k_WR(DM9000_BPTR, (0x03 << 4) | 0x07);
    /* Flow Control : High/Low Water */
    dm9k_WR(DM9000_FCTR, (0x03 << 4) | 0x08);
    /* SH FIXME: This looks strange! Flow Control */
    dm9k_WR(DM9000_FCR, 0x0);
    /* 特殊位 */
    dm9k_WR(DM9000_SMCR, 0);
    /* 清除发送状态 */
    dm9k_WR(DM9000_NSR, DM9000_NSR_WAKEST | DM9000_NSR_TX2END | DM9000_NSR_TX1END);
    /* 清除中断状态 */
    dm9k_WR(DM9000_ISR, DM9000_ISR_ROS | DM9000_ISR_ROOS | DM9000_ISR_PTS | DM9000_ISR_PRS);
}

/**************************************************************************
 *
 *  Function  :  dm9000 填充mac地址
 *
 *************************************************************************/
void dm9k_fill_macadd()
{
    uint8_t mac_addr[6] = {0x00, 0x01, 0x02, 0x0c, 0x0b, 0x0a};
    uint16_t i = 0;
    /* fill device MAC address registers */
    for (i = 0; i < 6; i++)
    {
        dm9k_WR(DM9000_PAB0 + i, mac_addr[i]);
    }
    /*maybe this is some problem*/
    for (i = 0; i < 8; i++)
    {
        dm9k_WR(DM9000_MAB0 + i, 0xff);
    }
    /* read back mac, just to be sure */
    printf("mac read back: ");
    for (i = 0; i < 6; i++)
    {
        printf("%02x ", dm9k_RD(DM9000_PAB0 + i));
    }
    printf("\r\n");
}

/**************************************************************************
 *
 *  Function  :  dm9000
 *芯片的捕获，实际上就是对dm9000上的生产厂家ID、产品ID信息进行对比
 *
 *************************************************************************/
int dm9k_probe()
{
    uint32_t id_val;
    // 读取生产厂家ID低字节
    id_val = dm9k_RD(DM9000_VID0);
    // 读取生产厂家ID高字节
    id_val |= (dm9k_RD(DM9000_VID1) << 8);

    // 读取产品ID低字节
    id_val |= (dm9k_RD(DM9000_PID0) << 16);
    // 读取产品ID高字节
    id_val |= (dm9k_RD(DM9000_PID1) << 24);

    if (id_val == DM9000_ID)
    {
        printf("dm9000 is found!\r\n");
        return 0;
    }
    else
    {
        printf("dm9000 is not found! id=%x\r\n", (unsigned int)id_val);
        return -1;
    }
}

void delay(uint32_t nms)
{
    for(int i =0 ;i<nms;i++)
    {
        for(int j=0 ;j<1000;j++)
        {}
    }
}

void dm_init(void)
{
    dm9k_WR(DM9000_NCR, 1); //软件复位DM9000
    delay(300);              //延时至少20μs
    dm9k_WR(DM9000_NCR, 0); //清除复位位

    dm9k_WR(DM9000_NCR, 1); //为了确保复位正确，再次复位
    delay(300);
    dm9k_WR(DM9000_NCR, 0);

    dm9k_WR(DM9000_GPCR, 1); //设置GPIO0为输出
    dm9k_WR(DM9000_GPR, 0);  //激活内部PHY

    dm9k_WR(DM9000_NSR, 0x2c); //清TX状态
    dm9k_WR(DM9000_ISR, 0xf);  //清中断状态

    dm9k_WR(DM9000_RCR, 0x39); //设置RX控制
    dm9k_WR(DM9000_TCR, 0);    //设置TX控制
    dm9k_WR(DM9000_BPTR, 0x3f);
    dm9k_WR(DM9000_FCTR, 0x3a);
    dm9k_WR(DM9000_FCR, 0xff);
    dm9k_WR(DM9000_SMCR, 0x00);

    dm9k_WR(DM9000_NSR, 0x2c); //再次清TX状态
    dm9k_WR(DM9000_ISR, 0xf);  //再次清中断状态
}

int dm9k_init(void)
{

    dm9k_io_init();

    /* 芯片重置 */
    dm_init();
    /* TODO:这个函数初始化顺序有点问题 待修改 */
    //dm9k_reset();

    /* 芯片捕获 */
    if (dm9k_probe() != 0)
        return 1;

    /* MAC初始化 */
    dm9k_mac_init();

    /*设置MAC*/
    dm9k_fill_macadd();

    /* 启动DM9000，这里如果加入RCR_ALL意为接受广播数据，会造成接收数据异常 */
    dm9k_WR(DM9000_RCR, DM9000_RCR_DIS_LONG | DM9000_RCR_DIS_CRC | DM9000_RCR_RXEN);

    /* Enable TX/RX interrupt mask */
    dm9k_WR(DM9000_IMR, DM9000_IMR_PAR);

    return 0;
}