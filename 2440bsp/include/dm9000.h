#ifndef __DM9000_H
#define __DM9000_H


#define DM9K_CMD (*(volatile unsigned long *)0x10000000)
#define DM9K_DATA (*(volatile unsigned long *)0x10000004)

typedef struct dm9k
{
    void * init;
    void * read;
    void * write;
    void * ioctl;
}dm9k_type;




#endif