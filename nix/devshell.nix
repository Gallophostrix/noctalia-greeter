{
  pkgs,
  noctalia-greeter,
}:
pkgs.mkShell {
  inputsFrom = [noctalia-greeter];
  nativeBuildInputs = with pkgs; [
    just
    lefthook
    clang-tools
    gnugrep
    gnused
    findutils
    gdb
  ];
  shellHook = ''
    export NOCTALIA_GREETER_ASSETS_DIR="$PWD/assets"
    echo "Noctalia-greeter dev-shell | 'just --list' to see available tasks"
  '';
}
