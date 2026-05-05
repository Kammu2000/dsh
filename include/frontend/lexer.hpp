#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

enum class TokenType : uint8_t
{
    // we cannot have reserved keywords in shell like ls, cd as they can be used
    // as an arguement also so we need to keep a generic token for words
    REDIRECT_IN,
    REDIRECT_OUT,
    REDIRECT_APPEND,
    PIPE,
    AND,
    WORD,
    END_OF_FILE,
};

struct Token
{
    TokenType m_type;
    std::string m_value;

    Token(TokenType type, std::string value = "") : m_type(type), m_value(std::move(value)) {}
};

class Lexer
{
  public:
    explicit Lexer(std::string code);
    std::vector<Token> tokenize();

  private:
    std::string m_code;
    size_t m_idx;
    static bool is_identifier_char(unsigned char character);
    std::string read_identifier();
};
