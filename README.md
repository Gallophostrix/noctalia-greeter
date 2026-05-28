# Noctalia Greeter

**_quiet by design_**


<p align="center">
  <img src="https://assets.noctalia.dev/noctalia-logo.svg?v=2" alt="Noctalia Logo" style="width: 192px" />
</p>
 
 
 
 

---

## What is Noctalia Greeter?

A minimal, modern login greeter for [greetd](https://github.com/kennylevinsen/greetd), designed to match the Noctalia look and feel.

It runs as a Wayland client inside [Cage](https://github.com/cage-kiosk/cage), uses the same UI/theming stack as [Noctalia Shell](https://github.com/noctalia-dev/noctalia-shell), and focuses on a clean, reliable authentication flow.

---

## 📋 Requirements

- `greetd`
- `cage`
- `dbus` (`dbus-run-session`)
- A `greeter` system user

Build tools:

- Meson + Ninja
- C++20 compiler
- `pkg-config`
- `just` (optional, but recommended)

Build-time libraries (pkg-config names):

- `wayland-client`, `wayland-protocols`
- `xkbcommon`
- `freetype2`, `fontconfig`
- `cairo`, `cairo-ft`, `pango`, `pangocairo`, `pangoft2`
- `librsvg-2.0`
- `glib-2.0`, `gobject-2.0`, `gio-2.0`
- `egl` / `glesv2` / `wayland-egl` (or `epoxy` fallback)
- `libwebp`

---

## 🚀 Getting Started

### 1) Build

```bash
meson setup build
meson compile -C build
```

or:

```bash
just build
```

### 2) Install

```bash
just install
```

This installs the greeter binaries, session launcher, polkit policy, and assets, then runs system setup:

- `scripts/setup_greetd_pam.sh`
- `scripts/setup_greeter_system.sh`

### 3) Configure greetd

Add this to `/etc/greetd/config.toml`:

```toml
[default_session]
command = "/usr/local/bin/noctalia-greeter-session"
user = "greeter"
```

If your install prefix is different, use the installed path for `noctalia-greeter-session`.

### 4) Restart greetd

```bash
sudo systemctl restart greetd
# or
sudo sv restart greetd
```

---

## Packaging

Meson installs the following (paths use your `prefix`, commonly `/usr/local`):

| Artifact | Install location | Role |
|----------|------------------|------|
| `noctalia-greeter` | `bindir` | Login UI (Wayland client under Cage) |
| `noctalia-greeter-session` | `bindir` | greetd session command (`cage` + greeter) |
| `noctalia-greeter-apply-appearance` | `bindir` | Root helper for shell → greeter appearance sync |
| `assets/` | `share/noctalia-greeter/assets` | Fonts, icons, etc. |
| `org.noctalia.greeter.apply-appearance.policy` | `share/polkit-1/actions` | polkit rule for the sync helper |

**Runtime state** (created by setup scripts and the sync helper):

- `/var/lib/noctalia-greeter/` — greeter-owned appearance data (`appearance.json`, optional `wallpaper.*`)
- `/var/log/noctalia-greeter.log`, `/var/lib/noctalia-greeter/greeter.log` — logs (see `just setup-log-dir`)

**Environment overrides** (optional):

- `NOCTALIA_GREETER_STATE_DIR` — override appearance install directory (default `/var/lib/noctalia-greeter`)
- `GREETER_USER` — account that owns state files (default `greeter`)
- `NOCTALIA_GREETER_ASSETS_DIR` — asset root when not using the installed `share/noctalia-greeter/assets` tree

### Appearance sync (Noctalia Shell v5)

Appearance sync is only supported with **[Noctalia Shell v5](https://github.com/noctalia-dev/noctalia-shell/tree/v5)** (the `v5` branch). Older shell releases do not include the settings control or staging flow.

From **Settings → Shell → Security → Noctalia Greeter → Sync Now**, the shell:

1. Stages `appearance.json` (and a wallpaper file when needed) under the user’s `$XDG_RUNTIME_DIR/noctalia-greeter-sync/`
2. Runs `pkexec noctalia-greeter-apply-appearance <staging-dir>` (admin prompt via polkit)
3. Installs into `/var/lib/noctalia-greeter/` with mode `0644` / directory `0755`, owned by `greeter`

The greeter reads `appearance.json` on startup and adds a **Synced** entry to the color-scheme picker (built-in palettes keep solid backgrounds). Each successful sync also writes `/var/lib/noctalia-greeter/state.json` with `"scheme_name": "Synced"` so the greeter defaults to that scheme on the next login (session choice is preserved when already set). **Both packages must be installed** (shell v5 + greeter + polkit policy). After syncing, restart greetd or log out once to see the shell wallpaper and palette.

Manual test of the helper (as root), after staging a directory:

```bash
sudo ./build/noctalia-greeter-apply-appearance /run/user/1000/noctalia-greeter-sync
```

---

## Development

Run locally under Cage:

```bash
just run-local
```

Run inside an existing Wayland session:

```bash
just run-niri
```

To force single-monitor mode for debugging, use Cage with `-m last`:

```bash
dbus-run-session cage -s -m last -- ./build/noctalia-greeter
```

AddressSanitizer:

```bash
just run-cage-asan
```

Recovery helper:

```bash
just recover
```

---

## Scope

Noctalia Greeter is a **display/login greeter** for greetd. It handles user/session selection and authentication UI.

It is **not** a desktop shell or compositor replacement.

---

## 🤝 Contributing

Contributions are welcome: fixes, polish, docs, or UX improvements.

- Open an issue for bugs and regressions
- Open a PR for improvements

---

## 📄 License

MIT License.

