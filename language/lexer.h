#pragma once

#include <string>
#include <vector>

enum class TokenKind {
    Identifier,
    String,
    Character,
    Number,
    Colon,
    Dot,
    Equals,
    EqualEqual,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Increment,
    OpenParen,
    CloseParen,
    Semicolon,
    OpenBrace,
    CloseBrace,
    End
};

struct Token {
    TokenKind kind;
    std::string text;
    int line;
    int column;
};

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize() const;

private:
    std::string source_;
};
