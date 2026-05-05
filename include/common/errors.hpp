#pragma once
#include <cstddef>
#include <exception>
#include <string>
#include <utility>

class ShellError : public std::exception
{
  public:
    ShellError(std::string message) : message(std::move(message)) {}
    const char* what() const noexcept override { return message.c_str(); }
    virtual std::string type() const noexcept = 0;

  private:
    std::string message;
};

class LexerError : public ShellError
{
  public:
    LexerError(std::string message) : ShellError(std::move(message)) {}
    std::string type() const noexcept override { return "LEXER ERROR"; }
};

class ParserError : public ShellError
{
  public:
    ParserError(std::string message) : ShellError(std::move(message)) {}
    std::string type() const noexcept override { return "PARSER ERROR"; }
};
