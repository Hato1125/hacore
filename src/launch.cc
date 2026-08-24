#include <linux/prctl.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

#include <format>
#include <ranges>

#include "launch.hh"

namespace hacore {
  void launch::run() {
    const auto wrapper = wrappers
      | std::views::transform([](const auto& wp) {
          auto& [cmd, args] = wp;

          if (args.empty()) {
            return cmd;
          }

          const auto joined = args
            | std::views::join_with(' ')
            | std::ranges::to<std::string>();

          return std::format("{} {}", cmd, joined);
        })
      | std::views::join_with(' ')
      | std::ranges::to<std::string>();

    const auto cmd = std::format(
      "{} {} {}",
      wrapper,
      exe,
      args
        | std::views::join_with(' ')
        | std::ranges::to<std::string>()
    );

    /*
      LAUNCHER ... forks REAPER, returns immediately
        └─REAPER ... subreaper; reaps GAME + orphans, exits with GAME status
           └─GAME ... runs cmd via /bin/sh in its own process group
    */

    pid_t supervisor = fork();

    if (supervisor == -1) {
      perror("fork supervisor");
      return;
    }

    if (supervisor == 0) {
      if (prctl(PR_SET_CHILD_SUBREAPER, 1) == -1) {
        perror("prctl");
        _exit(125);
      }

      pid_t target = fork();

      if (target == -1) {
        perror("fork target");
        _exit(125);
      }

      if (target == 0) {
        if (setpgid(0, 0) == -1) {
          perror("setpgid");
          _exit(125);
        }

        execl(
          "/bin/sh",
          "sh",
          "-c",
          cmd.c_str(),
          static_cast<char*>(nullptr)
        );
        perror("execl");
        _exit(127);
      }

      int status = 0;
      int target_status = 0;
      pid_t w;

      while ((w = waitpid(-1, &status, 0)) > 0) {
        if (w == target) {
          target_status = status;
        }
      }

      _exit(
        WIFEXITED(target_status)
          ? WEXITSTATUS(target_status)
          : WTERMSIG(target_status) + 128
      );
    }
  }
}
