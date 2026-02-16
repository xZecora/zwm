# default.nix
with import <nixpkgs> {};
stdenv.mkDerivation {
    name = "zwm"; # Probably put a more meaningful name here
    buildInputs = [ pkg-config xorg.libX11 xorg.libXinerama ];
}
