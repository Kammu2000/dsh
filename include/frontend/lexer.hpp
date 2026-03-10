#pragma once
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

enum class TokenType : uint8_t {
  // we cannot have reserved keywords in shell like ls, cd as they can be used
  // as an arguement also so we need to keep a generic token for words
  REDIRECT_IN,
  REDIRECT_OUT,
  REDIRECT_APPEND,
  WORD,
  PIPE,
  END_OF_FILE,
};

class Token {
private:
  TokenType type;
  std::string value;

public:
  Token(TokenType type, std::string value = "")
      : type(type), value(std::move(value)) {}
  void print() const;
  TokenType getType();
  std::string getValue();
};

class Lexer {
private:
  std::string line;
  static bool isWordChar(unsigned char character);
  std::string getWord(int &idx);

public:
  explicit Lexer(std::string line);
  std::vector<Token> tokenize();
};
