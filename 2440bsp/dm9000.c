#include "dm9000.h"
#include "s3c24xx.h"

void dm9k_io_init(void) {
    
    MEM_CTL->BWSCON = 
    MEM_CTL->BANKCON2 = 0x01;
    
}

void dm9k_init(void) {}