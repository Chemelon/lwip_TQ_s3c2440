#include "stdint.h"
// typedef unsigned int uint32_t;
// typedef unsigned short uint16_t;
// typedef unsigned char uint8_t;

// typedef uint32_t u32;
// typedef uint16_t u16;
// typedef uint8_t u8 ;

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

typedef struct
{
    volatile uint32_t TCFG0;
    volatile uint32_t TCFG1;
    volatile uint32_t TCON;
    volatile uint32_t TCNTB0;
    volatile uint32_t TCMPB0;
    volatile uint32_t TCNTO0;
    volatile uint32_t TCNTB1;
    volatile uint32_t TCMPB1;
    volatile uint32_t TCNTO1;
    volatile uint32_t TCNTB2;
    volatile uint32_t TCMPB2;
    volatile uint32_t TCNTO2;
    volatile uint32_t TCNTB3;
    volatile uint32_t TCMPB3;
    volatile uint32_t TCNTO3;
    volatile uint32_t TCNTB4;
    volatile uint32_t TCNTO4;
} timer_controller_type;