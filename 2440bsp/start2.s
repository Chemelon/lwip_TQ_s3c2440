
.equ        S3C2440_MPLL_100MHZ,     ((0x5c<<12)|(0x01<<4)|(0x03))
.equ        S3C2440_MPLL_200MHZ,     ((0x5c<<12)|(0x01<<4)|(0x02))
.equ        S3C2440_MPLL_400MHZ,     ((0x5c<<12)|(0x01<<4)|(0x01))
.equ        MEM_CTL_BASE,             0x48000000
.text
.global _start
_start:
/*执行main*/
    ldr lr, =halt
    ldr pc, =main

halt:
    b halt