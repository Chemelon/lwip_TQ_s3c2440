@******************************************************************************
@ File：head.s
@ 功能：设置SDRAM，将程序复制到SDRAM，然后跳到SDRAM继续执行
@******************************************************************************

.extern     main
.text 
.global _start

_start:
            ldr     sp, =4096               @设置堆栈
            bl      disable_watch_dog       @关WATCH DOG
            bl      clock_init              @初始化时钟

            @开启 icache
            mrc p15, 0, r0, c1, c0, 0
            orr r0, r0, #1<<12
            mcr p15, 0, r0, c1, c0, 0

            bl      memsetup                @初始化SDRAM

/************************************************/
    ldr r0, = 0x56000010    @ GPBCON
    ldr r1, = 0x00015401    @ GPB0 GPB5 GPB6 OUT
    ldr r2, = 0x30000000
    str r1, [r2] @ 存到内存0x30000000里
    ldr r3, [r2]
    str r3, [r0]
    
    ldr r0, = 0x56000014    @ GPBDAT
    ldr r1, = 0x000001E1    @ LED1 LED2 OFF 响证明sdram没问题
    str r1, [r0]

/************************************************/

            ldr     sp, =0x34000000         @设置栈

            bl      nand_init

            mov r0, #0
            ldr r1, =_start
            ldr r2, =__bss_start
            sub r2, r2, r1
            bl copy_code_to_sdram           @加载代码到内存
            bl clean_bss

/************************************************/
            ldr r0, = 0x56000014    @ GPBDAT
            ldr r1, = 0x00000000    
            str r1, [r0]
/************************************************/
            
            ldr     lr, =halt_loop          @设置返回地址
            ldr     pc, =main               @b指令和bl指令只能前后跳转32M的范围，所以这里使用向pc赋值的方法进行跳转

halt_loop:
            b       halt_loop
