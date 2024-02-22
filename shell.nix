{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.clang_17
  ];
  shellHook = ''
    exec fish
  '';
}
