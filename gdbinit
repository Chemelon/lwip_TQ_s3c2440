target remote 192.168.112.1:2331
mon reset halt
flushregs
load
flushregs
thb main
c
