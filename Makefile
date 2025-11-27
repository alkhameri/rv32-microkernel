CROSS ?= riscv64-unknown-elf-
CC := $(CROSS)gcc
AS := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy

INCLUDES := -Iinclude
CFLAGS := -march=rv32imac_zicsr -mabi=ilp32 -Wall -Wextra -O2 -ffreestanding -nostdlib -nostartfiles $(INCLUDES)
ASFLAGS := $(CFLAGS)
LDFLAGS := -T linker.ld -nostdlib -nostartfiles -ffreestanding

SRC_C := $(wildcard src/kernel/*.c)
SRC_S := $(wildcard src/boot/*.S)
OBJ := $(SRC_C:.c=.o) $(SRC_S:.S=.o)

.PHONY: all clean run

all: kernel.elf

kernel.elf: $(OBJ) linker.ld
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

run: kernel.elf
	qemu-system-riscv32 -machine virt -bios none -nographic -kernel $<

clean:
	rm -f $(OBJ) kernel.elf
