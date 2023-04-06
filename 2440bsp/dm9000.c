#include "dm9000.h"
#include "dm9000x.h"
#include "s3c24xx.h"
#include "timer.h"
#include "usart.h"
#include <stdio.h>
#include <sys/_stdint.h>

#define CONFIG_DM9000_BASE 0x20000300
#define DM9000_IO          CONFIG_DM9000_BASE
#define DM9000_DATA        (CONFIG_DM9000_BASE + 4)
#define CONFIG_DM9000_USE_16BIT

/* #define CONFIG_DM9000_DEBUG */
#define CONFIG_DM9000_DEBUG

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;


#ifdef CONFIG_DM9000_DEBUG
#define DM9000_DBG(fmt, args...) printf(fmt, ##args)
#else /*  */
#define DM9000_DBG(fmt, args...)
#endif /*  */

static uint16_t phy_read(int);
static void phy_write(int, uint16_t);
static uint8_t DM9000_ior(int);
static int dm9000_probe(void);
static void DM9000_iow(int reg, uint8_t value);
// static u16 read_srom_word(int);	//HJ_del 20100528

/* DM9000 network board routine ---------------------------- */

#define DM9000_outb(d, r) (*(volatile u8 *)r = d)
#define DM9000_outw(d, r) (*(volatile u16 *)r = d)
#define DM9000_outl(d, r) (*(volatile u32 *)r = d)
#define DM9000_inb(r)     (*(volatile u8 *)r)
#define DM9000_inw(r)     (*(volatile u16 *)r)
#define DM9000_inl(r)     (*(volatile u32 *)r)

#ifdef CONFIG_DM9000_DEBUG
void dump_regs(void)
{
    DM9000_DBG("\r\n");
    DM9000_DBG("NCR   (0x00): %02x\r\n", DM9000_ior(0));
    DM9000_DBG("NSR   (0x01): %02x\r\n", DM9000_ior(1));
    DM9000_DBG("TCR   (0x02): %02x\r\n", DM9000_ior(2));
    DM9000_DBG("TSRI  (0x03): %02x\r\n", DM9000_ior(3));
    DM9000_DBG("TSRII (0x04): %02x\r\n", DM9000_ior(4));
    DM9000_DBG("RCR   (0x05): %02x\r\n", DM9000_ior(5));
    DM9000_DBG("RSR   (0x06): %02x\r\n", DM9000_ior(6));
    DM9000_DBG("ISR   (0xFE): %02x\r\n", DM9000_ior(DM9000_ISR));
    DM9000_DBG("\r\n");
}
#endif /*  */

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
    // EXTI->EXTINT0 |= 0x0 << 28; /* 设置EINT7的信号触发方式，低电平 */
    EXTI->EXTINT0 |= 0x1 << 28; /* 设置EINT7的信号触发方式，高电平 */

    // 中断清除
    EXTI->EINTPEND = 1 << 7; /* 向相应位置写1清除次级源挂起寄存器 */
    SRCPND |= BIT_EINT4_7;   /* 向相应位置写1清除源挂起寄存器 */
    INTPND |= BIT_EINT4_7;   /* 向相应位置写1清除挂起寄存器 */

    // 使能中断
    EXTI->EINTMASK &= ~(1 << 7); /* 关闭外部中断屏蔽 */
    INTMSK &= ~BIT_EINT4_7;      /* 关闭EINT4~7中断屏蔽，总中断  */
}

/*
  Search DM9000 board, allocate space and register it
*/
int dm9000_probe(void)
{
    u32 id_val;

    id_val = DM9000_ior(DM9000_VIDL);
    id_val |= DM9000_ior(DM9000_VIDH) << 8;
    id_val |= DM9000_ior(DM9000_PIDL) << 16;
    id_val |= DM9000_ior(DM9000_PIDH) << 24;
    if (id_val == DM9000_ID)
    {
        printf("dm9000 i/o: 0x%x, id: 0x%x \r\n", CONFIG_DM9000_BASE, id_val);
        return 0;
    }
    else
    {
        printf("dm9000 not found at 0x%08x id: 0x%08x\r\n", CONFIG_DM9000_BASE, id_val);
        return -1;
    }
}

/* General Purpose dm9000 reset routine */
static void dm9000_reset(void)
{
    DM9000_DBG("resetting\r\n");
    DM9000_iow(DM9000_NCR, NCR_RST);
    HAL_Delay(3); /* delay 3ms */
    DM9000_iow(DM9000_NCR, 0x00);

    DM9000_iow(DM9000_NCR, NCR_RST);
    HAL_Delay(3); /* delay 3ms */
    DM9000_iow(DM9000_NCR, 0x00);
}

