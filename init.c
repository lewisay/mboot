/**
 * init.c
 *
 * lewis
 */
#include "nand.h"

int is_boot_from_norflash(void)
{
	volatile int *p = (volatile int *) 0;
	int val = 0;

	val = *p;
	*p = 0x12345678;

	if (*p == 0x12345678) {
		*p = val;
		return 0;
	}

	return 1;
}

void copy_code_to_sdram(unsigned char *src, unsigned char *dest, unsigned int len)
{
	unsigned int i = 0;
	if (is_boot_from_norflash()) {
		while (i < len) {
			dest[i] = src[i];
			i++;
		}
	} else {
		nand_read((unsigned int) src, dest, len);
	}
}

void clear_bss(void)
{
	extern int __bss_start, __bss_end;
	int *p = &__bss_start;

	for (; p < &__bss_end; p++) {
		*p = 0;
	}
}
