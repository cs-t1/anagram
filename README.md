# anagram
QEMU simulation backend and baremetal testsuite for the anagRAM attack family

## QEMU Setup instructions

### With Nix

Open a devshell with the QEMU requirements and build custom QEMU:

```sh
nix-shell -E 'let pkgs = import <nixpkgs> {}; in pkgs.mkShell { inputsFrom = [ pkgs.qemu ]; }'
cd qemu
# Configure QEMU to the desired target
./configure --target-list=x86_64-softmmu --prefix=$(pwd)

# build
make -j 4
```

This makes the qemu binary available at `qemu/build/qemu-system-x86_64`.

## Kernel build and test instructions

### With Nix

```sh
# Open main dev shell
nix develop

cd baremetal

nix-build

# Run
../qemu/build/qemu-system-x86_64 -cdrom result/bos.iso -m 32M
```
