#include "frontend/lexer.hpp"
#include "common/errors.hpp"
#include <cctype>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

Lexer::Lexer(std::string code) : m_code(std::move(code)), m_idx(0) {}

bool Lexer::is_identifier_char(unsigned char character)
{
    return (bool)std::isalnum(character) || character == '_' || character == '-' ||
           character == '.' || character == '/';
}

std::string Lexer::read_identifier()
{
    size_t len = m_code.length();
    std::string word;

    while (m_idx < len && is_identifier_char(m_code[m_idx]))
    {
        word += m_code[m_idx++];
    }

    return word;
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    size_t len = m_code.length();

    while (m_idx < len)
    {
        if ((bool)std::isspace(m_code[m_idx]))
        {
            m_idx++;
            continue;
        }

        switch (m_code[m_idx])
        {
            case '|': {
                tokens.emplace_back(TokenType::PIPE);
                m_idx++;
                break;
            }

            case '>': {
                if (m_idx + 1 < len && m_code[m_idx + 1] == '>')
                {
                    tokens.emplace_back(TokenType::REDIRECT_APPEND);
                    m_idx += 2;
                }
                else
                {
                    tokens.emplace_back(TokenType::REDIRECT_OUT);
                    m_idx++;
                }

                break;
            }

            case '<': {
                tokens.emplace_back(TokenType::REDIRECT_IN);
                m_idx++;
                break;
            }

            default: {
                if (is_identifier_char(m_code[m_idx]))
                {
                    std::string word = read_identifier();
                    tokens.emplace_back(TokenType::WORD, word);
                }
                else
                {
                    throw LexerError("Found unknown character at position: " +
                                     std::to_string(m_idx));
                }

                break;
            }
        }
    }

    tokens.emplace_back(TokenType::END_OF_FILE);
    return tokens;
}
