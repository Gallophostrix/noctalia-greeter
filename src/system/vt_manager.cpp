#include "system/vt_manager.h"

#include "core/log.h"

#include <fcntl.h>
#include <format>
#include <linux/kd.h>
#include <linux/vt.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace noctalia::system {

  VtManager::~VtManager() { release(); }

  bool VtManager::acquire(bool debugMode) {
    m_debug = debugMode;
    if (m_debug) {
      core::log(core::LogLevel::Info, "vt", "--debug active; skipping VT acquisition");
      return true;
    }

    m_ttyFd = ::open("/dev/tty0", O_RDWR | O_CLOEXEC);
    if (m_ttyFd < 0) {
      core::log(core::LogLevel::Error, "vt", "failed to open /dev/tty0");
      return false;
    }

    vt_stat state{};
    if (::ioctl(m_ttyFd, VT_GETSTATE, &state) == 0) {
      m_previousVt = state.v_active;
    }
    if (::ioctl(m_ttyFd, VT_OPENQRY, &m_vtNumber) < 0 || m_vtNumber < 0) {
      core::log(core::LogLevel::Error, "vt", "failed to find a free VT");
      return false;
    }
    if (::ioctl(m_ttyFd, VT_ACTIVATE, m_vtNumber) < 0 || ::ioctl(m_ttyFd, VT_WAITACTIVE, m_vtNumber) < 0) {
      core::log(core::LogLevel::Error, "vt", "failed to switch to greeter VT");
      return false;
    }

    const auto vtPath = std::format("/dev/tty{}", m_vtNumber);
    m_vtFd = ::open(vtPath.c_str(), O_RDWR | O_CLOEXEC);
    if (m_vtFd < 0) {
      core::log(core::LogLevel::Error, "vt", std::format("failed to open {}", vtPath));
      return false;
    }
    setGraphicsMode(true);
    core::log(core::LogLevel::Info, "vt", std::format("acquired VT {}", m_vtNumber));
    return true;
  }

  void VtManager::setGraphicsMode(bool graphics) {
    if (m_vtFd < 0) {
      return;
    }
    const auto mode = graphics ? KD_GRAPHICS : KD_TEXT;
    if (::ioctl(m_vtFd, KDSETMODE, mode) < 0) {
      core::log(core::LogLevel::Warn, "vt", graphics ? "failed to enter graphics mode" : "failed to restore text mode");
    }
  }

  void VtManager::release() {
    if (!m_debug) {
      setGraphicsMode(false);
      if (m_ttyFd >= 0 && m_previousVt > 0) {
        (void)::ioctl(m_ttyFd, VT_ACTIVATE, m_previousVt);
        (void)::ioctl(m_ttyFd, VT_WAITACTIVE, m_previousVt);
      }
    }
    if (m_vtFd >= 0) {
      ::close(m_vtFd);
      m_vtFd = -1;
    }
    if (m_ttyFd >= 0) {
      ::close(m_ttyFd);
      m_ttyFd = -1;
    }
  }

} // namespace noctalia::system