/* Initilize dm9000 board
 */
uint8_t bi_enetaddr[6] = {0x00, 0x01, 0x02, 0x0f, 0x0e, 0x0d};
int eth_init(void)
{
    int i, oft;
    DM9000_DBG("eth_init()\r\n");

    dm9k_io_init();

    DM9000_iow(DM9000_GPCR, 0x01);
    DM9000_iow(DM9000_GPR, 0x00);

    /* RESET device */
    dm9000_reset();

    i = 100;
    do
    {
        HAL_Delay(3); /* delay 3ms */
    } while (i-- && (0x46 != DM9000_ior(DM9000_VIDL)));

    if (i == 0x0)
        return -1;

    if (dm9000_probe() < 0)
        return -1;

    /* Auto-negotiation */
    phy_write(0x04, 0x00a1);
    phy_write(0x00, 0x1200);

    dm9000_reset();

    /* Program operating register */
    DM9000_iow(DM9000_NCR, 0x0);                                  /* only intern phy supported by now */
    DM9000_iow(DM9000_IMR, IMR_PAR);                              /* Enable TX/RX interrupt mask */
    DM9000_iow(DM9000_TCR, 0);                                    /* TX Polling clear */
    DM9000_iow(DM9000_BPTR, 0x3f);                                /* Less 3Kb, 200us */
    DM9000_iow(DM9000_FCTR, FCTR_HWOT(3) | FCTR_LWOT(8));         /* Flow Control : High/Low Water */
    DM9000_iow(DM9000_FCR, 0x28);                                 /* SH FIXME: This looks strange! Flow Control */
    DM9000_iow(DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END); /* clear TX status */
    DM9000_iow(DM9000_ISR, 0x0f);                                 /* Clear interrupt status */

    DM9000_iow(0x38, 0x6b);

    DM9000_iow(DM9000_SMCR, 0); /* Special Mode */

    printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n", bi_enetaddr[0], bi_enetaddr[1], bi_enetaddr[2], bi_enetaddr[3],
           bi_enetaddr[4], bi_enetaddr[5]);
    for (i = 0, oft = 0x10; i < 6; i++, oft++)
        DM9000_iow(oft, bi_enetaddr[i]);
    for (i = 0, oft = 0x16; i < 8; i++, oft++)
        DM9000_iow(oft, (7 == i) ? 0x80 : 0x00);

    /* read back mac, just to be sure */
    for (i = 0, oft = 0x10; i < 6; i++, oft++)
        DM9000_DBG("%02x:", DM9000_ior(oft));
    DM9000_DBG("\r\n");

    for (uint32_t tmo = HAL_GetTick() + 3000;;)
    {
        if (tmo > HAL_GetTick())
        {
            /* 检查link ok */
            if ((DM9000_ior(DM9000_NSR) & (0x01 << 6)) == 0x01)
            {
                break;
            }
        }
        else
        {
#ifdef CONFIG_DM9000_DEBUG
            dump_regs();
#endif
            printf("check linkup timeout\r\n");
            break;
        }
    }

    DM9000_iow(DM9000_NSR, 0x2c); //清除各种状态标志位
    DM9000_iow(DM9000_ISR, 0x0f); //清除所有中断标志位

    /* Activate DM9000 */
    DM9000_iow(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN); /* RX enable */
    /* 开启接收中断 */
    DM9000_iow(DM9000_IMR, IMR_PAR | 0x01);

    dm9k_pbuff_init();

    return 0;
}

/*
  Hardware start transmission.
  Send a packet to media from the upper layer.
*/
int eth_send(volatile void *packet, int length)
{
    char *data_ptr;
    u32 tmplen, i;
    int tmo;
    /* 关闭dm9k中断请求 */
    DM9000_iow(DM9000_IMR, IMR_PAR);

    DM9000_DBG("eth_send: length: %d\r\n", length);

    /* Move data to DM9000 TX RAM */
    data_ptr = (char *)packet;
    DM9000_outb(DM9000_MWCMD, DM9000_IO);

#ifdef CONFIG_DM9000_USE_8BIT
    /* Byte mode */
    for (i = 0; i < length; i++)
        DM9000_outb((data_ptr[i] & 0xff), DM9000_DATA);

#endif /*  */
#ifdef CONFIG_DM9000_USE_16BIT
    tmplen = (length + 1) / 2;
    for (i = 0; i < tmplen; i++)
        DM9000_outw(((u16 *)data_ptr)[i], DM9000_DATA);

#endif /*  */
#ifdef CONFIG_DM9000_USE_32BIT
    tmplen = (length + 3) / 4;
    for (i = 0; i < tmplen; i++)
        DM9000_outl(((u32 *)data_ptr)[i], DM9000_DATA);

#endif /*  */

    /* Set TX length to DM9000 */
    DM9000_iow(DM9000_TXPLL, length & 0xff);
    DM9000_iow(DM9000_TXPLH, (length >> 8) & 0xff);

    /* Issue TX polling command */
    DM9000_iow(DM9000_TCR, TCR_TXREQ); /* Cleared after TX complete */

    /* wait for end of transmission */
    tmo = HAL_GetTick() + 500;
    while (DM9000_ior(DM9000_TCR) & TCR_TXREQ)
    {
        if (HAL_GetTick() >= tmo)
        {
#ifdef CONFIG_DM9000_DEBUG
            dump_regs();
#endif
            printf("transmission timeout\r\n");
            break;
        }
    }
    DM9000_DBG("transmit done\r\n");

    /* 开启dm9k接收中断 */
    DM9000_iow(DM9000_IMR, IMR_PAR | 0x01);

    return 0;
}

