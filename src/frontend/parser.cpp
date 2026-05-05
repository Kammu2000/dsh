#include "runtime/astVisitor.hpp"
#include <cstddef>
#include <frontend/lexer.hpp>
#include <frontend/parser.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

CommandExpression::CommandExpression(std::string command, std::vector<std::string> args,
                                     std::vector<Redirect> redirects)
    : m_command(std::move(command)), m_args(std::move(args)), m_redirects(std::move(redirects)) {};

PipeExpression::PipeExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
    : m_left(std::move(left)), m_right(std::move(right)) {};

ProgramParser::ProgramParser(std::string source_code) : m_source_code(std::move(source_code)) {}

Program ProgramParser::build_ast()
{
    Lexer lexer(m_source_code);
    std::vector<Token> tokens = lexer.tokenize();

    ExpressionParser exprParser(tokens);
    Program program;

    std::unique_ptr<Expression> temp = exprParser.parse_expression();
    program.m_statements.push_back(std::move(temp));

    return program;
}

ExpressionParser::ExpressionParser(std::vector<Token>& tokens) : m_tokens(tokens), m_idx(0) {};

bool ExpressionParser::is_redirect()
{
    switch (m_tokens[m_idx].m_type)
    {
        case TokenType::REDIRECT_IN:
        case TokenType::REDIRECT_OUT:
        case TokenType::REDIRECT_APPEND:
            return true;

        default:
            return false;
    }
}

std::unique_ptr<Expression> ExpressionParser::parse_expression()
{
    return parse_pipe_expression();
}

std::unique_ptr<Expression> ExpressionParser::parse_pipe_expression()
{
    size_t len = m_tokens.size();
    auto left = parse_command_expression();

    while (m_idx < len && m_tokens[m_idx].m_type == TokenType::PIPE)
    {
        m_idx++;
        auto right = parse_command_expression();
        auto pipe = std::make_unique<PipeExpression>(std::move(left), std::move(right));
        left = std::move(pipe);
    }

    return left;
}

std::unique_ptr<Expression> ExpressionParser::parse_command_expression()
{
    size_t len = m_tokens.size();

    if (m_idx < len && m_tokens[m_idx].m_type == TokenType::WORD)
    {
        auto command = m_tokens[m_idx++].m_value;
        auto args = parse_command_arguments();
        auto redirects = parse_redirects();

        return std::make_unique<CommandExpression>(std::move(command), std::move(args),
                                                   std::move(redirects));
    }

    throw std::runtime_error("Command expression could not be parsed");
}

std::vector<std::string> ExpressionParser::parse_command_arguments()
{
    size_t len = m_tokens.size();
    std::vector<std::string> args;

    while (m_idx < len && m_tokens[m_idx].m_type == TokenType::WORD)
    {
        args.push_back(m_tokens[m_idx++].m_value);
    }

    return args;
}

std::vector<Redirect> ExpressionParser::parse_redirects()
{
    std::vector<Redirect> redirects;
    size_t len = m_tokens.size();

    while (m_idx < len && is_redirect())
    {
        auto type = static_cast<RedirectType>(m_tokens[m_idx++].m_type);

        if (m_idx >= len || m_tokens[m_idx].m_type != TokenType::WORD)
        {
            throw std::runtime_error("Expected filename after redirect");
        }

        auto fileName = m_tokens[m_idx++].m_value;
        redirects.emplace_back(type, fileName);
    }

    return redirects;
}

void PipeExpression::accept(ASTVisitor& visitor) const
{
    visitor.visit(*this);
}

void CommandExpression::accept(ASTVisitor& visitor) const
{
    visitor.visit(*this);
};
