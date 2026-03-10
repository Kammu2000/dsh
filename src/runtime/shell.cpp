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
    evaluator.evaluate(*ast.stmts[0]);
  }

  return 0;
}