/*
  Stop the interface.
  The interface is stopped when it is brought.
*/
void eth_halt(void)
{
    DM9000_DBG("eth_halt\n");

    /* RESET devie */
    phy_write(0, 0x8000);         /* PHY RESET */
    DM9000_iow(DM9000_GPR, 0x01); /* Power-Down PHY */
    HAL_Delay(1);                 /* delay 1ms */
    DM9000_iow(DM9000_IMR, 0x80); /* Disable all interrupt */
    DM9000_iow(DM9000_RCR, 0x00); /* Disable RX */
}

/*
  Received a packet and pass to upper layer
*/
#define PKTSIZE       1518
#define PKTSIZE_ALIGN 1536
#define PKTALIGN      32
#define PKTBUFSRX     4
volatile uint8_t PktBuf[(PKTBUFSRX + 1) * PKTSIZE_ALIGN + PKTALIGN];
volatile uint8_t *NetTxPacket = 0;         /* THE transmit packet		*/
volatile uint8_t *NetRxPackets[PKTBUFSRX]; /* Receive packets			*/

void dm9k_pbuff_init(void)
{
    /* 初始化内存池 */
    NetTxPacket = NULL;
    if (!NetTxPacket)
    {
        int i;
        /*
         *	Setup packet buffers, aligned correctly.
         */
        NetTxPacket = &PktBuf[0] + (PKTALIGN - 1);
        NetTxPacket -= (ulong)NetTxPacket % PKTALIGN;
        for (i = 0; i < PKTBUFSRX; i++)
        {
            NetRxPackets[i] = NetTxPacket + (i + 1) * PKTSIZE_ALIGN;
        }
    }
    printf("dm9k memp inited\r\n");
}

