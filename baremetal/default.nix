{ }:

let
  pkgs = import <nixpkgs> { };
  #crossgcc = pkgs.pkgsCross.x86_64-embedded.buildPackages.gcc;
  crossgcc = pkgs.pkgsCross.i686-embedded.buildPackages.gcc;
in pkgs.callPackage ./bos.nix {
  crossgcc = crossgcc;
  grub2 = pkgs.grub2;
  xorriso = pkgs.xorriso;
}

