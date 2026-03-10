#include "runtime/astVisitor.hpp"
#include <cstddef>
#include <frontend/lexer.hpp>
#include <frontend/parser.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

static void printIndent(int indent) {
  for (int i = 0; i < indent; i++) {
    std::cout << "  ";
  }
}

class ExpressionParser {
private:
  std::vector<Token> tokens;
  int idx = 0;

public:
  ExpressionParser(std::vector<Token> &tokens) : tokens(tokens) {}

  bool isRedirect() {
    switch (tokens[idx].getType()) {
    case TokenType::REDIRECT_IN:
    case TokenType::REDIRECT_OUT:
    case TokenType::REDIRECT_APPEND:
      return true;

    default:
      return false;
    }

    return false;
  }

  std::unique_ptr<Expression> parseExpression() {
    return parsePipeExpression();
  }

  std::unique_ptr<Expression> parsePipeExpression() {
    size_t len = tokens.size();
    auto left = this->parseCommandExpression();

    while (idx < len && tokens[idx].getType() == TokenType::PIPE) {
      idx++;
      auto right = this->parseCommandExpression();

      auto pipe = std::make_unique<PipeExpression>();
      pipe->left = std::move(left);
      pipe->right = std::move(right);

      left = std::move(pipe);
    }

    return left;
  }

  std::unique_ptr<Expression> parseCommandExpression() {
    auto commandExpr = std::make_unique<CommandExpression>();
    size_t len = tokens.size();

    if (idx < len && tokens[idx].getType() == TokenType::WORD) {
      commandExpr->command = tokens[idx++].getValue();
      commandExpr->args = this->parseCommandArguments();
    }

    commandExpr->redirects = this->parseRedirects();
    return commandExpr;
  }

  std::vector<std::string> parseCommandArguments() {
    size_t len = tokens.size();
    std::vector<std::string> args;

    while (idx < len && tokens[idx].getType() == TokenType::WORD) {
      args.push_back(tokens[idx++].getValue());
    }

    return args;
  }

  std::vector<Redirect> parseRedirects() {
    std::vector<Redirect> redirects;
    size_t len = tokens.size();

    while (idx < len && isRedirect()) {
      Redirect redirect;
      redirect.type = static_cast<RedirectType>(tokens[idx].getType());
      idx++; // consume redirect type

      if (tokens[idx].getType() != TokenType::WORD) {
        throw std::runtime_error("Expected filename after redirect");
      }

      redirect.fileName = tokens[idx].getValue();
      idx++; // consume fileName
      redirects.push_back(redirect);
    }

    return redirects;
  }
};

ProgramParser::ProgramParser(std::string srcCode)
    : sourceCode(std::move(srcCode)) {}

Program ProgramParser::buildAST() {
  Lexer lexer(this->sourceCode);
  std::vector<Token> tokens = lexer.tokenize();

  ExpressionParser exprParser(tokens);
  Program program;

  std::unique_ptr<Expression> temp = exprParser.parseExpression();
  program.stmts.push_back(std::move(temp));

  return program;
}

void CommandExpression::print(int indent) const {
  printIndent(indent);
  std::cout << "Command: " << command << "\n";

  if (!args.empty()) {
    printIndent(indent + 1);
    std::cout << "Args:\n";
    for (const auto &arg : args) {
      printIndent(indent + 2);
      std::cout << arg << "\n";
    }
  }

  if (!redirects.empty()) {
    printIndent(indent + 1);
    std::cout << "Redirects:\n";
    for (const auto &redirect : redirects) {
      printIndent(indent + 2);
      std::cout << "Type: " << static_cast<int>(redirect.type)
                << " File: " << redirect.fileName << "\n";
    }
  }
}

void PipeExpression::print(int indent) const {
  printIndent(indent);
  std::cout << "Pipe\n";

  left->print(indent + 1);
  right->print(indent + 1);
}

void PipeExpression::accept(ASTVisitor &visitor) const { visitor.visit(*this); }

void CommandExpression::accept(ASTVisitor &visitor) const {
  visitor.visit(*this);
};
