#pragma once

#include <string>
#include <sys/types.h>
#include <vector>

namespace noctalia::system {

  struct User {
    std::string name;
    std::string realName;
    std::string home;
    std::string shell;
    uid_t uid = 0;
    gid_t gid = 0;
  };

  // True for normal login accounts (excludes system UIDs and nologin shells).
  [[nodiscard]] bool isLoginCapableUser(uid_t uid, const char* shell);

  std::vector<User> enumerateUsers();

} // namespace noctalia::system
