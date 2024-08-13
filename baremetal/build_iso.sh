#! sh

cp src/grub.cfg iso/boot/grub/
cp zig-out/bin/kernel iso/boot/kernel.elf

grub-mkrescue -o kernel.iso iso
