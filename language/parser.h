#pragma once

#include "ast.h"
#include "lexer.h"

#include <vector>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    ProgramNode parse();
    int currentLine() const;

private:
    const Token& current() const;
    const Token& peek(std::size_t offset) const;
    bool match(TokenKind kind);
    Token consume(TokenKind kind, const char* message);
    void consumeFieldSeparator(const char* message);
    void consumeIdentifier(const char* value);

    std::string consumeString(const char* message);
    int consumeNumber(const char* message);

    ElementNode parseElement(ElementType type);
    WindowNode parseWindowBlock();
    void parseScriptBlock(ProgramNode& program);
    void parseElementIntoProgram(ProgramNode& program);
    void parseCloseTrigger(ProgramNode& program);
    VariableDeclaration parseVariableDeclaration(
        const std::string& typeName
    );
    std::shared_ptr<Statement> parseStatement();
    std::vector<std::shared_ptr<Statement>> parseStatementBlock(
        const char* owner
    );
    ValueExpression parseValueExpression();
    Condition parseCondition(bool parenthesized = true);
    ComparisonOperator parseComparisonOperator();
    std::shared_ptr<Statement> parseIfStatement();
    std::shared_ptr<Statement> parseWhileStatement();
    std::shared_ptr<Statement> parseForStatement();
    std::shared_ptr<Statement> parseIncrementStatement();
    std::shared_ptr<Statement> parseVariableAssignment();
    std::shared_ptr<Statement> parseSwitchViewStatement();
    std::shared_ptr<Statement> parseTriggerStatement();
    UpdateAction parsePropertyAssignment();
    std::shared_ptr<Statement> parseUpdateStatement();
    UpdateBlock parseUpdateBlock(const std::string& targetId);
    UpdateAction parseObjectProperty(const std::string& targetId);
    std::variant<std::string, UpdateAction::TextTemplate> parseTextValue();
    void parseElementProperty(ElementNode& element);
    std::vector<std::shared_ptr<Statement>> parseClickAction();
    UpdateAction parseUpdateAction();

    std::vector<Token> tokens_;
    std::size_t position_ = 0;
};
