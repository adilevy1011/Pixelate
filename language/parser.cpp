#include "parser.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace {

bool isFontConstant(const std::string& value) {
    static const std::unordered_set<std::string> fonts = {
        "DEFAULT", "ARIAL", "CALIBRI", "CONSOLAS", "COURIERNEW",
        "TIMESNEWROMAN", "TAHOMA", "VERDANA"
    };
    return fonts.find(value) != fonts.end();
}

}

Parser::Parser(std::vector<Token> tokens)
    : tokens_(std::move(tokens)) {
}

const Token& Parser::current() const {
    return tokens_.at(position_);
}

int Parser::currentLine() const {
    return current().line;
}

const Token& Parser::peek(std::size_t offset) const {
    return tokens_.at(position_ + offset);
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

void Parser::consumeFieldSeparator(const char* message) {
    // Fields are comma-delimited, but the final field may be followed
    // directly by the closing brace (like a Python dictionary literal).
    if (current().kind == TokenKind::CloseBrace) {
        return;
    }

    consume(TokenKind::Comma, message);
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

    while (current().kind != TokenKind::End) {
        Token block = consume(
            TokenKind::Identifier,
            "Expected window or script block."
        );

        if (block.text == "window") {
            program.windows.push_back(parseWindowBlock());
        } else if (block.text == "script") {
            parseScriptBlock(program);
        } else {
            throw std::runtime_error("Unknown top-level block.");
        }
    }

    consume(TokenKind::End, "Unexpected content after window.");

    if (program.windows.empty()) {
        throw std::runtime_error("A window block is required.");
    }

    if (program.windows.size() > 1) {
        std::unordered_set<std::string> ids;
        for (const WindowNode& window : program.windows) {
            if (window.id.empty()) {
                throw std::runtime_error(
                    "Every window needs an id when multiple windows are defined."
                );
            }

            if (!ids.insert(window.id).second) {
                throw std::runtime_error(
                    "Window ids must be unique: " + window.id
                );
            }
        }
    }

    // Elements declared in a script belong to the application's window.  Keep
    // them when views are built from WindowNode, instead of letting the
    // single-window compatibility copy below overwrite them.
    if (!program.elements.empty()) {
        auto target = program.windows.begin();

        if (program.windows.size() > 1) {
            const auto mainWindow = std::find_if(
                program.windows.begin(),
                program.windows.end(),
                [](const WindowNode& window) {
                    return window.id == "main";
                }
            );

            if (mainWindow != program.windows.end()) {
                target = mainWindow;
            }
        }

        target->elements.insert(
            target->elements.end(),
            program.elements.begin(),
            program.elements.end()
        );
    }

    if (program.windows.size() == 1) {
        const WindowNode& window = program.windows.front();
        program.title = window.title;
        program.width = window.width;
        program.height = window.height;
        program.iconPath = window.iconPath;
        program.background = window.background;
        program.closeTrigger = window.closeTrigger;
        program.elements = window.elements;
    }

    return program;
}

WindowNode Parser::parseWindowBlock() {
    WindowNode window;
    consume(TokenKind::OpenBrace, "Expected '{' after window.");

    while (!match(TokenKind::CloseBrace)) {
        Token token = consume(
            TokenKind::Identifier,
            "Expected window property or element."
        );

        if (token.text == "id") {
            consume(TokenKind::Colon, "Expected ':' after id.");
            window.id = consumeString("Expected window id.");
            consumeFieldSeparator("Expected ',' after id.");
        } else if (token.text == "title") {
            consume(TokenKind::Colon, "Expected ':' after title.");
            window.title = consumeString("Expected window title.");
            consumeFieldSeparator("Expected ',' after title.");
        } else if (token.text == "width") {
            consume(TokenKind::Colon, "Expected ':' after width.");
            window.width = consumeNumber("Expected window width.");
            consumeFieldSeparator("Expected ',' after width.");
        } else if (token.text == "height") {
            consume(TokenKind::Colon, "Expected ':' after height.");
            window.height = consumeNumber("Expected window height.");
            consumeFieldSeparator("Expected ',' after height.");
        } else if (token.text == "iconPath") {
            consume(TokenKind::Colon, "Expected ':' after iconPath.");
            window.iconPath = consumeString("Expected icon path.");
            consumeFieldSeparator("Expected ',' after iconPath.");
        } else if (token.text == "background") {
            consume(TokenKind::Colon, "Expected ':' after background.");
            window.background.red = consumeNumber("Expected red channel.");
            window.background.green = consumeNumber("Expected green channel.");
            window.background.blue = consumeNumber("Expected blue channel.");

            if (window.background.red < 0 || window.background.red > 255 ||
                window.background.green < 0 || window.background.green > 255 ||
                window.background.blue < 0 || window.background.blue > 255) {
                throw std::runtime_error(
                    "Background color channels must be between 0 and 255."
                );
            }
            consumeFieldSeparator("Expected ',' after background.");
        } else if (token.text == "label" || token.text == "par" || token.text == "button" ||
                   token.text == "textbox" || token.text == "textarea" || token.text == "rect" ||
                   token.text == "circle" || token.text == "triangle") {
            Token id = consume(TokenKind::Identifier, "Expected element id.");
            ElementType type = token.text == "label"
                ? ElementType::Label
                : token.text == "par"
                    ? ElementType::Paragraph
                : token.text == "button"
                    ? ElementType::Button
                    : token.text == "textbox"
                        ? ElementType::TextBox
                        : token.text == "textarea"
                            ? ElementType::TextArea
                            : token.text == "rect"
                                ? ElementType::Rect
                                : token.text == "circle"
                                    ? ElementType::Circle
                                    : ElementType::Triangle;

            ElementNode element = parseElement(type);
            element.id = id.text;
            window.elements.push_back(std::move(element));
        } else {
            throw std::runtime_error("Unknown window property or element.");
        }
    }

    return window;
}

void Parser::parseScriptBlock(ProgramNode& program) {
    consume(TokenKind::OpenBrace, "Expected '{' after script.");

    while (!match(TokenKind::CloseBrace)) {
        Token command = consume(TokenKind::Identifier,
                                "Expected script command or statement.");

        if (command.text == "add") {
            Token type = consume(
                TokenKind::Identifier,
                "Expected label, par, button, textbox, textarea, rect, circle, triangle, or CLOSE_TRIGGER after add."
            );

            if (type.text == "CLOSE_TRIGGER") {
                parseCloseTrigger(program);
            } else if (type.text == "label" || type.text == "par" || type.text == "button" ||
                       type.text == "textbox" || type.text == "textarea" || type.text == "rect" ||
                       type.text == "circle" || type.text == "triangle") {
                ElementType elementType = type.text == "label"
                    ? ElementType::Label
                    : type.text == "par"
                        ? ElementType::Paragraph
                    : type.text == "button"
                        ? ElementType::Button
                        : type.text == "textbox"
                            ? ElementType::TextBox
                            : type.text == "textarea"
                                ? ElementType::TextArea
                                : type.text == "rect"
                                    ? ElementType::Rect
                                    : type.text == "circle"
                                        ? ElementType::Circle
                                        : ElementType::Triangle;
                Token id = consume(TokenKind::Identifier, "Expected element id.");
                ElementNode element = parseElement(elementType);
                element.id = id.text;
                program.elements.push_back(std::move(element));
            } else {
                throw std::runtime_error(
                    "Only label, par, button, textbox, textarea, rect, circle, triangle, and CLOSE_TRIGGER can be added."
                );
            }
        } else if (
            command.text == "int" ||
            command.text == "short" ||
            command.text == "long" ||
            command.text == "float" ||
            command.text == "double" ||
            command.text == "bool" ||
            command.text == "char" ||
            command.text == "String"
        ) {
            VariableDeclaration declaration = parseVariableDeclaration(command.text);
            program.variables.push_back(declaration);
            auto statement = std::make_shared<Statement>();
            statement->value = declaration;
            program.statements.push_back(std::move(statement));
        } else if (command.text == "if" || command.text == "while" ||
                   command.text == "for" || command.text == "update" ||
                   command.text == "switch_view") {
            --position_;
            program.statements.push_back(parseStatement());
        } else if (command.text == "trigger") {
            --position_;
            auto statement = parseTriggerStatement();
            program.triggers.push_back(
                std::get<std::shared_ptr<TriggerStatement>>(statement->value)
            );
        } else if (peek(0).kind == TokenKind::Dot ||
                   peek(0).kind == TokenKind::Increment ||
                   peek(0).kind == TokenKind::Equals) {
            --position_;
            program.statements.push_back(parseStatement());
        } else {
            throw std::runtime_error("Unknown script command or type.");
        }
    }
}

VariableDeclaration Parser::parseVariableDeclaration(const std::string& typeName) {
    VariableType type;

    if (typeName == "int") type = VariableType::Int;
    else if (typeName == "short") type = VariableType::Short;
    else if (typeName == "long") type = VariableType::Long;
    else if (typeName == "float") type = VariableType::Float;
    else if (typeName == "double") type = VariableType::Double;
    else if (typeName == "bool") type = VariableType::Bool;
    else if (typeName == "char") type = VariableType::Char;
    else if (typeName == "String") type = VariableType::String;
    else throw std::runtime_error("Unknown variable type.");

    std::string name = consume(
        TokenKind::Identifier,
        "Expected variable name."
    ).text;

    consume(TokenKind::Equals, "Expected '=' after variable name.");

    VariableValue value;

    try {
        if (type == VariableType::String) {
            value = consumeString("Expected string value.");
        } else if (type == VariableType::Char) {
            value = consume(TokenKind::Character, "Expected character value.").text[0];
        } else if (type == VariableType::Bool) {
            std::string boolean = consume(
                TokenKind::Identifier,
                "Expected true or false."
            ).text;

            if (boolean == "true") value = true;
            else if (boolean == "false") value = false;
            else throw std::runtime_error("Expected true or false.");
        } else {
            std::string number = consume(
                TokenKind::Number,
                "Expected numeric value."
            ).text;

            bool decimal = number.find('.') != std::string::npos;

            if ((type == VariableType::Int ||
                 type == VariableType::Short ||
                 type == VariableType::Long) && decimal) {
                throw std::runtime_error(
                    "Integer variables cannot use decimal values."
                );
            }

            if (type == VariableType::Int) value = std::stoi(number);
            else if (type == VariableType::Short) {
                value = static_cast<short>(std::stoi(number));
            } else if (type == VariableType::Long) {
                value = std::stoll(number);
            } else if (type == VariableType::Float) {
                value = static_cast<float>(std::stod(number));
            } else if (type == VariableType::Double) {
                value = std::stod(number);
            }
        }
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid variable value.");
    } catch (const std::out_of_range&) {
        throw std::runtime_error("Variable value is out of range.");
    }

    VariableDeclaration declaration{type, name, value};
    consume(TokenKind::Semicolon, "Expected ';' after variable declaration.");
    return declaration;
}

void Parser::parseCloseTrigger(ProgramNode& program) {
    CloseTriggerNode trigger;
    consume(TokenKind::OpenBrace, "Expected '{' after CLOSE_TRIGGER.");

    while (!match(TokenKind::CloseBrace)) {
        Token property = consume(
            TokenKind::Identifier,
            "Expected CLOSE_TRIGGER property."
        );

        if (property.text == "message") {
            consume(TokenKind::Colon, "Expected ':' after message.");
            trigger.message = consumeString("Expected close message.");
            consumeFieldSeparator("Expected ',' after message.");
        } else if (property.text == "options") {
            consume(TokenKind::Colon, "Expected ':' after options.");
            trigger.option = consume(
                TokenKind::Identifier,
                "Expected close dialog option."
            ).text;

            if (trigger.option != "YESNO") {
                throw std::runtime_error(
                    "The only supported CLOSE_TRIGGER option is YESNO."
                );
            }
            consumeFieldSeparator("Expected ',' after options.");
        } else {
            throw std::runtime_error("Unknown CLOSE_TRIGGER property.");
        }
    }

    program.closeTrigger = trigger;
}

void Parser::parseElementIntoProgram(ProgramNode& program) {
    Token typeToken = consume(
        TokenKind::Identifier,
        "Expected label, par, button, textbox, or textarea after add."
    );

    if (typeToken.text != "label" && typeToken.text != "par" &&
        typeToken.text != "button" &&
        typeToken.text != "textbox" && typeToken.text != "textarea") {
        throw std::runtime_error(
            "Only label, par, button, textbox, and textarea can be added."
        );
    }

    Token id = consume(TokenKind::Identifier, "Expected element id.");
    ElementType type = typeToken.text == "label"
        ? ElementType::Label
        : typeToken.text == "par"
            ? ElementType::Paragraph
        : typeToken.text == "button"
            ? ElementType::Button
            : typeToken.text == "textbox"
                ? ElementType::TextBox
                : ElementType::TextArea;

    ElementNode element = parseElement(type);
    element.id = id.text;
    program.elements.push_back(std::move(element));
}

ElementNode Parser::parseElement(ElementType type) {
    ElementNode element{};
    element.type = type;

    consume(TokenKind::OpenBrace, "Expected '{' after element id.");

    while (!match(TokenKind::CloseBrace)) {
        parseElementProperty(element);
    }

    if (type == ElementType::Triangle) {
        const bool anyPoint = element.p1Specified || element.p2Specified || element.p3Specified;
        const bool allPoints = element.p1Specified && element.p2Specified && element.p3Specified;
        if (anyPoint && !allPoints) {
            throw std::runtime_error("Triangle elements must specify p1, p2, and p3 together.");
        }
        if (element.xSpecified != element.ySpecified) {
            throw std::runtime_error("Triangle elements must specify both x and y together.");
        }
    }

    return element;
}

void Parser::parseElementProperty(ElementNode& element) {
    Token property = consume(TokenKind::Identifier, "Expected element property.");

    if (property.text == "text") {
        consume(TokenKind::Colon, "Expected ':' after text.");
        element.text = parseTextValue();
        consumeFieldSeparator("Expected ',' after text.");
    } else if (property.text == "placeholder") {
        if (element.type != ElementType::TextBox &&
            element.type != ElementType::TextArea) {
            throw std::runtime_error(
                "Only textbox and textarea elements can use placeholder."
            );
        }
        consume(TokenKind::Colon, "Expected ':' after placeholder.");
        element.placeholder = consumeString("Expected placeholder text.");
        consumeFieldSeparator("Expected ',' after placeholder.");
    } else if (property.text == "x") {
        consume(TokenKind::Colon, "Expected ':' after x.");
        element.x = consumeNumber("Expected x position.");
        element.xSpecified = true;
        consumeFieldSeparator("Expected ',' after x.");
    } else if (property.text == "y") {
        consume(TokenKind::Colon, "Expected ':' after y.");
        element.y = consumeNumber("Expected y position.");
        element.ySpecified = true;
        consumeFieldSeparator("Expected ',' after y.");
    } else if (property.text == "width") {
        consume(TokenKind::Colon, "Expected ':' after width.");
        element.width = consumeNumber("Expected element width.");
        consumeFieldSeparator("Expected ',' after width.");
    } else if (property.text == "height") {
        consume(TokenKind::Colon, "Expected ':' after height.");
        element.height = consumeNumber("Expected element height.");
        consumeFieldSeparator("Expected ',' after height.");
    } else if (property.text == "angle") {
        if (element.type != ElementType::Rect && element.type != ElementType::Triangle) {
            throw std::runtime_error("Only rect and triangle elements can use angle.");
        }
        consume(TokenKind::Colon, "Expected ':' after angle.");
        element.angle = consumeNumber("Expected rectangle angle.");
        consumeFieldSeparator("Expected ',' after angle.");
    } else if (property.text == "radius") {
        if (element.type != ElementType::Circle) throw std::runtime_error("Only circle elements can use radius.");
        consume(TokenKind::Colon, "Expected ':' after radius.");
        element.radius = consumeNumber("Expected circle radius.");
        if (element.radius <= 0) throw std::runtime_error("Circle radius must be greater than zero.");
        consumeFieldSeparator("Expected ',' after radius.");
    } else if (property.text == "p1" || property.text == "p2" || property.text == "p3") {
        if (element.type != ElementType::Triangle) throw std::runtime_error("Only triangle elements can use points.");
        consume(TokenKind::Colon, "Expected ':' after triangle point.");
        consume(TokenKind::OpenParen, "Expected '(' before triangle point.");
        PointNode point;
        point.x = consumeNumber("Expected triangle point x.");
        consume(TokenKind::Comma, "Expected ',' between triangle point coordinates.");
        point.y = consumeNumber("Expected triangle point y.");
        consume(TokenKind::CloseParen, "Expected ')' after triangle point.");
        if (property.text == "p1") { element.p1 = point; element.p1Specified = true; }
        if (property.text == "p2") { element.p2 = point; element.p2Specified = true; }
        if (property.text == "p3") { element.p3 = point; element.p3Specified = true; }
        consumeFieldSeparator("Expected ',' after triangle point.");
    } else if (property.text == "font") {
        consume(TokenKind::Colon, "Expected ':' after font.");
        element.font = consume(TokenKind::Identifier, "Expected font constant.").text;
        if (!isFontConstant(element.font)) {
            throw std::runtime_error("Unknown font constant.");
        }
        consumeFieldSeparator("Expected ',' after font.");
    } else if (property.text == "fontSize") {
        consume(TokenKind::Colon, "Expected ':' after fontSize.");
        element.fontSize = consumeNumber("Expected font size.");
        if (element.fontSize <= 0) {
            throw std::runtime_error("Font size must be greater than zero.");
        }
        consumeFieldSeparator("Expected ',' after fontSize.");
    } else if (property.text == "onclick") {
        element.onClick = parseClickAction();
        consumeFieldSeparator("Expected ',' after onclick.");
    } else {
        throw std::runtime_error("Unknown element property.");
    }
}

std::vector<std::shared_ptr<Statement>> Parser::parseClickAction() {
    consume(TokenKind::OpenBrace, "Expected '{' after onclick.");

    std::vector<std::shared_ptr<Statement>> actions;

    while (!match(TokenKind::CloseBrace)) {
        actions.push_back(parseStatement());
    }

    return actions;
}

UpdateAction Parser::parseUpdateAction() {
    consumeIdentifier("update");

    return parsePropertyAssignment();
}

std::shared_ptr<Statement> Parser::parseUpdateStatement() {
    consumeIdentifier("update");
    const std::string targetId = consume(
        TokenKind::Identifier,
        "Expected object id after update."
    ).text;

    auto statement = std::make_shared<Statement>();

    if (current().kind == TokenKind::OpenBrace) {
        UpdateBlock block;
        block.targetId = targetId;
        block.actions = parseUpdateBlock(targetId).actions;
        statement->value = std::move(block);
        return statement;
    }

    --position_;
    statement->value = parsePropertyAssignment();
    return statement;
}

UpdateBlock Parser::parseUpdateBlock(const std::string& targetId) {
    UpdateBlock block;
    block.targetId = targetId;
    consume(TokenKind::OpenBrace, "Expected '{' after update target.");

    while (!match(TokenKind::CloseBrace)) {
        block.actions.push_back(parseObjectProperty(block.targetId));
    }

    return block;
}

UpdateAction Parser::parseObjectProperty(const std::string& targetId) {
    UpdateAction action;
    action.targetId = targetId;
    action.property = consume(
        TokenKind::Identifier,
        "Expected property name in update block."
    ).text;
    consume(TokenKind::Colon, "Expected ':' after update property.");

    if (action.property == "background") {
        ColorNode color;
        color.red = consumeNumber("Expected red channel.");
        color.green = consumeNumber("Expected green channel.");
        color.blue = consumeNumber("Expected blue channel.");
        action.value = color;
    } else if (action.property == "x" || action.property == "y" ||
               action.property == "width" || action.property == "height" ||
               action.property == "angle" || action.property == "radius") {
        action.value = parseValueExpression();
    } else if (action.property == "fontSize") {
        action.value = parseValueExpression();
    } else if (action.property == "font") {
        action.value = consume(
            TokenKind::Identifier,
            "Expected font constant."
        ).text;
        if (!isFontConstant(std::get<std::string>(action.value))) {
            throw std::runtime_error("Unknown font constant.");
        }
    } else if (targetId == "mouse" && action.property == "shape") {
        action.value = consume(
            TokenKind::Identifier,
            "Expected mouse shape."
        ).text;
    } else {
        auto text = parseTextValue();
        if (std::holds_alternative<UpdateAction::TextTemplate>(text)) {
            action.value = std::get<UpdateAction::TextTemplate>(text);
        } else {
            action.value = std::get<std::string>(text);
        }
    }

    consumeFieldSeparator("Expected ',' after update property.");

    return action;
}

UpdateAction Parser::parsePropertyAssignment() {

    UpdateAction action;
    action.targetId = consume(
        TokenKind::Identifier,
        "Expected target element id."
    ).text;

    if (match(TokenKind::Dot)) {
        action.property = consume(
            TokenKind::Identifier,
            "Expected property name after '.'."
        ).text;
    }

    consume(TokenKind::Equals, "Expected '=' in update action.");

    if (action.targetId == "mouse" && action.property == "shape") {
        const std::string shape = consume(
            TokenKind::Identifier,
            "Expected mouse shape."
        ).text;

        static const std::vector<std::string> validShapes = {
            "ARROW", "IBEAM", "WAIT", "CROSS", "UPARROW", "SIZE", "ICON",
            "SIZENWSE", "SIZENESW", "SIZEWE", "SIZENS", "SIZEALL",
            "NO", "HAND", "APPSTARTING", "HELP", "PIN", "PERSON"
        };

        if (std::find(validShapes.begin(), validShapes.end(), shape) ==
            validShapes.end()) {
            throw std::runtime_error("Unknown mouse shape.");
        }

        action.value = shape;
    } else if (action.property == "background") {
        ColorNode color;
        color.red = consumeNumber("Expected red channel.");
        color.green = consumeNumber("Expected green channel.");
        color.blue = consumeNumber("Expected blue channel.");

        if (color.red < 0 || color.red > 255 ||
            color.green < 0 || color.green > 255 ||
            color.blue < 0 || color.blue > 255) {
            throw std::runtime_error(
                "Background color channels must be between 0 and 255."
            );
        }

        action.value = color;
    } else if (action.property == "x" || action.property == "y" ||
               action.property == "width" || action.property == "height" ||
               action.property == "angle" || action.property == "radius") {
        action.value = parseValueExpression();
    } else if (action.property == "fontSize") {
        action.value = parseValueExpression();
    } else if (action.property == "font") {
        action.value = consume(
            TokenKind::Identifier,
            "Expected font constant."
        ).text;
        if (!isFontConstant(std::get<std::string>(action.value))) {
            throw std::runtime_error("Unknown font constant.");
        }
    } else {
        auto text = parseTextValue();
        if (std::holds_alternative<UpdateAction::TextTemplate>(text)) {
            action.value = std::get<UpdateAction::TextTemplate>(text);
        } else {
            action.value = std::get<std::string>(text);
        }
    }

    consume(TokenKind::Semicolon, "Expected ';' after assignment.");

    return action;
}

std::variant<std::string, UpdateAction::TextTemplate> Parser::parseTextValue() {
    if (current().kind == TokenKind::Identifier && current().text == "f") {
        consume(TokenKind::Identifier, "Expected formatted string prefix.");
        return UpdateAction::TextTemplate{
            consumeString("Expected formatted string.")
        };
    }

    return consumeString("Expected replacement text.");
}

std::shared_ptr<Statement> Parser::parseStatement() {
    Token command = consume(TokenKind::Identifier, "Expected statement.");
    const int line = command.line;
    auto stamp = [line](std::shared_ptr<Statement> statement) {
        statement->line = line;
        return statement;
    };

    if (command.text == "update") {
        --position_;
        return stamp(parseUpdateStatement());
    }

    if (command.text == "switch_view") {
        --position_;
        return stamp(parseSwitchViewStatement());
    }

    if (command.text == "trigger") {
        --position_;
        return stamp(parseTriggerStatement());
    }

    if (command.text == "if") {
        --position_;
        return stamp(parseIfStatement());
    }

    if (command.text == "while") {
        --position_;
        return stamp(parseWhileStatement());
    }

    if (command.text == "for") {
        --position_;
        return stamp(parseForStatement());
    }

    if (command.text == "int" || command.text == "short" ||
        command.text == "long" || command.text == "float" ||
        command.text == "double" || command.text == "bool" ||
        command.text == "char" || command.text == "String") {
        auto statement = std::make_shared<Statement>();
        statement->value = parseVariableDeclaration(command.text);
        return stamp(statement);
    }

    if (current().kind == TokenKind::Increment ||
        current().kind == TokenKind::Decrement) {
        --position_;
        return stamp(parseIncrementStatement());
    }

    if (current().kind == TokenKind::Equals) {
        --position_;
        return stamp(parseVariableAssignment());
    }

    if (current().kind == TokenKind::Dot) {
        if (peek(2).kind == TokenKind::Increment ||
            peek(2).kind == TokenKind::Decrement) {
            --position_;
            return stamp(parseIncrementStatement());
        }
        --position_;
        auto statement = std::make_shared<Statement>();
        statement->value = parsePropertyAssignment();
        return stamp(statement);
    }

    throw std::runtime_error("Unknown statement.");
}

std::shared_ptr<Statement> Parser::parseSwitchViewStatement() {
    consumeIdentifier("switch_view");
    consume(TokenKind::OpenParen, "Expected '(' after switch_view.");

    std::string windowId;
    if (current().kind == TokenKind::String) {
        windowId = consumeString("Expected window id.");
    } else {
        windowId = consume(
            TokenKind::Identifier,
            "Expected window id string."
        ).text;
    }

    bool destroyWindow = true;
    if (match(TokenKind::Comma)) {
        Token option = consume(
            TokenKind::Identifier,
            "Expected destroy_window option."
        );

        if (option.text != "destroy_window") {
            throw std::runtime_error("Expected destroy_window option.");
        }

        consume(TokenKind::Equals, "Expected '=' after destroy_window.");
        Token value = consume(TokenKind::Identifier, "Expected true or false.");

        if (value.text == "true") destroyWindow = true;
        else if (value.text == "false") destroyWindow = false;
        else throw std::runtime_error("Expected true or false.");
    }

    consume(TokenKind::CloseParen, "Expected ')' after switch_view.");
    consume(TokenKind::Semicolon, "Expected ';' after switch_view.");

    auto statement = std::make_shared<Statement>();
    statement->value = SwitchViewStatement{windowId, destroyWindow};
    return statement;
}

std::vector<std::shared_ptr<Statement>> Parser::parseStatementBlock(
    const char* owner
) {
    consume(TokenKind::OpenBrace, owner);
    std::vector<std::shared_ptr<Statement>> statements;

    while (!match(TokenKind::CloseBrace)) {
        statements.push_back(parseStatement());
    }

    return statements;
}

ValueExpression Parser::parseValueExpression() {
    ValueExpression expression;

    if (current().kind == TokenKind::String) {
        expression.literal = consume(TokenKind::String, "Expected value.").text;
    } else if (current().kind == TokenKind::Character) {
        expression.literal = consume(TokenKind::Character, "Expected value.").text[0];
    } else if (current().kind == TokenKind::Number) {
        std::string value = consume(TokenKind::Number, "Expected value.").text;
        if (value.find('.') == std::string::npos) {
            expression.literal = std::stoi(value);
        } else {
            expression.literal = std::stod(value);
        }

    } else {
        std::string name = consume(TokenKind::Identifier, "Expected value.").text;

        if (match(TokenKind::Dot)) {
            name += "." + consume(
                TokenKind::Identifier,
                "Expected property after '.'."
            ).text;
        }

        if (name == "true") expression.literal = true;
        else if (name == "false") expression.literal = false;
        else if (isFontConstant(name)) expression.literal = name;
        else expression.variable = name;
    }

    if (current().kind == TokenKind::Plus ||
        current().kind == TokenKind::Minus) {
        auto arithmetic = std::make_shared<ArithmeticExpression>();
        arithmetic->left = expression;
        arithmetic->operation = current().kind == TokenKind::Plus ? '+' : '-';
        ++position_;
        arithmetic->right = parseValueExpression();
        expression.literal.reset();
        expression.variable.clear();
        expression.arithmetic = std::move(arithmetic);
    }

    return expression;
}

ComparisonOperator Parser::parseComparisonOperator() {
    if (match(TokenKind::EqualEqual)) return ComparisonOperator::Equal;
    if (match(TokenKind::NotEqual)) return ComparisonOperator::NotEqual;
    if (match(TokenKind::Less)) return ComparisonOperator::Less;
    if (match(TokenKind::LessEqual)) return ComparisonOperator::LessEqual;
    if (match(TokenKind::Greater)) return ComparisonOperator::Greater;
    if (match(TokenKind::GreaterEqual)) return ComparisonOperator::GreaterEqual;
    throw std::runtime_error("Expected comparison operator.");
}

Condition Parser::parseCondition(bool parenthesized) {
    if (parenthesized) {
        consume(TokenKind::OpenParen, "Expected '(' before condition.");
    }
    Condition condition = parseConditionOr();
    if (parenthesized) {
        consume(TokenKind::CloseParen, "Expected ')' after condition.");
    }
    return condition;
}

Condition Parser::parseConditionOr() {
    Condition condition = parseConditionAnd();
    while (match(TokenKind::OrOr)) {
        auto combined = std::make_shared<Condition>();
        combined->logical = Condition::LogicalOperator::Or;
        combined->leftCondition = std::make_shared<Condition>(std::move(condition));
        combined->rightCondition = std::make_shared<Condition>(parseConditionAnd());
        condition = std::move(*combined);
    }
    return condition;
}

Condition Parser::parseConditionAnd() {
    Condition condition = parseConditionPrimary();
    while (match(TokenKind::AndAnd)) {
        auto combined = std::make_shared<Condition>();
        combined->logical = Condition::LogicalOperator::And;
        combined->leftCondition = std::make_shared<Condition>(std::move(condition));
        combined->rightCondition = std::make_shared<Condition>(parseConditionPrimary());
        condition = std::move(*combined);
    }
    return condition;
}

Condition Parser::parseConditionPrimary() {
    if (match(TokenKind::OpenParen)) {
        Condition condition = parseConditionOr();
        consume(TokenKind::CloseParen, "Expected ')' after condition.");
        return condition;
    }

    Condition condition;

    if (match(TokenKind::Bang)) {
        condition.left = parseValueExpression();
        condition.hasComparison = false;
        condition.negated = true;
        return condition;
    }

    condition.left = parseValueExpression();

    if (current().kind == TokenKind::CloseParen ||
        current().kind == TokenKind::AndAnd || current().kind == TokenKind::OrOr) {
        condition.hasComparison = false;
        return condition;
    }

    condition.operation = parseComparisonOperator();
    condition.right = parseValueExpression();
    return condition;
}

std::shared_ptr<Statement> Parser::parseIfStatement() {
    consumeIdentifier("if");
    auto statement = std::make_shared<Statement>();
    auto node = std::make_shared<IfStatement>();
    node->condition = parseCondition();
    node->body = parseStatementBlock("Expected '{' after if condition.");

    if (current().kind == TokenKind::Identifier && current().text == "else") {
        consumeIdentifier("else");

        if (current().kind == TokenKind::Identifier && current().text == "if") {
            node->elseBody.push_back(parseIfStatement());
        } else {
            node->elseBody = parseStatementBlock("Expected '{' after else.");
        }
    }

    statement->value = std::move(node);
    return statement;
}

std::shared_ptr<Statement> Parser::parseWhileStatement() {
    consumeIdentifier("while");
    auto statement = std::make_shared<Statement>();
    auto node = std::make_shared<WhileStatement>();
    node->condition = parseCondition();
    node->body = parseStatementBlock("Expected '{' after while condition.");
    statement->value = std::move(node);
    return statement;
}

std::shared_ptr<Statement> Parser::parseForStatement() {
    consumeIdentifier("for");
    consume(TokenKind::OpenParen, "Expected '(' after for.");

    Token type = consume(TokenKind::Identifier, "Expected for initializer type.");
    if (type.text != "int" && type.text != "short" && type.text != "long" &&
        type.text != "float" && type.text != "double") {
        throw std::runtime_error("For initializer must use a numeric type.");
    }

    auto node = std::make_shared<ForStatement>();
    node->initializer = parseVariableDeclaration(type.text);
    node->condition = parseCondition(false);
    consume(TokenKind::Semicolon, "Expected ';' after for condition.");
    node->incrementVariable = consume(
        TokenKind::Identifier,
        "Expected for increment variable."
    ).text;
    consume(TokenKind::Increment, "Expected '++' in for loop.");
    consume(TokenKind::CloseParen, "Expected ')' after for clauses.");
    node->body = parseStatementBlock("Expected '{' after for clauses.");

    auto statement = std::make_shared<Statement>();
    statement->value = std::move(node);
    return statement;
}

std::shared_ptr<Statement> Parser::parseIncrementStatement() {
    std::string target = consume(
        TokenKind::Identifier,
        "Expected variable or widget property before '++'."
    ).text;

    std::string property;
    if (match(TokenKind::Dot)) {
        property = consume(
            TokenKind::Identifier,
            "Expected numeric property before '++'."
        ).text;
        if (property != "fontSize" && property != "radius" && property != "angle" &&
            property != "x" && property != "y") {
            throw std::runtime_error(
                "Only x, y, fontSize, radius, and angle can be incremented on widgets."
            );
        }
    }

    const bool decrement = match(TokenKind::Decrement);
    if (!decrement) {
        consume(TokenKind::Increment, "Expected '++' or '--'.");
    }
    consume(TokenKind::Semicolon, "Expected ';' after increment.");

    auto statement = std::make_shared<Statement>();
    statement->value = IncrementStatement{target, property, decrement};
    return statement;
}

std::shared_ptr<Statement> Parser::parseVariableAssignment() {
    VariableAssignment assignment;
    assignment.name = consume(
        TokenKind::Identifier,
        "Expected variable name before '='."
    ).text;
    consume(TokenKind::Equals, "Expected '=' after variable name.");
    assignment.value = parseValueExpression();
    consume(TokenKind::Semicolon, "Expected ';' after assignment.");

    auto statement = std::make_shared<Statement>();
    statement->value = std::move(assignment);
    return statement;
}

std::shared_ptr<Statement> Parser::parseTriggerStatement() {
    consumeIdentifier("trigger");
    auto node = std::make_shared<TriggerStatement>();

    consume(TokenKind::OpenParen, "Expected '(' after trigger.");

    if (current().kind == TokenKind::Identifier && current().text == "mouseMove") {
        consume(TokenKind::Identifier, "Expected trigger event.");
        node->mouseMove = true;
    } else if (current().kind == TokenKind::Identifier &&
               (current().text == "onkey" || current().text == "onkeyci")) {
        const bool insensitive = current().text == "onkeyci";
        ++position_;
        consume(TokenKind::OpenParen, "Expected '(' after key trigger.");
        const std::string key = consumeString("Expected key name in key trigger.");
        consume(TokenKind::CloseParen, "Expected ')' after key name.");
        static const std::vector<std::string> special = {
            "space", "esc", "enter", "shift", "alt", "up", "down", "left", "right", "tab"
        };
        const bool singleLetter = key.size() == 1 &&
            std::isalpha(static_cast<unsigned char>(key[0]));
        if (!singleLetter &&
            std::find(special.begin(), special.end(), key) == special.end()) {
            throw std::runtime_error("Key triggers accept one letter or a named special key.");
        }
        node->key = key;
        node->keyCaseInsensitive = insensitive;
    } else {
        node->condition = parseCondition(false);
    }

    consume(TokenKind::CloseParen, "Expected ')' after trigger condition.");
    node->body = parseStatementBlock("Expected '{' after trigger condition.");

    auto statement = std::make_shared<Statement>();
    statement->value = node;
    return statement;
}
