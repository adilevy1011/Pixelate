#include "parser.h"

#include <stdexcept>
#include <utility>

Parser::Parser(std::vector<Token> tokens)
    : tokens_(std::move(tokens)) {
}

const Token& Parser::current() const {
    return tokens_.at(position_);
}

bool Parser::match(TokenKind kind) {
    if (current().kind != kind) {
        return false;
    }

    ++position_;
    return true;
}

Token Parser::consume(TokenKind kind, const char* message) {
    if (current().kind != kind) {
        throw std::runtime_error(message);
    }

    return tokens_.at(position_++);
}

void Parser::consumeIdentifier(const char* value) {
    Token token = consume(TokenKind::Identifier, "Expected identifier.");

    if (token.text != value) {
        throw std::runtime_error("Unexpected identifier.");
    }
}

std::string Parser::consumeString(const char* message) {
    return consume(TokenKind::String, message).text;
}

int Parser::consumeNumber(const char* message) {
    Token token = consume(TokenKind::Number, message);

    try {
        return std::stoi(token.text);
    } catch (...) {
        throw std::runtime_error("Invalid number.");
    }
}

ProgramNode Parser::parse() {
    ProgramNode program;

    consumeIdentifier("window");
    consume(TokenKind::OpenBrace, "Expected '{' after window.");

    while (!match(TokenKind::CloseBrace)) {
        Token token = consume(TokenKind::Identifier, "Expected window property or element.");

        if (token.text == "title") {
            consume(TokenKind::Colon, "Expected ':' after title.");
            program.title = consumeString("Expected window title.");
        } else if (token.text == "width") {
            consume(TokenKind::Colon, "Expected ':' after width.");
            program.width = consumeNumber("Expected window width.");
        } else if (token.text == "height") {
            consume(TokenKind::Colon, "Expected ':' after height.");
            program.height = consumeNumber("Expected window height.");
        } else if (token.text == "label" || token.text == "button") {
            Token id = consume(TokenKind::Identifier, "Expected element id.");
            ElementType type = token.text == "label"
                ? ElementType::Label
                : ElementType::Button;

            ElementNode element = parseElement(type);
            element.id = id.text;
            program.elements.push_back(std::move(element));
        } else {
            throw std::runtime_error("Unknown window property or element.");
        }
    }

    consume(TokenKind::End, "Unexpected content after window.");
    return program;
}

ElementNode Parser::parseElement(ElementType type) {
    ElementNode element{};
    element.type = type;

    consume(TokenKind::OpenBrace, "Expected '{' after element id.");

    while (!match(TokenKind::CloseBrace)) {
        parseElementProperty(element);
    }

    return element;
}

void Parser::parseElementProperty(ElementNode& element) {
    Token property = consume(TokenKind::Identifier, "Expected element property.");

    if (property.text == "text") {
        consume(TokenKind::Colon, "Expected ':' after text.");
        element.text = consumeString("Expected element text.");
    } else if (property.text == "x") {
        consume(TokenKind::Colon, "Expected ':' after x.");
        element.x = consumeNumber("Expected x position.");
    } else if (property.text == "y") {
        consume(TokenKind::Colon, "Expected ':' after y.");
        element.y = consumeNumber("Expected y position.");
    } else if (property.text == "width") {
        consume(TokenKind::Colon, "Expected ':' after width.");
        element.width = consumeNumber("Expected element width.");
    } else if (property.text == "height") {
        consume(TokenKind::Colon, "Expected ':' after height.");
        element.height = consumeNumber("Expected element height.");
    } else if (property.text == "on") {
        element.onClick = parseClickAction();
    } else {
        throw std::runtime_error("Unknown element property.");
    }
}

SetTextAction Parser::parseClickAction() {
    consumeIdentifier("click");
    consume(TokenKind::OpenBrace, "Expected '{' after on click.");
    consumeIdentifier("set_text");

    SetTextAction action;
    action.targetId = consume(
        TokenKind::Identifier,
        "Expected target element id."
    ).text;
    action.text = consumeString("Expected replacement text.");

    consume(TokenKind::CloseBrace, "Expected '}' after click action.");
    return action;
}
