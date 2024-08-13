# Copyright 2020-2024, Hadrien Barral
# SPDX-License-Identifier: GPL-3.0-only

MAKEFLAGS += --no-builtin-rules --no-builtin-variables
.SUFFIXES:

BUILD := build
BIN := $(BUILD)/bos.iso
OPT := -O2
WARN := -Wall -Wextra -Wshadow -Wconversion
CFLAGS := -std=gnu2x -m32 -ffreestanding -MMD $(OPT) $(WARN)

C_TOOL := gcc

all: $(BUILD)/bos.iso

$(BUILD):
	mkdir -p $(BUILD)

-include $(BIN).d

$(BUILD)/print.o: src/stdio/print.c | $(BUILD)
	$(C_TOOL) -c -o "$@" $^ $(CFLAGS) -MMD ${EXTRA}

$(BUILD)/putchar.o: src/stdio/putchar.c | $(BUILD)
	$(C_TOOL) -c -o "$@" $^ $(CFLAGS) -MMD ${EXTRA}

$(BUILD)/tty.o: src/arch/tty.c | $(BUILD)
	$(C_TOOL) -c -o "$@" $^ $(CFLAGS) -MMD ${EXTRA}

$(BUILD)/multiboot.o: src/arch/multiboot.c | $(BUILD)
	$(C_TOOL) -c -o "$@" $^ $(CFLAGS) -MMD ${EXTRA}

$(BUILD)/faults.o: src/faults.c | $(BUILD)
	$(C_TOOL) -c -o "$@" $^ $(CFLAGS) -MMD ${EXTRA}

$(BUILD)/cache_driver.o: src/cache_driver.c | $(BUILD)
	$(C_TOOL) -c -o "$@" $^ $(CFLAGS) -MMD ${EXTRA}

$(BUILD)/boot.o: boot.s | $(BUILD)
	$(C_TOOL) -c -o "$@" $^ $(CFLAGS) -MMD ${EXTRA}

$(BUILD)/kernel.o: src/kernel.c | $(BUILD)
	$(C_TOOL) -c -o "$@" $^ $(CFLAGS) -MMD ${EXTRA}

$(BUILD)/bos.bin: $(BUILD)/boot.o $(BUILD)/print.o $(BUILD)/putchar.o $(BUILD)/tty.o $(BUILD)/faults.o $(BUILD)/cache_driver.o  $(BUILD)/kernel.o
	${C_TOOL} -m32 -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc -z max-page-size=0x1000 -z noexecstack -no-pie

$(BUILD)/bos.iso: $(BUILD)/bos.bin
	mkdir -p $(BUILD)/iso/boot/grub
	cp grub.cfg $(BUILD)/iso/boot/grub/grub.cfg
	cp $^ $(BUILD)/iso/boot/
	grub-mkrescue -o $@ $(BUILD)/iso

clean:
	rm -rf $(BUILD)
