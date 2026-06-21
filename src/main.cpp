#include <iostream>
#include <vector>
#include <string>

#include "executer.hpp"
#include "parser.hpp"

int main() {
  std::vector<std::string> history;

  while (true) {
    std::cout << "nsh> ";
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
      continue;
    }

    history.push_back(input);

    std::vector<std::string> tokens = Parser::parseInput(input);

    if (tokens[0] == "history") {
      for (size_t i = 0; i < history.size(); i++) {
        std::cout << i + 1 << " " << history[i] << std::endl;
      }
      continue;
    }

    Executer::execute(tokens);
  }
}
