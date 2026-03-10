#include "runtime/evaluator.hpp"
#include "frontend/parser.hpp"
#include <array>
#include <cstdlib>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

constexpr mode_t DEFAULT_FILE_MODE = 0644;

void Evaluator::visit(const PipeExpression &expr) { evaluatePipe(expr); }

void Evaluator::visit(const CommandExpression &expr) { evaluateCommand(expr); }

void Evaluator::evaluate(const Expression &expr) { expr.accept(*this); }

void Evaluator::evaluatePipe(const PipeExpression &pipeExpr) {
  std::array<int, 2> fileDesc{};
  pipe(fileDesc.data());

  // left process
  pid_t left = fork();

  if (left == 0) {
    dup2(fileDesc[1], STDOUT_FILENO);
    close(fileDesc[0]);
    close(fileDesc[1]);
    this->evaluate(*pipeExpr.left);
    exit(0);
  }

  // right process
  pid_t right = fork();

  if (right == 0) {
    dup2(fileDesc[0], STDIN_FILENO);
    close(fileDesc[0]);
    close(fileDesc[1]);
    this->evaluate(*pipeExpr.right);
    exit(0);
  }

  close(fileDesc[0]);
  close(fileDesc[1]);
  waitpid(left, nullptr, 0);
  waitpid(right, nullptr, 0);
}

void Evaluator::evaluateCommand(const CommandExpression &cmd) {
  pid_t pid = fork();

  if (pid == 0) {
    for (const auto &redirect : cmd.redirects) {
      int fileDesc = 0;

      if (redirect.type == RedirectType::REDIRECT_OUT) {
        // NOLINTNEXTLINE
        fileDesc = open(redirect.fileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC,
                        DEFAULT_FILE_MODE);
        dup2(fileDesc, STDOUT_FILENO);
      } else if (redirect.type == RedirectType::REDIRECT_APPEND) {
        // NOLINTNEXTLINE
        fileDesc = open(redirect.fileName.c_str(),
                        O_WRONLY | O_CREAT | O_APPEND, DEFAULT_FILE_MODE);
        dup2(fileDesc, STDOUT_FILENO);
      } else if (redirect.type == RedirectType::REDIRECT_IN) {
        // NOLINTNEXTLINE
        fileDesc = open(redirect.fileName.c_str(), O_RDONLY);
        dup2(fileDesc, STDIN_FILENO);
      }

      close(fileDesc);
    }

    std::vector<char *> argv;
    argv.push_back(const_cast<char *>(cmd.command.c_str())); // NOLINT

    for (const auto &arg : cmd.args) {
      argv.push_back(const_cast<char *>(arg.c_str())); // NOLINT
    }

    execvp(cmd.command.c_str(), argv.data());

    perror("Execution failed");
    exit(1);
  } else {
    waitpid(pid, nullptr, 0);
  }
}
