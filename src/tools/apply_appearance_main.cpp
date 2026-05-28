#include "core/log.h"
#include "greeter/appearance_sync.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

namespace {

constexpr Logger kLog("apply-appearance");

void printUsage(const char *programName) {
  std::cerr
      << "usage: " << programName << " <staging-directory>\n"
      << "\n"
      << "Installs noctalia-shell appearance data staged by the shell into\n"
      << greeter::appearance::stateDirectory().string() << ".\n"
      << "Environment:\n"
      << "  " << greeter::appearance::kStateDirEnv
      << "  override install directory (default "
      << greeter::appearance::kDefaultStateDir << ")\n"
      << "  " << greeter::appearance::kGreeterUserEnv
      << "  greeter account name (default "
      << greeter::appearance::kDefaultGreeterUser << ")\n";
}

} // namespace

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printUsage(argv[0] != nullptr ? argv[0]
                                  : "noctalia-greeter-apply-appearance");
    return 2;
  }

  const std::filesystem::path stagingDirectory = argv[1];
  std::string error;
  if (!greeter::appearance::installFromStaging(stagingDirectory, error)) {
    kLog.error("{}", error);
    return 1;
  }

  kLog.info("installed appearance from '{}'", stagingDirectory.string());
  return 0;
}
