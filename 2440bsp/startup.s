@******************************************************************************
@ File：head.s
@ 功能：设置SDRAM，将程序复制到SDRAM，然后跳到SDRAM继续执行
@******************************************************************************

.extern     main
.text 
.global _start

_start:
            @ ldr     sp, =4096               @设置堆栈
            @ bl      disable_watch_dog       @关WATCH DOG
            @ bl      clock_init              @初始化时钟
            @ bl      memsetup                @初始化SDRAM

            ldr     sp, =0x34000000         @设置栈
            ldr     lr, =halt_loop          @设置返回地址
            @ bl      __start
            ldr     pc, =main               @b指令和bl指令只能前后跳转32M的范围，所以这里使用向pc赋值的方法进行跳转
halt_loop:

            b       halt_loop
