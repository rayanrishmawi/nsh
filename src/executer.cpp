#include "executer.hpp"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <vector>

#include "builtins.hpp"

void Executer::execute(const std::vector<std::string>& tokens) {
  if (tokens.empty()) {
    return;
  }

  std::vector<std::string> args = tokens;

  bool background = false;

  if (!args.empty() && args.back() == "&") {
    background = true;
    args.pop_back();
  }

  if (Builtins::handle(args)) {
    return;
  }

  pid_t pid = fork();

  if (pid < 0) {
    std::cerr << args[0] << ": failed to execute command" << std::endl;
  } else if (pid == 0) {
    for (size_t i = 0; i < args.size(); i++) {
      if (args[i] == ">") {
        if (i + 1 >= args.size()) {
          std::cerr << "missing output file" << std::endl;
          _exit(1);
        }

        int file_fd = open(args[i + 1].c_str(),
                           O_WRONLY | O_CREAT | O_TRUNC,
                           0644);

        if (file_fd < 0) {
          perror("open");
          _exit(1);
        }

        dup2(file_fd, STDOUT_FILENO);
        close(file_fd);

        args.erase(args.begin() + i, args.begin() + i + 2);
        break;
      }
    }

    std::vector<const char*> argv;

    for (const std::string& arg : args) {
      argv.push_back(arg.c_str());
    }

    argv.push_back(nullptr);

    int status = execvp(argv[0], const_cast<char* const*>(argv.data()));

    if (status != 0) {
      std::string msg = "failed to execute command";

      if (errno == ENOENT) {
        msg = "command not found";
      }

      std::cerr << args[0] << ": " << msg << std::endl;
    }

    _exit(1);
  } else {
    if (!background) {
      waitpid(pid, nullptr, 0);
    }
  }
}
