with import <nixpkgs> { };
mkShell rec {
  stdenv = gccStdenv;
  nativeBuildInputs = [ cambalache cmake-format ];
  buildInputs = [ (import ./. { inherit pkgs; }) ];
  NIX_LD_LIBRARY_PATH = lib.makeLibraryPath ([ stdenv.cc.cc ] ++ buildInputs);
  NIX_LD = "${stdenv.cc.libc_bin}/bin/ld.so";
}
