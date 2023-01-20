#ifndef __DM9000_H
#define __DM9000_H

#define DM9K_CMD 0x10000000
#define DM9K_DATA 0x10000004

typedef struct dm9k
{
    void * init;
    void * read;
    void * write;


}dm9k_type;


#endif