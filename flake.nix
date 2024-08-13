{
  description = "anagRAM flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    pre-commit-hooks.url = "github:cachix/pre-commit-hooks.nix";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        crossgcc = pkgs.pkgsCross.i686-embedded.buildPackages.gcc;
        bos = pkgs.callPackage ./baremetal/bos.nix {
          crossgcc = crossgcc;
          grub2 = pkgs.grub2;
          xorriso = pkgs.xorriso;
        };
      in {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            pkgs.pkgsCross.x86_64-embedded.buildPackages.gcc

            gdb

            grub2
            qemu
            xorriso

            # bos
          ];
        };
      });
}

