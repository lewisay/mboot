/**
 * nand.h
 *
 * lewis
 */

#ifndef __S3C2440_NAND_H__
#define __S3C2440_NAND_H__

extern void nand_init(void);

extern void nand_select(void);
extern void nand_deselect(void);

extern void nand_cmd(unsigned char cmd);
extern void nand_addr(unsigned int addr);
extern void nand_wait_ready(void);
extern void nand_read(unsigned int addr, unsigned char *buf, unsigned int len);

#endif
