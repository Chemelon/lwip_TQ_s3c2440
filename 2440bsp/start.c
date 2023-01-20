#include "s3c24xx.h"
#include "nand.h"

#if 0
#define S3C2440_MPLL_400MHZ ((0x5c << 12) | (0x01 << 4) | (0x01))
#define S3C2440_MPLL_200MHZ ((0x5c << 12) | (0x01 << 4) | (0x02))

/*上电后，WATCH DOG默认是开着的，要把它关掉 */
void disable_watch_dog(void) { WTCON = 0; }

/*
 * 对于MPLLCON寄存器，[19:12]为MDIV，[9:4]为PDIV，[1:0]为SDIV
 * 有如下计算公式：
 *  S3C2410: MPLL(FCLK) = (m * Fin)/(p * 2^s)
 *  S3C2410: MPLL(FCLK) = (2 * m * Fin)/(p * 2^s)
 *  其中: m = MDIV + 8, p = PDIV + 2, s = SDIV
 * 对于本开发板，Fin = 12MHz
 * 设置CLKDIVN，令分频比为：FCLK:HCLK:PCLK=1:2:4，
 * FCLK=200MHz,HCLK=100MHz,PCLK=50MHz
 */
void clock_init(void)
{
  CLKDIVN = 0x03; // FCLK:HCLK:PCLK=1:4:8

  /*
    如果HDIVN非0，CPU的总线模式应该从“fast bus mode”变为“asynchronous bus mode”
   */
  __asm__("mrc    p15, 0, r1, c1, c0, 0\n" /* 读出控制寄存器 */
          "orr    r1, r1, #0xc0000000\n"   /* 设置为“asynchronous bus mode” */
          "mcr    p15, 0, r1, c1, c0, 0\n" /* 写入控制寄存器 */
  );

  MPLLCON = S3C2440_MPLL_200MHZ; /* 现在，FCLK=200MHz,HCLK=100MHz,PCLK=50MHz */
}


/* 设置控制SDRAM的13个寄存器 */
void memsetup()
{
  int i = 0;
  unsigned long *p = (unsigned long *)MEM_CTL_BASE;

  /* SDRAM 13个寄存器的值 */
  unsigned long const mem_cfg_val[] = {
      0x22011110, // BWSCON
      0x00000700, // BANKCON0
      0x00000700, // BANKCON1
      0x00000700, // BANKCON2
      0x00000700, // BANKCON3
      0x00000700, // BANKCON4
      0x00000700, // BANKCON5
      0x00018005, // BANKCON6
      0x00018005, // BANKCON7
      // 0x008C07A3,     //REFRESH
      0x008C04F4, // REFRESH
      /*
      RFEN = 1
      Tsrc = 0x11 7 clocks
      Refersh Counter = 0x4f4 1279
       */
      0x000000B1, // BANKSIZE
      0x00000030, // MRSRB6
      0x00000030, // MRSRB7
  };

  for (; i < 13; i++)
    p[i] = mem_cfg_val[i];
}
#endif
void copy_code_to_sdram(unsigned int src, unsigned char *target, unsigned int len)
{
  nand_read(src, target, len);
}

void clean_bss(void)
{
  extern int __bss_start__, __bss_end__;
  int *p = &__bss_start__;

  for (; p < &__bss_end__; p++)
    *p = 0;
}
