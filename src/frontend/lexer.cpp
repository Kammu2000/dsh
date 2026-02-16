#include "frontend/lexer.hpp"
#include <cctype>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

void Token::print() const {
  std::cout << static_cast<int>(this->type) << " " << this->value << "\n";
}

TokenType Token::getType() { return this->type; }
std::string Token::getValue() { return this->value; }

Lexer::Lexer(std::string line) : line(std::move(line)) {}

bool Lexer::isWordChar(unsigned char character) {
  return !std::isspace(character) && character != '|' && character != '>' &&
         character != '<';
}

std::string Lexer::getWord(int &idx) {
  size_t len = line.length();
  std::string word;

  while (idx < len && isWordChar(line[idx])) {
    word += line[idx++];
  }

  return word;
}

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> tokens;

  int idx = 0;
  size_t len = line.length();

  while (idx < len) {
    if (std::isspace(line[idx])) {
      idx++;
      continue;
    }

    switch (line[idx]) {
    case '|': {
      tokens.emplace_back(TokenType::PIPE);
      idx++;
      break;
    }

    case '>': {
      if (idx + 1 < len && line[idx + 1] == '>') {
        tokens.emplace_back(TokenType::REDIRECT_APPEND);
        idx += 2;
      } else {
        tokens.emplace_back(TokenType::REDIRECT_OUT);
        idx++;
      }

      break;
    }

    case '<': {
      tokens.emplace_back(TokenType::REDIRECT_IN);
      idx++;
      break;
    }

    default: {

      if (isWordChar(line[idx])) {
        std::string word = getWord(idx);
        tokens.emplace_back(TokenType::WORD, word);
      } else {
        throw std::runtime_error("Unknown character was found at position" +
                                 std::to_string(idx));
      }

      break;
    }
    }
  }

  tokens.emplace_back(TokenType::END_OF_FILE);
  return tokens;
}
