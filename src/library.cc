#include <print>
#include <ranges>

#include <glaze/json/generic.hpp>
#include <glaze/toml.hpp>

#include "library.hh"

namespace hacore {
  game::game(
    std::string name,
    std::string sort,
    hacore::launch launch
  ) noexcept : _name(std::move(name)),
    _sort(std::move(sort)),
    _launch(std::move(launch)) {}

  const std::string& game::name() const noexcept {
    return _name;
  }

  const std::string& game::sort() const noexcept {
    return _sort;
  }

  const hacore::launch& game::launch_options() const noexcept {
    return _launch;
  }

  library::library(std::filesystem::path dir) noexcept
    : _dir(std::move(dir)) {}

  bool library::load(load_options opitons) {
    /*
      Load each game from a directory with the following structure:

      <library>/
        └─<game>/
          ├─game.toml
          └─launch.toml
    */

    namespace fs = std::filesystem;

    _games.clear();
    _steam_games.clear();
    _gog_games.clear();

    for (const auto& dir : fs::directory_iterator(_dir)) {
      const auto launch_file = dir.path() / "launch.toml";
      const auto game_file = dir.path() / "game.toml";
      if (!fs::exists(launch_file) || !fs::exists(game_file)) {
        continue;
      }

      std::string buffer;

      serializable_launch pslaunch;
      const auto lerror = glz::read_file_toml(
        pslaunch,
        launch_file.string(),
        buffer
      );
      if (lerror) {
        std::println(
          stderr,
          "Failed to read {}: {}",
          launch_file.string(),
          glz::format_error(lerror, buffer)
        );
        continue;
      }

      serializable_game psgame;
      const auto gerror = glz::read_file_toml(
        psgame,
        game_file.string(),
        buffer
      );
      if (gerror) {
        std::println(
          stderr,
          "Failed to read {}: {}",
          game_file.string(),
          glz::format_error(gerror, buffer)
        );
        continue;
      }

      _games.emplace_back(
        psgame.name,
        psgame.sort,
        launch{
          .root_dir = fs::path(std::move(pslaunch.root_dir)),
          .working_dir = fs::path(std::move(pslaunch.working_dir)),
          .exe = fs::path(std::move(pslaunch.exe)),
          .args = std::move(pslaunch.args),
          .wrappers = std::move(pslaunch.wrapper),
        }
      );
    }

    return true;
  }

  std::span<const game> library::games() const noexcept {
    return {_games.data(), _games.size()};
  }

  std::span<const game> library::steam_games() const noexcept {
    return {_steam_games.data(), _steam_games.size()};
  }

  std::span<const game> library::gog_games() const noexcept {
    return {_gog_games.data(), _gog_games.size()};
  }
}
