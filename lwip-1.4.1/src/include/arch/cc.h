#ifndef __CC_H
#define __CC_H
#include "usart.h"
#include <stdio.h>

typedef unsigned char u8_t;
typedef signed char s8_t;
typedef unsigned short u16_t;
typedef signed short s16_t;

typedef unsigned int u32_t;
typedef signed int s32_t;
typedef unsigned int sys_prot_t;
typedef unsigned int mem_ptr_t;

#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT   __attribute__((__packed__))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#define LWIP_DEBUG

#define LWIP_PLATFORM_DIAG(x)                                                                                          \
    {                                                                                                                  \
        printf x;                                                                                                      \
        printf("\r");                                                                                                  \
    }

#define LWIP_PLATFORM_ASSERT(x)                                                                                        \
    {                                                                                                                  \
        printf(x);                                                                                                     \
        printf("\r");                                                                                                  \
        while (1)                                                                                                      \
            ;                                                                                                          \
    }

#define LWIP_ERROR(message, expression, handler)                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expression))                                                                                             \
        {                                                                                                              \
            printf(message);                                                                                           \
            handler;                                                                                                   \
        }                                                                                                              \
    } while (0)


#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

//#define SZT_F 8f

#define LWIP_PROVIDE_ERRNO

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

//* 临界代码保护宏
//#define	SYS_ARCH_DECL_PROTECT(x)	    //u32_t cpu_sr
//#define	SYS_ARCH_PROTECT(x)			    //cpu_sr =
// DisableInt()//OS_ENTER_CRITICAL() #define	SYS_ARCH_UNPROTECT(x)
// EnableInt(cpu_sr)     //OS_EXIT_CRITICAL()

#endif /* __CC_H */
