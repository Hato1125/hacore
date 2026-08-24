#ifndef _HACORE_LAUNCH_HH
#define _HACORE_LAUNCH_HH

#include <string>
#include <vector>
#include <utility>
#include <filesystem>

namespace hacore {
  struct wrapper {
    std::string exe;
    std::vector<std::string> args;
  };

  class launch {
  public:
    std::filesystem::path root_dir;
    std::filesystem::path working_dir;
    std::filesystem::path exe;
    std::vector<std::string> args;
    std::vector<wrapper> wrappers;

    void run();
  };
}

#endif
