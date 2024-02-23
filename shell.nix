{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.clang-tools_17
  ];
  shellHook = ''
    export PATH=$PATH:${pkgs.clang-tools}/bin
    exec fish
  '';
}
