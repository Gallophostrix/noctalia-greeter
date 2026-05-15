#include "auth/pam_authenticator.h"

#include <cstring>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <string>
#include <utility>

namespace noctalia::auth {
  namespace {
    struct ConversationData {
      const std::string* password = nullptr;
    };

    int converse(int count, const pam_message** messages, pam_response** responses, void* appdata) {
      if (count <= 0 || messages == nullptr || responses == nullptr) {
        return PAM_CONV_ERR;
      }

      auto* data = static_cast<ConversationData*>(appdata);
      auto* reply = static_cast<pam_response*>(calloc(static_cast<std::size_t>(count), sizeof(pam_response)));
      if (reply == nullptr) {
        return PAM_BUF_ERR;
      }

      for (int i = 0; i < count; ++i) {
        const int style = messages[i] != nullptr ? messages[i]->msg_style : PAM_ERROR_MSG;
        if (style == PAM_PROMPT_ECHO_OFF || style == PAM_PROMPT_ECHO_ON) {
          const char* text = data != nullptr && data->password != nullptr ? data->password->c_str() : "";
          reply[i].resp = strdup(text);
          if (reply[i].resp == nullptr) {
            for (int j = 0; j < i; ++j) {
              free(reply[j].resp);
            }
            free(reply);
            return PAM_BUF_ERR;
          }
        }
      }

      *responses = reply;
      return PAM_SUCCESS;
    }

    PamResult authenticateInternal(const std::string& user, const std::string& password, const char* service,
                                   pam_handle** outPam) {
      ConversationData data{.password = &password};
      pam_conv conv{.conv = converse, .appdata_ptr = &data};
      pam_handle_t* pam = nullptr;

      int rc = pam_start(service, user.c_str(), &conv, &pam);
      if (rc != PAM_SUCCESS) {
        return {.ok = false, .error = pam_strerror(pam, rc), .environment = {}};
      }

      rc = pam_authenticate(pam, 0);
      if (rc == PAM_SUCCESS) {
        rc = pam_acct_mgmt(pam, 0);
      }
      if (rc == PAM_NEW_AUTHTOK_REQD) {
        rc = pam_chauthtok(pam, PAM_CHANGE_EXPIRED_AUTHTOK);
      }
      if (rc == PAM_SUCCESS) {
        rc = pam_open_session(pam, 0);
      }

      PamResult result;
      result.ok = rc == PAM_SUCCESS;
      if (!result.ok) {
        result.error = pam_strerror(pam, rc);
        pam_end(pam, rc);
        return result;
      }

      if (char** env = pam_getenvlist(pam); env != nullptr) {
        for (char** it = env; *it != nullptr; ++it) {
          result.environment.emplace_back(*it);
          free(*it);
        }
        free(env);
      }

      if (outPam != nullptr) {
        *outPam = pam;
      } else {
        pam_close_session(pam, 0);
        pam_end(pam, rc);
      }
      return result;
    }
  } // namespace

  PamSession::PamSession(pam_handle* pam, std::vector<std::string> environment) noexcept
      : m_pam(pam), m_environment(std::move(environment)) {}

  PamSession::~PamSession() { close(); }

  PamSession::PamSession(PamSession&& other) noexcept
      : m_pam(other.m_pam), m_environment(std::move(other.m_environment)) {
    other.m_pam = nullptr;
  }

  PamSession& PamSession::operator=(PamSession&& other) noexcept {
    if (this != &other) {
      close();
      m_pam = other.m_pam;
      m_environment = std::move(other.m_environment);
      other.m_pam = nullptr;
    }
    return *this;
  }

  void PamSession::close() {
    if (m_pam == nullptr) {
      return;
    }
    (void)pam_close_session(m_pam, 0);
    (void)pam_end(m_pam, PAM_SUCCESS);
    m_pam = nullptr;
    m_environment.clear();
  }

  PamResult PamAuthenticator::authenticate(const std::string& user, const std::string& password, const char* service) {
    return authenticateInternal(user, password, service, nullptr);
  }

  PamSession PamAuthenticator::openSession(const std::string& user, const std::string& password, const char* service) {
    m_lastError.clear();
    pam_handle* pam = nullptr;
    const PamResult result = authenticateInternal(user, password, service, &pam);
    if (!result.ok || pam == nullptr) {
      m_lastError = result.error;
      return {};
    }
    return PamSession(pam, result.environment);
  }

} // namespace noctalia::auth
