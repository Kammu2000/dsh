#pragma once
#include "runtime/astVisitor.hpp"
#include "lexer.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

enum class RedirectType : uint8_t
{
    REDIRECT_IN,
    REDIRECT_OUT,
    REDIRECT_APPEND
};

enum class ExpressionType : uint8_t
{
    PIPE,
    COMMAND
};

struct Redirect
{
    RedirectType m_type;
    std::string m_file_name;

    Redirect(RedirectType type, std::string file_name)
        : m_type(type), m_file_name(std::move(file_name))
    {
    }
};

class Expression
{
  public:
    Expression() = default;
    virtual ~Expression() = default;
    Expression(const Expression&) = delete;
    Expression& operator=(const Expression&) = delete;
    Expression(Expression&&) = default;
    Expression& operator=(Expression&&) = default;
    virtual void accept(ASTVisitor& visitor) const = 0;
};

class CommandExpression : public Expression
{
  public:
    CommandExpression(std::string command, std::vector<std::string> args,
                      std::vector<Redirect> redirects);
    std::string get_command() const { return m_command; }
    std::vector<std::string> get_args() const { return m_args; }
    std::vector<Redirect> get_redirects() const { return m_redirects; }

    void accept(ASTVisitor& visit) const override;

  private:
    std::string m_command;
    std::vector<std::string> m_args;
    std::vector<Redirect> m_redirects;
};

class PipeExpression : public Expression
{
  public:
    PipeExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
    const Expression* get_left() const { return m_left.get(); };
    const Expression* get_right() const { return m_right.get(); }

    void accept(ASTVisitor& visit) const override;

  private:
    std::unique_ptr<Expression> m_left, m_right;
};

struct Program
{
    std::vector<std::unique_ptr<Expression>> m_statements;
};

class ProgramParser
{
  public:
    explicit ProgramParser(std::string source_code);
    Program build_ast();

  private:
    std::string m_source_code;
};

class ExpressionParser
{
  public:
    ExpressionParser(std::vector<Token>& tokens);

    bool is_redirect();
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Expression> parse_pipe_expression();
    std::unique_ptr<Expression> parse_command_expression();
    std::vector<std::string> parse_command_arguments();
    std::vector<Redirect> parse_redirects();

  private:
    std::vector<Token> m_tokens;
    size_t m_idx;
};
