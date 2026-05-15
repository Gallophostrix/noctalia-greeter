#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace noctalia::session {

  enum class SessionType { X11, Wayland };

  struct SessionSearchDir {
    std::filesystem::path path;
    SessionType type = SessionType::Wayland;
  };

  struct Session {
    std::string id;
    std::string name;
    std::string exec;
    std::filesystem::path desktopFile;
    SessionType type = SessionType::Wayland;
  };

  std::vector<Session> discoverSessions();
  std::vector<Session> discoverSessionsFromDirs(const std::vector<SessionSearchDir>& dirs);

  // Strips desktop-entry Exec field codes (%f, %u, %k, …) before session launch.
  [[nodiscard]] std::string sanitizeSessionExec(std::string_view exec);

} // namespace noctalia::session
