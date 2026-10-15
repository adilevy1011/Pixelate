#pragma once

#include "ast.h"
#include "lexer.h"

#include <vector>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    ProgramNode parse();

private:
    const Token& current() const;
    bool match(TokenKind kind);
    Token consume(TokenKind kind, const char* message);
    void consumeIdentifier(const char* value);

    std::string consumeString(const char* message);
    int consumeNumber(const char* message);

    ElementNode parseElement(ElementType type);
    void parseElementProperty(ElementNode& element);
    SetTextAction parseClickAction();

    std::vector<Token> tokens_;
    std::size_t position_ = 0;
};
