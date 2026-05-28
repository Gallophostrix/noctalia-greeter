#pragma once

#include "config/config_types.h"

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

namespace greeter::appearance {

inline constexpr int kManifestVersion = 1;
inline constexpr const char *kDefaultStateDir = "/var/lib/noctalia-greeter";
inline constexpr const char *kManifestFileName = "appearance.json";
inline constexpr const char *kPreferencesFileName = "state.json";
inline constexpr const char *kWallpaperBaseName = "wallpaper";
// Greeter color-scheme picker label when shell appearance data is installed.
inline constexpr const char *kSyncedSchemeDisplayName = "Synced";
inline constexpr const char *kDefaultGreeterUser = "greeter";
inline constexpr const char *kGreeterUserEnv = "GREETER_USER";
inline constexpr const char *kStateDirEnv = "NOCTALIA_GREETER_STATE_DIR";

[[nodiscard]] std::filesystem::path stateDirectory();
[[nodiscard]] std::filesystem::path manifestPath();
[[nodiscard]] std::filesystem::path preferencesPath();
[[nodiscard]] std::filesystem::path
stagingManifestPath(const std::filesystem::path &stagingDirectory);
[[nodiscard]] bool syncedAppearanceInstalled();

[[nodiscard]] const std::vector<std::string_view> &requiredPaletteKeys();

[[nodiscard]] std::optional<WallpaperFillMode>
parseFillMode(std::string_view value);

// Validates appearance.json in a shell staging directory before install.
[[nodiscard]] bool
validateStagingManifest(const std::filesystem::path &stagingDirectory,
                        std::string &errorOut);

// Copies staging into the greeter state directory and chowns it to the greeter
// user.
[[nodiscard]] bool
installFromStaging(const std::filesystem::path &stagingDirectory,
                   std::string &errorOut);

} // namespace greeter::appearance
