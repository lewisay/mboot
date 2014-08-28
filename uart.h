/**
 * uart.h
 *
 * lewis
 */

#ifndef __S3C2440_UART_H__
#define __S3C2440_UART_H__

extern void uart_init(void);

extern void putc(unsigned char c);
extern void puts(char *str);
extern void puthex(unsigned int val);

extern unsigned char getc(void);

#endif
