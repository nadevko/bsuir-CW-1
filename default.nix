{ pkgs ? import <nixpkgs> { } }:
with pkgs;
stdenv.mkDerivation rec {
  pname = "cw1";
  version = "1.0";

  src = ./.;

  nativeBuildInputs = [
    meson
    ninja
    pkg-config
    gtkmm4
    git
    wrapGAppsHook4
    desktop-file-utils
    gettext
    opencv
  ];

  meta = with lib; {
    description = "Image dedublication tool";
    homepage = "https://nadevko.github.io/bsuir-CW-1";
    downloadPage = "https://github.com/nadevko/bsuir-CW-1/releases";
    license = licenses.gpl3Only;
    platforms = platforms.linux;
    mainProgram = pname;
  };
}
