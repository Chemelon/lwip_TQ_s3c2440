#include "dm9000.h"
#include "dm9000reg.h"
#include "s3c24xx.h"

void dm9k_WR(uint16_t reg,uint16_t data)
{
    DM9K_CMD = reg;
    DM9K_DATA = data;
}

uint16_t dm9k_RD(uint16_t reg)
{
    DM9K_CMD = reg;
    return DM9K_DATA;
}

void dm9k_io_init(void) {
    /* 内存控制器初始化 */
#define     B4_Tacs         0x0 /*  0clk */
#define     B4_Tcos         0x0 /*  3clk */
#define     B4_Tacc         0x7 /* 14clk */
#define     B4_Tcoh         0x1 /*  1clk */
#define     B4_Tah          0x0 /*  0clk */
#define     B4_Tacp         0x0 /*  6clk */
#define     B4_PMC          0x0 /* normal */
    /* BANK2 */
    MEM_CTL->BWSCON &= ~(0x07<<8);
    MEM_CTL->BWSCON |= (0x01<<8);
    MEM_CTL->BANKCON2 = ((B4_Tacs<<13)|(B4_Tcos<<11)|(B4_Tacc<<8)|(B4_Tcoh<<6)|(B4_Tah<<4)|(B4_Tacp<<2)|(B4_PMC<<0));
    /* 中断初始化 EINT7 */
}

void dm9k_init(void) {


}