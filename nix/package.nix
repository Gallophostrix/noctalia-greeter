{
  lib,
  stdenv,
  meson,
  ninja,
  pkg-config,
  wayland-scanner,
  makeWrapper,
  cage,
  wlr-randr,
  wayland,
  wayland-protocols,
  libGL,
  freetype,
  fontconfig,
  cairo,
  pango,
  libxkbcommon,
  libwebp,
  glib,
  librsvg,
}: let
  inherit (builtins) head match readFile;
  version = head (match ".*version: '([^']+)'.*" (readFile ../meson.build));
in
  stdenv.mkDerivation {
    pname = "noctalia-greeter";
    inherit version;
    src = lib.cleanSource ./..;
    postPatch = ''
      sed -i "s/'-march=native', '-mtune=native',//" meson.build
    '';
    nativeBuildInputs = [meson ninja pkg-config wayland-scanner makeWrapper];
    buildInputs = [
      wayland
      wayland-protocols
      libGL
      freetype
      fontconfig
      cairo
      pango
      libxkbcommon
      libwebp
      glib
      librsvg
    ];
    mesonFlags = ["--buildtype=release"];
    postInstall = ''
      wrapProgram $out/bin/noctalia-greeter \
        --prefix PATH : ${lib.makeBinPath [cage wlr-randr]}
      wrapProgram $out/bin/noctalia-greeter-session \
        --prefix PATH : ${lib.makeBinPath [cage wlr-randr]}
    '';
    meta = with lib; {
      description = "A minimal login greeter for greetd that matches the look and feel of Noctalia Shell.";
      homepage = "https://github.com/noctalia-dev/noctalia-greeter";
      license = licenses.mit;
      platforms = platforms.linux;
      mainProgram = "noctalia-greeter";
    };
  }
