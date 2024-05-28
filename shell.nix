with import <nixpkgs> { };
mkShell rec {
  nativeBuildInputs = [
    meson
    ninja
    pkg-config
    cmake-format
    clang-tools
    desktop-file-utils
    plantuml
    cambalache
    (callPackage
      "${fetchGit "https://github.com/bkryza/clang-uml"}/packaging/nix" { })
  ];
  buildInputs = [ gtkmm4.dev libadwaita gettext opencv ];
  NIX_LD_LIBRARY_PATH = lib.makeLibraryPath ([ stdenv.cc.cc ] ++ buildInputs);
  NIX_LD = "${stdenv.cc.libc_bin}/bin/ld.so";
}
