{ stdenv, crossgcc, grub2, xorriso, is_64bits ? false, ... }:
let baseName = if is_64bits then "x86_64" else "i686";
  flags = "-std=gnu99 -ffreestanding -O2 -Wall -Wextra -Wpedantic";
in stdenv.mkDerivation {
  name = "bos";
  src = ./.;
  #dontUnpack = true;
  dontInstall = true;
  preferLocalBuild = true;
  buildInputs = [ crossgcc grub2 xorriso ];
  buildPhase = ''
    mkdir $out

    # Build the library
    ${baseName}-elf-gcc -c src/stdio/print.c -o print.o  ${flags}
    ${baseName}-elf-gcc -c src/stdio/putchar.c -o putchar.o ${flags}
    ${baseName}-elf-gcc -c src/arch/tty.c -o tty.o ${flags}
    ${baseName}-elf-gcc -c src/arch/multiboot.c -o multiboot.o ${flags}
    ${baseName}-elf-gcc -c src/faults.c -o faults.o ${flags}
    ${baseName}-elf-gcc -c src/cache_driver.c -o cache_driver.o ${flags}

    # Build the kernel
    ${baseName}-elf-as boot.s -o boot.o
    ${baseName}-elf-gcc -c src/kernel.c -o kernel.o ${flags}

    # Link the kernel and output a binary
    ${baseName}-elf-gcc -T linker.ld -o $out/bos.bin -ffreestanding -O2 -nostdlib *.o -lgcc -z max-page-size=0x1000

    # Setup and make the boot ISO
    mkdir -p $out/iso/boot/grub
    cp grub.cfg $out/iso/boot/grub/grub.cfg
    cp $out/bos.bin $out/iso/boot/bos.bin
    grub-mkrescue -o $out/bos.iso $out/iso
  '';
  out = [ "myos.bin" ];
}

