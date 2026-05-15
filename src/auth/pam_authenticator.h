#pragma once

#include <string>
#include <vector>

struct pam_handle;

namespace noctalia::auth {

  struct PamResult {
    bool ok = false;
    std::string error;
    std::vector<std::string> environment;
  };

  // Owns a PAM handle from successful authentication until close() or destruction.
  class PamSession {
  public:
    PamSession() = default;
    ~PamSession();

    PamSession(const PamSession&) = delete;
    PamSession& operator=(const PamSession&) = delete;
    PamSession(PamSession&& other) noexcept;
    PamSession& operator=(PamSession&& other) noexcept;

    [[nodiscard]] bool valid() const noexcept { return m_pam != nullptr; }
    [[nodiscard]] const std::vector<std::string>& environment() const noexcept { return m_environment; }

    void close();

  private:
    friend class PamAuthenticator;
    PamSession(pam_handle* pam, std::vector<std::string> environment) noexcept;

    pam_handle* m_pam = nullptr;
    std::vector<std::string> m_environment;
  };

  class PamAuthenticator {
  public:
    PamResult authenticate(const std::string& user, const std::string& password,
                           const char* service = "noctalia-greeter");

    // On success, returns a session handle that must stay open until pam_close_session runs.
    [[nodiscard]] PamSession openSession(const std::string& user, const std::string& password,
                                         const char* service = "noctalia-greeter");
    [[nodiscard]] const std::string& lastError() const noexcept { return m_lastError; }

  private:
    std::string m_lastError;
  };

} // namespace noctalia::auth
