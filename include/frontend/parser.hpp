#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

enum class RedirectType : uint8_t {
  REDIRECT_IN,
  REDIRECT_OUT,
  REDIRECT_APPEND
};
enum class ExpressionType : uint8_t { PIPE, COMMAND };

class Redirect {
public:
  RedirectType type;
  std::string fileName;
};

// TODO:need to learn about all kinds of special member functions written here
class Expression {
public:
  Expression() = default;
  virtual ~Expression() = default;
  Expression(const Expression &) = delete;
  Expression &operator=(const Expression &) = delete;
  Expression(Expression &&) = default;
  Expression &operator=(Expression &&) = default;
  virtual void print(int indent = 0) const = 0;
};

class PipeExpression : public Expression {
public:
  ExpressionType type;
  std::unique_ptr<Expression> left, right;
  void print(int indent = 0) const override;
};

class CommandExpression : public Expression {
public:
  ExpressionType type;
  std::string command;
  std::vector<std::string> args;
  std::vector<Redirect> redirects;
  void print(int indent = 0) const override;
};

class Program {
public:
  std::vector<std::unique_ptr<Expression>> stmts;
};

class ProgramParser {
private:
  std::string sourceCode;

public:
  explicit ProgramParser(std::string srcCode);
  Program buildAST();
};
