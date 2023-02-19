
#define _SYS__STDINT_H

typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef struct 
{
    volatile uint32_t BWSCON;
    volatile uint32_t BANKCON0;
    volatile uint32_t BANKCON1;
    volatile uint32_t BANKCON2;
    volatile uint32_t BANKCON3;
    volatile uint32_t BANKCON4;
    volatile uint32_t BANKCON5;
    volatile uint32_t BANKCON6;
    volatile uint32_t BANKCON7;
    volatile uint32_t REFRESH;
    volatile uint32_t BANKSIZE;
    volatile uint32_t MRSRB6;
    volatile uint32_t MRSRB7;
} memory_controller_type;

typedef struct 
{
    volatile uint32_t EXTINT0;
    volatile uint32_t EXTINT1;
    volatile uint32_t EXTINT2;
    volatile uint32_t EINTFLT0;
    volatile uint32_t EINTFLT1;
    volatile uint32_t EINTFLT2;
    volatile uint32_t EINTFLT3;
    volatile uint32_t EINTMASK;
    volatile uint32_t EINTPEND;
} exti_controller_type;