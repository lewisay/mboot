/**
 * uart.c
 *
 * lewis
 */

#include "uart.h"

/**
 *  uart 控制器
 */
#define ULCON0		(*(volatile unsigned long *) 0x50000000)
#define UCON0		(*(volatile unsigned long *) 0x50000004)
#define UFCON0		(*(volatile unsigned long *) 0x50000008)
#define UMCON0		(*(volatile unsigned long *) 0x5000000c)
#define UTRSTAT0	(*(volatile unsigned long *) 0x50000010)
#define UTXH0		(*(volatile unsigned char *) 0x50000020)
#define URXH0		(*(volatile unsigned char *) 0x50000024)
#define UBRDIV0		(*(volatile unsigned long *) 0x50000028)

#define PCLK            50000000
#define UART_CLK        PCLK
#define UART_BAUD_RATE  115200
#define UART_BRD        ((UART_CLK  / (UART_BAUD_RATE * 16)) - 1)

#define TXD0READY   (1<<2)
#define RXD0READY   (1)

/**
 * GPH2[5:4]
 * GPH3[7:6]
 */
#define GPHCON	(*(volatile unsigned long *) 0x56000070)

void uart_init(void)
{
	GPHCON &= ~((0x03 << 4) | (0x03 << 6));
	GPHCON |= (0x02 << 4) | (0x02 << 6);

	ULCON0 = 0x03;
	UCON0 = (0x01 << 0) | (0x01 << 2);
	UFCON0 = 0x00;
	UMCON0 = 0x00;
	UBRDIV0 = UART_BRD;
}

void putc(unsigned char c)
{
	while (!(UTRSTAT0 & TXD0READY));
	UTXH0 = c;
}

unsigned char getc(void)
{
	while (!(UTRSTAT0 & RXD0READY));
	return URXH0;
}

void puts(char *str)
{
	while (*str) {
		putc(*str);
		str++;
	}
}

void puthex(unsigned int val)
{
	int i, j;

	puts("0x");

	for (i = 0; i < 8; i++) {
		j = (val >> (7 - i) * 4) & 0xf;

		if ((j >= 0) && (j <= 9)) {
			putc('0' + j);
		} else {
			putc('A' + j - 0xA);
		}
	}
}