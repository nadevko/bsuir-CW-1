with import <nixpkgs> { };
mkShell rec {
  stdenv = gccStdenv;
  nativeBuildInputs = [
    meson
    ninja
    pkg-config
    cmake-format
    desktop-file-utils
    plantuml
    cambalache
    # (import ./. { inherit pkgs; })
  ];
  buildInputs = [ gtkmm4.dev gettext opencv ];
  NIX_LD_LIBRARY_PATH = lib.makeLibraryPath ([ stdenv.cc.cc ] ++ buildInputs);
  NIX_LD = "${stdenv.cc.libc_bin}/bin/ld.so";
}
