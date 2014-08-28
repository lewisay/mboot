/**
 * bootloader only for jz2440 by lewis
 *
 * 1、关闭看门狗
 * 2、设置时钟
 * 3、初始化SDRAM
 * 4、重定位
 * 5、执行 main
 */

/**
 * WTCON
 */
.text
.global _start
_start:
	ldr r0, =0x53000000
	mov r1, #0
	str r1, [r0]

#define s3c2440_MPLL_400MHZ ((0x5c << 12) | (0x01 << 4) | (0x01))
#define BWSCON_ADDR	0x48000000

/**
 * clk 200MHz 100MHz 50MHz
 */
	ldr r0, =0x4C000014
	mov r1, #0x05
	str r1, [r0]

	mrc p15, 0, r0, c1, c0, 0 
	orr r0, r0, #0xc0000000
	mcr p15, 0, r0, c1, c0, 0

	ldr r0, =0x4C000004
	ldr r1, =s3c2440_MPLL_400MHZ
	str r1, [r0]

	// enable icache
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #(1<<12)
	mcr	p15, 0, r0, c1, c0, 0

/**
 * init sdram
 */
	ldr r0, =BWSCON_ADDR
	adr r1, SDRAMDATA
	add r3, r0, #(13 * 4)
0:
	ldr r2, [r1], #4
	str r2, [r0], #4
	cmp r0, r3
	bne 0b

/**
 * copy code
 */

	ldr sp, =_start
	bl nand_init

	mov r0, #0
	ldr r1, =_start
	ldr r2, =__bss_start
	sub r2, r2, r1

	bl copy_code_to_sdram
	bl clear_bss

/**
 * main
 */
	ldr lr, =halt
	ldr pc, =main
halt:
	b halt

SDRAMDATA:
	.long 0x22011110	 // BWSCON
	.long 0x00000700	 // BANKCON0
	.long 0x00000700	 // BANKCON1
	.long 0x00000700	 // BANKCON2
	.long 0x00000700	 // BANKCON3  
	.long 0x00000700	 // BANKCON4
	.long 0x00000700	 // BANKCON5
	.long 0x00018005	 // BANKCON6
	.long 0x00018005	 // BANKCON7
	.long 0x008C04F4	 // REFRESH
	.long 0x000000B1	 // BANKSIZE
	.long 0x00000030	 // MRSRB6
	.long 0x00000030	 // MRSRB7