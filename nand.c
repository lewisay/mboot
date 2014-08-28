/**
 * nand.c
 *
 * lewis
 */
#include "nand.h"

/**
 * nand flash 控制器
 */
#define NFCONF *((volatile unsigned long *) 0x4E000000)
#define NFCONT *((volatile unsigned long *) 0x4E000004)
#define NFCMMD *((volatile unsigned char *) 0x4E000008)
#define NFADDR *((volatile unsigned char *) 0x4E00000C)
#define NFDATA *((volatile unsigned char *) 0x4E000010)
#define NFSTAT *((volatile unsigned char *) 0x4E000020)

#define NAND_PAGE_SIZE (2 * 1024)

#define TACLS	0		// HCLK × TACLS	HCLK = 100Mhz = 10ns
#define TWRPH0	1		// HCLK × ( TWRPH0 + 1 )	10ns * (TWRPH0 + 1) >= 15ns TWRPH0 >= 0.5
#define TWRPH1	0		// HCLK × ( TWRPH1 + 1 )

static inline void delay(void)
{
	volatile int i = 0;
	for (; i < 10; i++);
}

void nand_init(void)
{
	NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);
	NFCONT = (1 << 4) | (1 << 1) || (1 << 0);	// 初始化ecc、禁止片选、使能nand flash
}

void nand_select(void)
{
	NFCONT &= ~(1 << 1);	
}

void nand_deselect(void)
{
	NFCONT |= (1 << 1);		
}

void nand_cmd(unsigned char cmd)
{
	NFCMMD = cmd;
	delay();
}

void nand_addr(unsigned int addr)
{
	unsigned int col = addr % NAND_PAGE_SIZE;
	unsigned int page = addr / NAND_PAGE_SIZE;

	NFADDR = col & 0xff;
	delay();

	NFADDR = (col >> 8) & 0x0f;
	delay();

	NFADDR = page & 0xff;
	delay();

	NFADDR = (page >> 8) & 0xff;
	delay();

	NFADDR = (page >> 16) & 0x03;
	delay();
}

void nand_wait_ready(void)
{
	while (!(NFSTAT & 0x01)) {
		delay();
	}
}

unsigned char nand_data(void)
{
	return NFDATA; 
}

void nand_read(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int col = addr % NAND_PAGE_SIZE;
	int i = 0;

	nand_select();

	while (i < len) {
		nand_cmd(0x00);
		nand_addr(addr);
		nand_cmd(0x30);

		nand_wait_ready();

		for (; col < NAND_PAGE_SIZE && i < len; i++, col++) {
			buf[i] = nand_data();
			addr++;
		}

		col = 0;
	}

	nand_deselect();
}