# Directories
SRC_DIR := src
BUILD_DIR := build
ISO_DIR := iso/boot
GRUB_CFG := grub.cfg

# Tools
CC := i686-elf-gcc
LD := i686-elf-ld
CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(SRC_DIR)
LDFLAGS := -T linker.ld -nostdlib

# Kernel ELF and ISO name
KERNEL := $(BUILD_DIR)/kernel.elf
ISO := arulos.iso

# Find all .c files recursively
SOURCES := $(shell find $(SRC_DIR) -name '*.c')
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# Default build target
all: $(ISO)

# Compile each .c into .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link all objects into kernel ELF
$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# Build ISO image with GRUB
$(ISO): $(KERNEL)
	mkdir -p $(ISO_DIR)/grub
	cp $(KERNEL) $(ISO_DIR)/kernel.elf
	cp $(GRUB_CFG) $(ISO_DIR)/grub/grub.cfg
	grub-mkrescue -o $@ iso

# Run with QEMU
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

# Clean build output
clean:
	rm -rf $(BUILD_DIR) *.iso iso/boot/kernel.elf