int eth_rx(void)
{
    u8 rxbyte, *rdptr = (u8 *)NetRxPackets[0];
    u16 RxStatus, RxLen = 0;
    u32 tmplen, i;
    u16 save_mrr, check_mrr, calc_mrr;
#ifdef CONFIG_DM9000_USE_32BIT
    u32 tmpdata;
#endif

    /* Check packet ready or not */
    DM9000_ior(DM9000_MRCMDX); /* Dummy read */
    save_mrr = (DM9000_ior(0xf5) << 8) | DM9000_ior(0xf4);
    DM9000_DBG("read inertnal add=%04x\r\n", save_mrr);

    rxbyte = DM9000_ior(DM9000_MRCMDX); /* Got most updated data */

    if (rxbyte != 1)
    {
        /* Status check: this byte must be 0 or 1 */
        if (rxbyte > 1)
        {
            DM9000_iow(DM9000_RCR, 0x00); /* Stop Device */
            DM9000_iow(DM9000_ISR, 0x80); /* Stop INT request */
            DM9000_DBG("rx status check: %d\r\n", rxbyte);
        }
        return 0;
    }

    DM9000_DBG("receiving packet\r\n");

    /* A packet ready now  & Get status/length */
    DM9000_outb(DM9000_MRCMD, DM9000_IO);

#ifdef CONFIG_DM9000_USE_8BIT
    RxStatus = DM9000_inb(DM9000_DATA) + (DM9000_inb(DM9000_DATA) << 8);
    RxLen = DM9000_inb(DM9000_DATA) + (DM9000_inb(DM9000_DATA) << 8);

#endif /*  */
#ifdef CONFIG_DM9000_USE_16BIT
    RxStatus = DM9000_inw(DM9000_DATA);
    RxLen = DM9000_inw(DM9000_DATA);

#endif /*  */
#ifdef CONFIG_DM9000_USE_32BIT
    tmpdata = DM9000_inl(DM9000_DATA);
    RxStatus = tmpdata;
    RxLen = tmpdata >> 16;

#endif /*  */
    DM9000_DBG("rx status: 0x%04x rx len: %d\r\n", RxStatus, RxLen);

    /* Move data from DM9000 */
    /* Read received packet from RX SRAM */
#ifdef CONFIG_DM9000_USE_8BIT
    for (i = 0; i < RxLen; i++)
        rdptr[i] = DM9000_inb(DM9000_DATA);

#endif /*  */
#ifdef CONFIG_DM9000_USE_16BIT
    tmplen = (RxLen + 1) / 2;
    for (i = 0; i < tmplen; i++)
        ((u16 *)rdptr)[i] = DM9000_inw(DM9000_DATA);

#endif /*  */
#ifdef CONFIG_DM9000_USE_32BIT
    tmplen = (RxLen + 3) / 4;
    for (i = 0; i < tmplen; i++)
        ((u32 *)rdptr)[i] = DM9000_inl(DM9000_DATA);

#endif /*  */
    if ((RxStatus & 0xbf00) || (RxLen < 0x40) || (RxLen > DM9000_PKT_MAX))
    {
        if (RxStatus & 0x100)
        {
            printf("rx fifo error\r\n");
        }
        if (RxStatus & 0x200)
        {
            printf("rx crc error\r\n");
        }
        if (RxStatus & 0x8000)
        {
            printf("rx length error\r\n");
        }
        if (RxLen > DM9000_PKT_MAX)
        {
            printf("rx length too big\r\n");
            dm9000_reset();
        }
        /* 清除中断请求 */
        DM9000_iow(DM9000_ISR, 0x0f); /* clear INT request */
    }
    else
    {

        /* Pass to upper layer */
        DM9000_DBG("passing packet to upper layer\r\n");
        /* 清除中断请求 */
        DM9000_iow(DM9000_ISR, 0x0f); /* clear INT request */

        return RxLen;
    }
    return 0;
}

/*
  Read a word data from SROM
*/
#if 0  // HJ_del start 20100528
static u16
read_srom_word(int offset)
{
	DM9000_iow(DM9000_EPAR, offset);
	DM9000_iow(DM9000_EPCR, 0x4);
	udelay(200);
	DM9000_iow(DM9000_EPCR, 0x0);
	return (DM9000_ior(DM9000_EPDRL) + (DM9000_ior(DM9000_EPDRH) << 8));
}
#endif // HJ_del end 20100528

/*
   Read a byte from I/O port
*/
static u8 DM9000_ior(int reg)
{
    DM9000_outb(reg, DM9000_IO);
    return DM9000_inb(DM9000_DATA);
}

/*
   Write a byte to I/O port
*/
static void DM9000_iow(int reg, u8 value)
{
    DM9000_outb(reg, DM9000_IO);
    DM9000_outb(value, DM9000_DATA);
}

/*
   Read a word from phyxcer
*/
static u16 phy_read(int reg)
{
    u16 val;

    /* Fill the phyxcer register into REG_0C */
    DM9000_iow(DM9000_EPAR, DM9000_PHY | reg);
    DM9000_iow(DM9000_EPCR, 0xc); /* Issue phyxcer read command */
    do
    {
        HAL_Delay(1); /* Wait read complete */
    } while (0x0c != DM9000_ior(DM9000_EPCR));
    DM9000_iow(DM9000_EPCR, 0x0); /* Clear phyxcer read command */
    val = (DM9000_ior(DM9000_EPDRH) << 8) | DM9000_ior(DM9000_EPDRL);

    /* The read data keeps on REG_0D & REG_0E */
    DM9000_DBG("phy_read(%d): %d\r\n", reg, val);
    return val;
}

/*
   Write a word to phyxcer
*/
static void phy_write(int reg, u16 value)
{

    /* Fill the phyxcer register into REG_0C */
    DM9000_iow(DM9000_EPAR, DM9000_PHY | (reg & 0x3f));

    /* Fill the written data into REG_0D & REG_0E */
    DM9000_iow(DM9000_EPDRL, (value & 0xff));
    DM9000_iow(DM9000_EPDRH, ((value >> 8) & 0xff));
    DM9000_iow(DM9000_EPCR, 0xa); /* Issue phyxcer write command */
    do
    {
        HAL_Delay(1); /* Wait write complete */
    } while (0x0a != DM9000_ior(DM9000_EPCR));
    DM9000_iow(DM9000_EPCR, 0x0); /* Clear phyxcer write command */
    DM9000_DBG("phy_write(reg:%d, value:%04x)\r\n", reg, value);
}
