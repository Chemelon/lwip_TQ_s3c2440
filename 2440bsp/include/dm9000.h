#ifndef __DM9000_H
#define __DM9000_H

#include "s3c24xx.h"
#include <sys/_stdint.h>
// #define CONFIG_DM9000_BASE				0x20000000
// #define     DM9000_PKT_MAX		  1536
/* 之前使用u32去读写,没有相应,改为u8就可以了 */
#define DM9K_CMD  (*(volatile unsigned char *)CONFIG_DM9000_BASE)
#define DM9K_DATA (*(volatile unsigned char *)(CONFIG_DM9000_BASE + 4))

extern volatile uint8_t *NetRxPackets[4]; /* Receive packets			*/

// int dm9k_init(void);
// void dm9000_tx(uint8_t *data, uint32_t length);
// uint16_t dm9000_rx(uint8_t * data);
/* function declaration ------------------------------------- */
int eth_init(void);
void dm9k_io_init(void);
void dm9k_pbuff_init(void);
void eth_halt(void);
void dump_regs(void);
int eth_rx(void);
int eth_send(volatile void *, int);

/* 在ethernetif.c中 在此处声明方便调用 */
extern struct netif dm9k_netif;
void ethernetif_input(struct netif *netif);
void dm9k_netif_init(void);


#endif