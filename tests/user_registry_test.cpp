#include "system/user_registry.h"

#include <iostream>

namespace {

  bool expect(bool condition, const char* message) {
    if (!condition) {
      std::cerr << "FAIL: " << message << '\n';
      return false;
    }
    return true;
  }

} // namespace

int main() {
  bool ok = true;
  ok &= expect(noctalia::system::isLoginCapableUser(1000, "/bin/bash"), "regular user with shell is login-capable");
  ok &= expect(!noctalia::system::isLoginCapableUser(999, "/bin/bash"), "uid below 1000 is rejected");
  ok &= expect(!noctalia::system::isLoginCapableUser(1000, "/usr/sbin/nologin"), "nologin shells are rejected");
  ok &= expect(!noctalia::system::isLoginCapableUser(1000, nullptr), "missing shell is rejected");
  ok &= expect(noctalia::system::isLoginCapableUser(1001, "/bin/zsh"), "higher uid with shell is accepted");
  return ok ? 0 : 1;
}
