#include <cstddef>
#include <frontend/parser.hpp>
#include <iostream>
#include <runtime/evaluator.hpp>
#include <string>

int main() {
  while (true) {
    std::cout << "$ ";
    std::string code;

    // exit signal detection
    if (!std::getline(std::cin, code)) {
      break; // ctrl + d
    }

    ProgramParser parser(code);
    Program ast = parser.buildAST();

    Evaluator evaluator;

    for (const auto &stmt : ast.stmts) {
      evaluator.evaluate(*stmt);
    }
  }

  return 0;
}
