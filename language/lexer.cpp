#include "lexer.h"

#include <cctype>
#include <stdexcept>
#include <utility>

Lexer::Lexer(std::string source)
    : source_(std::move(source)) {
}

std::vector<Token> Lexer::tokenize() const {
    std::vector<Token> tokens;
    std::size_t index = 0;
    int line = 1;
    int column = 1;

    auto advance = [&] {
        if (source_[index] == '\n') {
            ++line;
            column = 1;
        } else {
            ++column;
        }
        ++index;
    };

    while (index < source_.size()) {
        char current = source_[index];

        if (std::isspace(static_cast<unsigned char>(current))) {
            advance();
            continue;
        }

        if (current == '#') {
            while (index < source_.size() && source_[index] != '\n') {
                advance();
            }
            continue;
        }

        int tokenLine = line;
        int tokenColumn = column;

        if (std::isalpha(static_cast<unsigned char>(current)) || current == '_') {
            std::string value;

            while (index < source_.size()) {
                char character = source_[index];

                if (!std::isalnum(static_cast<unsigned char>(character)) &&
                    character != '_') {
                    break;
                }

                value.push_back(character);
                advance();
            }

            tokens.push_back({
                TokenKind::Identifier,
                value,
                tokenLine,
                tokenColumn
            });
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(current)) || current == '-') {
            std::string value;

            if (current == '-') {
                value.push_back(current);
                advance();
            }

            while (index < source_.size() &&
                   std::isdigit(static_cast<unsigned char>(source_[index]))) {
                value.push_back(source_[index]);
                advance();
            }

            tokens.push_back({
                TokenKind::Number,
                value,
                tokenLine,
                tokenColumn
            });
            continue;
        }

        if (current == '"') {
            std::string value;
            advance();

            while (index < source_.size() && source_[index] != '"') {
                if (source_[index] == '\\') {
                    advance();

                    if (index >= source_.size()) {
                        throw std::runtime_error("Unterminated string.");
                    }

                    switch (source_[index]) {
                        case 'n': value.push_back('\n'); break;
                        case 't': value.push_back('\t'); break;
                        case '"': value.push_back('"'); break;
                        case '\\': value.push_back('\\'); break;
                        default:
                            throw std::runtime_error("Unknown string escape.");
                    }

                    advance();
                } else {
                    value.push_back(source_[index]);
                    advance();
                }
            }

            if (index >= source_.size()) {
                throw std::runtime_error("Unterminated string.");
            }

            advance();
            tokens.push_back({
                TokenKind::String,
                value,
                tokenLine,
                tokenColumn
            });
            continue;
        }

        TokenKind kind;

        switch (current) {
            case ':': kind = TokenKind::Colon; break;
            case '{': kind = TokenKind::OpenBrace; break;
            case '}': kind = TokenKind::CloseBrace; break;
            default:
                throw std::runtime_error("Unexpected character in source.");
        }

        advance();
        tokens.push_back({kind, std::string(1, current), tokenLine, tokenColumn});
    }

    tokens.push_back({TokenKind::End, {}, line, column});
    return tokens;
}
