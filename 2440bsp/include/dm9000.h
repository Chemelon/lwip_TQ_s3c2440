#ifndef __DM9000_H
#define __DM9000_H

#define CONFIG_DM9000_BASE				0x20000000
/* 之前使用u32去读写,没有相应,改为u8就可以了 */
#define DM9K_CMD (*(volatile unsigned char *)CONFIG_DM9000_BASE)
#define DM9K_DATA (*(volatile unsigned char *)(CONFIG_DM9000_BASE+4))

typedef struct dm9k
{
    void * init;
    void * read;
    void * write;
    void * ioctl;
}dm9k_type;

int dm9k_init(void);


#endif