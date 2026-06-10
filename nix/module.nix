{
  config,
  lib,
  ...
}: let
  cfg = config.services.noctalia-greeter;
in {
  options.services.noctalia-greeter = {
    enable = lib.mkEnableOption "Noctalia Greeter for greetd";
    package = lib.mkOption {
      type = lib.types.package;
      description = "The noctalia-greeter package to use.";
    };
    defaultSession = lib.mkOption {
      type = lib.types.nullOr lib.types.str;
      default = null;
      description = "Session sélectionnée par défaut (ex: \"hyprland\").";
    };
    output = lib.mkOption {
      type = lib.types.nullOr lib.types.str;
      default = null;
      description = "Connecteur sur lequel afficher le greeter (ex: \"DP-1\").";
    };
  };

  config = lib.mkIf cfg.enable {
    services.greetd = {
      enable = true;
      settings.default_session = {
        user = "greeter";
        command = lib.concatStringsSep " " (
          ["${cfg.package}/bin/noctalia-greeter-session"]
          ++ lib.optional (cfg.defaultSession != null)
          "-- --session ${cfg.defaultSession}"
        );
      };
    };
    systemd.tmpfiles.rules = [
      "d /var/lib/noctalia-greeter 0750 greeter greeter -"
    ];
    environment.etc."noctalia-greeter/greeter.conf" = lib.mkIf (cfg.output != null) {
      text = ''output="${cfg.output}"'';
    };
    security.pam.services.greetd = {};
    security.polkit.enable = true;
  };
}
