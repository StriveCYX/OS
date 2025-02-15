BUILD_DIR = ./build
ENTRY_POINT = 0xc0001500
AS = nasm
CC = gcc
LD = ld
LIB = -I lib/ -I lib/kernel/ -I lib/user/ -I kernel/ -I device/ -I thread/ -I userprog/
ASFLAGS = -f elf
CFLAGS = -m32 -Wall $(LIB) -c -fno-builtin -W -Wstrict-prototypes \
		-Wmissing-prototypes
LDFLAGS = -m elf_i386 -Ttext $(ENTRY_POINT) -e main -Map $(BUILD_DIR)/kernel.map
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/init.o $(BUILD_DIR)/interrupt.o \
		$(BUILD_DIR)/timer.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/print.o \
		$(BUILD_DIR)/debug.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/bitmap.o \
		$(BUILD_DIR)/string.o $(BUILD_DIR)/thread.o $(BUILD_DIR)/switch.o \
		$(BUILD_DIR)/list.o $(BUILD_DIR)/console.o $(BUILD_DIR)/sync.o \
		$(BUILD_DIR)/keyboard.o $(BUILD_DIR)/ioqueue.o $(BUILD_DIR)/tss.o \
		$(BUILD_DIR)/process.o $(BUILD_DIR)/syscall-init.o $(BUILD_DIR)/syscall.o\
		$(BUILD_DIR)/stdio.o\
##############c 代码编译###############
$(BUILD_DIR)/main.o: kernel/main.c lib/kernel/print.h \
	lib/stdint.h kernel/init.h thread/thread.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/init.o: kernel/init.c kernel/init.h lib/kernel/print.h \
	lib/stdint.h kernel/interrupt.h device/timer.h kernel/memory.h \
	thread/thread.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/interrupt.o: kernel/interrupt.c kernel/interrupt.h \
	lib/stdint.h kernel/global.h lib/kernel/io.h lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/timer.o: device/timer.c device/timer.h lib/stdint.h\
	lib/kernel/io.h lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/debug.o: kernel/debug.c kernel/debug.h \
	lib/kernel/print.h lib/stdint.h kernel/interrupt.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/memory.o: kernel/memory.c kernel/memory.h lib/stdint.h\
	lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/bitmap.o: lib/kernel/bitmap.c lib/kernel/bitmap.h\
 	lib/stdint.h lib/kernel/print.h lib/string.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/string.o: lib/string.c kernel/global.h\
 	kernel/debug.h lib/string.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/list.o: thread/list.c thread/list.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/thread.o: thread/thread.c thread/thread.h 
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/console.o: device/console.c device/console.h 
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/sync.o: thread/sync.c thread/sync.h 
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/keyboard.o: device/keyboard.c device/keyboard.h 
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/ioqueue.o: device/ioqueue.c device/ioqueue.h 
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/tss.o: userprog/tss.c userprog/tss.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/process.o: userprog/process.c userprog/process.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/syscall-init.o: userprog/syscall-init.c userprog/syscall-init.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/syscall.o: lib/user/syscall.c lib/user/syscall.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/stdio.o: lib/stdio.c lib/stdio.h
	$(CC) $(CFLAGS) $< -o $@
##############汇编代码编译###############
$(BUILD_DIR)/kernel.o: kernel/kernel.S
	$(AS) $(ASFLAGS) $< -o $@
$(BUILD_DIR)/print.o: lib/kernel/print.S
	$(AS) $(ASFLAGS) $< -o $@
$(BUILD_DIR)/switch.o: thread/switch.S
	$(AS) $(ASFLAGS) $< -o $@
##############链接所有目标文件###############
$(BUILD_DIR)/kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

.PHONY : mk_dir clean all
mk_dir:
	if [[ ! -d $(BUILD_DIR) ]];then mkdir $(BUILD_DIR);fi

clean:
	cd $(BUILD_DIR) && rm -f ./*

build: $(BUILD_DIR)/kernel.bin

hd:
	dd if=build/kernel.bin of=/home/cent-os/Programs/bochs/hd60M.img bs=512 count=200 seek=9 conv=notrunc

all: mk_dir build hd

