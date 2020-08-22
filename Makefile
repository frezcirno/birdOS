#########################
# Makefile for Orange'S #
#########################

# Entry point of Orange'S
# It must have the same value with 'KernelEntryPointPhyAddr' in load.inc!
ENTRYPOINT  = 0x100000

# Offset of entry point in kernel file
# It depends on ENTRYPOINT
ENTRYOFFSET =   0x400

# Programs, flags, etc.
ASM     = nasm
DASM    = objdump
CC      = gcc -std=c99
LD      = ld
ASMBFLAGS   = -I boot/include/
ASMKFLAGS   = -I include/ -I include/sys/ -f elf
CFLAGS      = -I include/ -I include/sys/ -c -fno-builtin -g -Wall -m32 -fno-stack-protector
LDFLAGS     = -Ttext $(ENTRYPOINT) -melf_i386
DASMFLAGS   = -D

# This Program
ORANGESBOOT = boot/boot0.bin boot/boot1.bin
ORANGESKERNEL = kernel.bin
OBJS        = kernel/kernel.o lib/syscall.o kernel/start.o kernel/main.o \
			kernel/clock.o kernel/keyboard.o kernel/tty.o kernel/console.o \
			kernel/i8259.o kernel/global.o kernel/protect.o kernel/proc.o \
			kernel/systask.o kernel/hd.o \
			lib/printf.o lib/sl.o lib/vsprintf.o lib/rand.o \
			lib/kliba.o lib/klib.o lib/ls.o lib/mkdir.o lib/misc.o \
			lib/open.o lib/read.o lib/write.o lib/close.o lib/unlink.o \
			lib/getpid.o lib/syslog.o lib/fork.o lib/glib.o lib/u_vga16.o lib/BGA.o \
			mm/main.o mm/func.o mm/memory.o \
			fs/main.o fs/open.o fs/misc.o fs/read_write.o fs/link.o fs/disklog.o \
			game/TTT.o game/2048.o game/flappy.o game/pushBox.o game/clock.o game/math.o
DASMOUTPUT  = kernel.bin.asm

# All Phony Targets
.PHONY : default clean disasm all build


# Default starting position
default : build


all : $(ORANGESBOOT) $(ORANGESKERNEL)


clean :
	-rm -f $(OBJS) $(ORANGESBOOT) $(ORANGESKERNEL)


disasm :
	$(DASM) $(DASMFLAGS) $(ORANGESKERNEL) > $(DASMOUTPUT)


build : $(ORANGESBOOT) $(ORANGESKERNEL)
	dd if=/dev/zero of=a.img count=2880
	dd if=boot/boot0.bin of=a.img count=1 conv=notrunc
	dd if=boot/boot1.bin of=a.img count=17 seek=1 conv=notrunc
	dd if=kernel.bin of=a.img count=1152 seek=36 conv=notrunc

.c.o:
	$(CC) $(CFLAGS) -o $@ $<

$(ORANGESKERNEL) : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

boot/boot0.bin : boot/boot0.asm boot/include/load.inc boot/include/lib16.inc boot/include/pm.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/boot1.bin : boot/boot1.asm boot/include/load.inc boot/include/lib16.inc boot/include/pm.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

kernel/kernel.o : kernel/kernel.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/kliba.o : lib/kliba.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/syscall.o : lib/syscall.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<


qemu: build
	qemu-system-i386                                 \
	  -m 512                                         \
	  -no-reboot                                     \
	  -fda a.img                                     \
	  -drive format=raw,file=80m.img                 \
	  -gdb tcp::1234                                 \
	  -boot d                                        \
	  -S                                             \
	  -vga std &
	gdb