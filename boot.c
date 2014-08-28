/**
 * boot.c
 *
 * lewis
 */
#include "setup.h"
#include "uart.h"
#include "nand.h"

#define CONFIG_NR_DRAM_BANKS 	1
#define BOOT_PARAMS_ADDR 		0x30000100
#define BOOT_MEM_BASE		 	0x30000000
#define BOOT_MEM_SIZE		 	(64 * 1024 * 1024)

static struct tag *params;
static void setup_start_tag(void);
static void setup_memory_tags(void);
static void setup_commandline_tag(char *commandline);
static void setup_end_tag(void);

static int strlen(const char * s);
static char *strcpy(char *dest, const char *src);

/**
 * 启动内核
 *
 */
int main(void)
{
	void (*theKernel)(int zero, int arch, unsigned int params);

	uart_init();

	puts("copy kernel from nand <0x60040 to 0x30008000 size 2M>\r\n");
	nand_read((unsigned int) (0x60000 + 64), (unsigned char *) 0x30008000, 2 * 1024 * 1024);

	puts("setup boot pararms\r\n");
	params = (struct tag *) BOOT_PARAMS_ADDR;
	setup_start_tag();
	setup_memory_tags();
	setup_commandline_tag("noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0 user_debug=0xFF");
	setup_end_tag();

	puts("boot kernel\r\n");
	theKernel = (void (*)(int, int, unsigned int))0x30008000;
	theKernel(0, 362, 0x30000100);

	puts("Error\r\n");

	return -1;
}

static void setup_start_tag(void)
{
	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size(tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next(params);
}

static void setup_memory_tags(void)
{
	params->hdr.tag = ATAG_MEM;
	params->hdr.size = tag_size(tag_mem32);

	params->u.mem.start = BOOT_MEM_BASE;
	params->u.mem.size = BOOT_MEM_SIZE;

	params = tag_next(params);
}

static void setup_commandline_tag(char *commandline)
{
	char *p;

	if (!commandline) {
		return;
	}

	for (p = commandline; *p == ' '; p++);

	if (*p == '\0') {
		return;
	}

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size = (sizeof(struct tag_header) + strlen(p) + 1 + 3) >> 2;

	strcpy(params->u.cmdline.cmdline, p);

	params = tag_next(params);
}

static void setup_end_tag(void)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

static int strlen(const char * s)
{
	const char *sc;
	for (sc = s; *sc != '\0'; ++sc);
	return sc - s;
}

static char *strcpy(char *dest, const char *src)
{
	char *tmp = dest;
	while ((*dest++ = *src++) != '\0');
	return tmp;
}
