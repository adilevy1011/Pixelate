#pragma once

#include <optional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

enum class ElementType {
    Label,
    Button
};

enum class VariableType {
    Int,
    Short,
    Long,
    Float,
    Double,
    Bool,
    Char,
    String
};

using VariableValue = std::variant<
    int,
    short,
    long long,
    float,
    double,
    bool,
    char,
    std::string
>;

struct VariableDeclaration {
    VariableType type;
    std::string name;
    VariableValue value;
};

struct ValueExpression {
    std::optional<VariableValue> literal;
    std::string variable;
};

enum class ComparisonOperator {
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual
};

struct Condition {
    ValueExpression left;
    ComparisonOperator operation;
    ValueExpression right;
};

struct ColorNode {
    int red = 255;
    int green = 255;
    int blue = 255;
};

struct UpdateAction {
    std::string targetId;
    std::string property = "text";
    struct TextTemplate {
        std::string text;
    };
    std::variant<int, std::string, ColorNode, TextTemplate> value;
};

struct UpdateBlock {
    std::string targetId;
    std::vector<UpdateAction> actions;
};

struct Statement;

struct IfStatement {
    Condition condition;
    std::vector<std::shared_ptr<Statement>> body;
};

struct WhileStatement {
    Condition condition;
    std::vector<std::shared_ptr<Statement>> body;
};

struct ForStatement {
    VariableDeclaration initializer;
    Condition condition;
    std::string incrementVariable;
    std::vector<std::shared_ptr<Statement>> body;
};

struct IncrementStatement {
    std::string variable;
};

struct TriggerStatement {
    bool mouseMove = false;
    std::optional<Condition> condition;
    std::vector<std::shared_ptr<Statement>> body;
};

struct Statement {
    std::variant<
        VariableDeclaration,
        UpdateAction,
        UpdateBlock,
        IncrementStatement,
        std::shared_ptr<IfStatement>,
        std::shared_ptr<WhileStatement>,
        std::shared_ptr<ForStatement>,
        std::shared_ptr<TriggerStatement>
    > value;
};

struct CloseTriggerNode {
    std::string message = "Do you really want to exit?";
    std::string option = "YESNO";
};

struct ElementNode {
    ElementType type;
    std::string id;
    std::string text;
    int x = 0;
    int y = 0;
    int width = 100;
    int height = 30;
    std::optional<std::vector<UpdateAction>> onClick;
};

struct ProgramNode {
    std::string title = "Pixelate App";
    int width = 800;
    int height = 600;
    std::string iconPath = "window\\pixelate.ico";
    ColorNode background;
    std::optional<CloseTriggerNode> closeTrigger;
    std::vector<VariableDeclaration> variables;
    std::vector<std::shared_ptr<Statement>> statements;
    std::vector<std::shared_ptr<TriggerStatement>> triggers;
    std::vector<ElementNode> elements;
};
