CROSS	= arm-linux-
CC      = $(CROSS)gcc
LD      = $(CROSS)ld
AR      = $(CROSS)ar
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump

CFLAGS 		:= -Wall -O2
CPPFLAGS   	:= -nostdinc -nostdlib -fno-builtin

objs := start.o init.o boot.o nand.o uart.o

boot.bin: $(objs)
	$(LD) -Tboot.lds -o boot.elf $^
	$(OBJCOPY) -O binary -S boot.elf $@
	$(OBJDUMP) -D -m arm boot.elf > boot.dis

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.bin *.elf *.dis