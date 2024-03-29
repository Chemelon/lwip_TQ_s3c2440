
.equ        S3C2440_MPLL_100MHZ,     ((0x5c<<12)|(0x01<<4)|(0x03))
.equ        S3C2440_MPLL_200MHZ,     ((0x5c<<12)|(0x01<<4)|(0x02))
.equ        S3C2440_MPLL_400MHZ,     ((0x5c<<12)|(0x01<<4)|(0x01))
.equ        MEM_CTL_BASE,             0x48000000

.section .isr_vector,"a",%progbits
.global _start
_start:
    b reset
HandleUndef:
    b HandleUndef
@ 处理FreeRTOS软件中断请求
@ HandleSWI:
    b HandleSWI
HandlePrefetchAbort:
    b HandlePrefetchAbort
HandleDataAbort:
    b HandleDataAbort
HandleNotUsed:
    b HandleNotUsed

    b HandleIRQ

@ 调用FreeRTOS的中断函数 切换上下文
@    b HandleFIQ
@ HandleFIQ:
@ 直接跳转到FreeRTOS的函数中,不会执行下面的代码
@ 在irq执行以及swi执行过程中不要调度任务
@ 如果irq或者swi被打断,则spsr_fiq中保存的就是对应中断模式的spsr(spsr_irq\spsr_swi)
    stmdb sp!, {r0-r12,lr}
    @ 读取spsr
    mrs	r12, spsr
    and r12, r12, #0x0f
    ldr r11, =#0x0f
    @ 检测是否是系统模式
    cmp r12, r11
    bne PendSwitch
    @ 是系统模式则执行上下文切换
    @ ldmia sp!, {r0-r12,lr}
    ldr pc, = vTickISR

PendSwitch:
    @ mov r0, lr
    @ sub lr, pc, #0
    @ ldr pc, =puthex
    @ mrs	r0, spsr
    @ sub lr, pc, #0
    @ ldr pc, =puthex
    @ 保持tick准确
    sub lr, pc, #0
    ldr pc, =xTaskIncrementTick

    @ DEBUG : 清除SRCPND
    ldr r0, =#0x4A000000
    ldr r1, =#1<<14
    str r1, [r0]
    @ TODO : 在irq执行完后执行上下文切换
    ldmia sp!, {r0-r12,lr}
    subs pc, lr, #4



HandleIRQ:
    stmdb sp!, {r0-r12,lr}  @ 保存使用到的寄存器
                            @ 注意,此时sp是中断模式的sp
                            
    ldr lr, = int_return    @ 设置调用ISR即EINT_Handle函数后的返回地址
    ldr pc, = irq_handle    @ 调用中断服务函数,在irq.c中
    b .

HandleSWI:
    ldr pc, = vPortYieldProcessor
    b .

int_return:
    ldmia sp!, {r0-r12,lr}  @中断返回
    subs pc, lr, #4 @ 计算返回地址
    b .

reset:
/*关看门狗*/
    ldr r0, =#0x53000000 @WTCON
    mov r1, #0
    str r1, [r0]

/*设置时钟*/
    ldr r0, =#0x4c000014 @ CLKDIVN
    mov r1, #0x05
    @ mov r1, #0x05   @ FCLK:HCLK:PCLK = 1:4:8 ←----------
    str r1, [r0]

    /* 如果HDIVN非0，CPU的总线模式应该从“fast bus mode”变为“asynchronous bus mode” */
    mrc p15, 0, r0, c1, c0, 0        /* 读出控制寄存器 */ 
    orr r0, r0, #0xc0000000          /* 设置为“asynchronous bus mode” */
    /* dcache 需要mmu启动,所以裸机状态下就不要使用了 */
    mcr p15, 0, r0, c1, c0, 0        /* 写入控制寄存器 */

    /*MPLLCON = S3C2440_MPLL_400MHZ*/
    ldr r0, =#0x4c000004 @ MPLLCON
    ldr r1, =S3C2440_MPLL_400MHZ    @ ←----------
    str r1, [r0]

/*开启 icache*/
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, #1<<12
    mcr p15, 0, r0, c1, c0, 0

/*初始化SDRAM*/
    mov r1,     #MEM_CTL_BASE       @ 存储控制器的13个寄存器的开始地址
    adrl r2,    mem_cfg_val         @ 这13个值的起始存储地址
    add r3,     r1, #52             @ 13*4 = 52
1:  
    ldr r4,     [r2], #4            @ 读取设置值，并让r2加4
    str r4,     [r1], #4            @ 将此值写入寄存器，并让r1加4
    cmp r1,     r3                  @ 判断是否设置完所有13个寄存器
    bne 1b                          @ 若没有写成，继续

/************************************************/
    @ 对sdram读写测试sdram是否成功初始化
    ldr r0, = #0x56000010    @ GPBCON
    ldr r1, = #0x00015401    @ GPB0 GPB5 GPB6 OUT
    ldr r2, = #0x30000000
    str r1, [r2] @ 存到内存0x30000000里
    ldr r3, [r2]
    str r3, [r0]
    
    ldr r0, = #0x56000014    @ GPBDAT
    @ ldr r1, = 0x000001E1    @ LED1 LED2 OFF 响证明sdram没问题
    ldr r1, = #0x000001E0    @ LED1 LED2 OFF 响证明sdram没问题
    str r1, [r0]
/************************************************/
@ 设置中断函数栈指针
    msr cpsr_c, #0xd2       @ 进入中断模式
    ldr sp, =0x34000000
    msr cpsr_c, #0xd1       @ 进入快中断模式
    ldr sp, =0x33FF6000
    msr cpsr_c, #0xd3       @ 进入管理模式
    ldr sp, =0x33FEC000
    msr cpsr_c, #0xdf       @ 进入系统模式

/************************************************/
/*重定位 把bootloader 从FLASH 复制到它的链接地址去*/
    ldr sp, =0x33FE2000     @ 设置栈

    bl nand_init

    mov r0, #0
    ldr r1, =_start
    ldr r2, =__bss_start__
    sub r2, r2, r1

    bl copy_code_to_sdram
    bl clean_bss
/************************************************/
    ldr r0, = #0x56000014    @ GPBDAT
    ldr r1, = #0x00000000    @ LED1 LED2 on 不响证明拷贝完毕
    str r1, [r0]
/************************************************/

/*执行main*/
    ldr lr, =halt
    ldr pc, =main
halt:
    b halt

.align 4
mem_cfg_val:
    .long   0x22011110      @ BWSCON
    .long   0x00000700      @ BANKCON0
    .long   0x00000700      @ BANKCON1
    .long   0x00000700      @ BANKCON2
    .long   0x00000700      @ BANKCON3  
    .long   0x00000700      @ BANKCON4
    .long   0x00000700      @ BANKCON5
    .long   0x00018005      @ BANKCON6
    .long   0x00018005      @ BANKCON7
    .long   0x008C04F4      @ REFRESH
    .long   0x000000B1      @ BANKSIZE
    .long   0x00000030      @ MRSRB6
    .long   0x00000030      @ MRSRB7







