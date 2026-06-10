{
  lib,
  stdenv,
  meson,
  ninja,
  pkg-config,
  wayland-scanner,
  makeWrapper,
  # Greeter (client Wayland)
  wayland,
  wayland-protocols,
  libGL,
  libglvnd,
  freetype,
  fontconfig,
  cairo,
  pango,
  libxkbcommon,
  libwebp,
  glib,
  librsvg,
  # Compositor (wlroots)
  wlroots_0_20,
  # Runtime session
  polkit,
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
    nativeBuildInputs = [
      meson
      ninja
      pkg-config
      wayland-scanner
      makeWrapper
    ];
    buildInputs = [
      # Greeter client
      wayland
      wayland-protocols
      libGL
      libglvnd
      freetype
      fontconfig
      cairo
      pango
      libxkbcommon
      libwebp
      glib
      librsvg
      # Compositor
      wlroots_0_20
    ];
    mesonFlags = ["--buildtype=release"];
    postInstall = ''
      wrapProgram $out/bin/noctalia-greeter-session \
        --prefix PATH : ${lib.makeBinPath [polkit]}
    '';
    meta = with lib; {
      description = "A minimal login greeter for greetd that matches the look and feel of Noctalia Shell.";
      homepage = "https://github.com/noctalia-dev/noctalia-greeter";
      license = licenses.mit;
      platforms = platforms.linux;
      mainProgram = "noctalia-greeter";
    };
  }
