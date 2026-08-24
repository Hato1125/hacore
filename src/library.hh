#ifndef _HACORE_LIBRARY_HH
#define _HACORE_LIBRARY_HH

#include <filesystem>
#include <span>
#include <string>
#include <vector>

#include "launch.hh"

namespace hacore {
  struct load_options {
    bool enable_steam = false;
    bool enable_gog = false;
  };

  class game {
  public:
    game(
      std::string name,
      std::string sort,
      hacore::launch launch
    ) noexcept;

    [[nodiscard]] const std::string& name() const noexcept;
    [[nodiscard]] const std::string& sort() const noexcept;
    [[nodiscard]] const hacore::launch& launch_options() const noexcept;

  private:
    std::string _name;
    std::string _sort;
    hacore::launch _launch;
  };

  class library {
    struct serializable_game {
      std::string name;
      std::string sort;
    };

    struct serializable_launch {
      std::string root_dir;
      std::string working_dir;
      std::string exe;
      std::vector<std::string> args;
      std::vector<hacore::wrapper> wrapper;
    };

  public:
    library(std::filesystem::path dir) noexcept;

    bool load(load_options opitons);

    [[nodiscard]] std::span<const game> games() const noexcept;
    [[nodiscard]] std::span<const game> steam_games() const noexcept;
    [[nodiscard]] std::span<const game> gog_games() const noexcept;

  private:
    std::filesystem::path _dir;
    std::vector<game> _games;
    std::vector<game> _steam_games;
    std::vector<game> _gog_games;
  };
}

#endif
