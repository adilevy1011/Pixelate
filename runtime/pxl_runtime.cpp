#include "pxl_runtime.h"

#include "../language/interpreter.h"
#include "../window/app.h"
#include "../window/ui/button.h"
#include "../window/ui/label.h"

#include <exception>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace {

std::wstring widen(const std::string& value) {
    return std::wstring(value.begin(), value.end());
}

}

VariableValue PxlRuntime::evaluateExpression(
    const ValueExpression& expression
) const {
    if (expression.literal) {
        return *expression.literal;
    }

    if (expression.variable == "mouse.x" || expression.variable == "mouse.y") {
        if (!app_) {
            throw std::runtime_error("Mouse position is unavailable.");
        }

        const MousePosition position = app_->mousePosition();
        return expression.variable == "mouse.x" ? position.x : position.y;
    }

    auto variable = variables_.find(expression.variable);
    if (variable == variables_.end()) {
        throw std::runtime_error("Unknown variable: " + expression.variable);
    }

    return variable->second;
}

bool PxlRuntime::evaluateCondition(const Condition& condition) const {
    const VariableValue left = evaluateExpression(condition.left);
    const VariableValue right = evaluateExpression(condition.right);

    auto numeric = [](const VariableValue& value, double& result) {
        return std::visit([&](const auto& item) {
            using Type = std::decay_t<decltype(item)>;
            if constexpr (std::is_arithmetic_v<Type>) {
                result = static_cast<double>(item);
                return true;
            } else {
                return false;
            }
        }, value);
    };

    double leftNumber = 0.0;
    double rightNumber = 0.0;
    const bool numericValues = numeric(left, leftNumber) && numeric(right, rightNumber);

    if (condition.operation == ComparisonOperator::Equal) {
        return numericValues ? leftNumber == rightNumber : left == right;
    }

    if (condition.operation == ComparisonOperator::NotEqual) {
        return numericValues ? leftNumber != rightNumber : left != right;
    }

    if (!numericValues) {
        throw std::runtime_error("Relational comparisons require numeric values.");
    }

    switch (condition.operation) {
        case ComparisonOperator::Less: return leftNumber < rightNumber;
        case ComparisonOperator::LessEqual: return leftNumber <= rightNumber;
        case ComparisonOperator::Greater: return leftNumber > rightNumber;
        case ComparisonOperator::GreaterEqual: return leftNumber >= rightNumber;
        default: return false;
    }
}

void PxlRuntime::applyUpdateAction(const UpdateAction& action) {
    if (action.targetId == "mouse" && action.property == "shape") {
        const auto* shape = std::get_if<std::string>(&action.value);
        if (!shape || !app_) return;

        static const std::unordered_map<std::string, MouseShape> shapes = {
            {"ARROW", MouseShape::Arrow},
            {"IBEAM", MouseShape::IBeam},
            {"WAIT", MouseShape::Wait},
            {"CROSS", MouseShape::Cross},
            {"UPARROW", MouseShape::UpArrow},
            {"SIZE", MouseShape::Size},
            {"ICON", MouseShape::Icon},
            {"SIZENWSE", MouseShape::SizeNWSE},
            {"SIZENESW", MouseShape::SizeNESW},
            {"SIZEWE", MouseShape::SizeWE},
            {"SIZENS", MouseShape::SizeNS},
            {"SIZEALL", MouseShape::SizeAll},
            {"NO", MouseShape::No},
            {"HAND", MouseShape::Hand},
            {"APPSTARTING", MouseShape::AppStarting},
            {"HELP", MouseShape::Help},
            {"PIN", MouseShape::Pin},
            {"PERSON", MouseShape::Person}
        };

        auto cursor = shapes.find(*shape);
        if (cursor != shapes.end()) {
            app_->setMouseShape(cursor->second);
        }
        return;
    }

    if (action.targetId == "window") {
        if (!app_) return;

        if (action.property == "title") {
            if (const auto* title = std::get_if<std::string>(&action.value)) {
                app_->setTitle(widen(*title));
            } else if (const auto* formatted =
                       std::get_if<UpdateAction::TextTemplate>(&action.value)) {
                app_->setTitle(widen(renderText(*formatted)));
            }
        } else if (action.property == "background") {
            if (const auto* color = std::get_if<ColorNode>(&action.value)) {
                app_->setBackgroundColor({color->red, color->green, color->blue});
            }
        }
        return;
    }

    auto element = elements_.find(action.targetId);
    if (element == elements_.end()) return;

    Widget* widget = element->second;

    if (action.property == "x" || action.property == "y" ||
        action.property == "width" || action.property == "height") {
        const auto* number = std::get_if<int>(&action.value);
        if (!number) return;

        int x = widget->x();
        int y = widget->y();
        int width = widget->width();
        int height = widget->height();

        if (action.property == "x") x = *number;
        if (action.property == "y") y = *number;
        if (action.property == "width") width = *number;
        if (action.property == "height") height = *number;

        if (width > 0 && height > 0) {
            widget->setPosition(x, y);
            widget->setSize(width, height);
        }
        return;
    }

    if (action.property != "text") return;

    std::string text;
    if (const auto* plainText = std::get_if<std::string>(&action.value)) {
        text = *plainText;
    } else if (const auto* formatted =
               std::get_if<UpdateAction::TextTemplate>(&action.value)) {
        text = renderText(*formatted);
    } else {
        return;
    }

    if (auto* label = dynamic_cast<Label*>(element->second)) {
        label->setText(widen(text));
    } else if (auto* button = dynamic_cast<Button*>(element->second)) {
        button->setText(widen(text));
    }
}

std::string PxlRuntime::renderText(
    const UpdateAction::TextTemplate& text
) const {
    std::string result;

    for (std::size_t index = 0; index < text.text.size();) {
        if (text.text[index] != '{') {
            result.push_back(text.text[index++]);
            continue;
        }

        const std::size_t end = text.text.find('}', index + 1);
        if (end == std::string::npos) {
            throw std::runtime_error("Unclosed formatted string expression.");
        }

        ValueExpression expression;
        expression.variable = text.text.substr(index + 1, end - index - 1);
        VariableValue value = evaluateExpression(expression);

        std::visit([&result](const auto& item) {
            using Type = std::decay_t<decltype(item)>;
            if constexpr (std::is_same_v<Type, std::string>) {
                result += item;
            } else if constexpr (std::is_same_v<Type, bool>) {
                result += item ? "true" : "false";
            } else if constexpr (std::is_same_v<Type, char>) {
                result.push_back(item);
            } else {
                result += std::to_string(item);
            }
        }, value);

        index = end + 1;
    }

    return result;
}

void PxlRuntime::fireMouseMoveTriggers() {
    for (const auto& trigger : triggers_) {
        if (trigger->mouseMove) {
            executeStatements(trigger->body);
        }
    }
}

void PxlRuntime::checkTriggers() {
    for (std::size_t index = 0; index < triggers_.size(); ++index) {
        const auto& trigger = triggers_[index];
        if (trigger->mouseMove || !trigger->condition) continue;

        const bool active = evaluateCondition(*trigger->condition);
        if (active && !triggerStates_[index]) {
            executeStatements(trigger->body);
        }
        triggerStates_[index] = active;
    }
}

void PxlRuntime::incrementVariable(const std::string& name) {
    auto variable = variables_.find(name);
    if (variable == variables_.end()) {
        throw std::runtime_error("Unknown variable: " + name);
    }

    std::visit([](auto& value) {
        using Type = std::decay_t<decltype(value)>;
        if constexpr (std::is_arithmetic_v<Type> && !std::is_same_v<Type, bool>) {
            ++value;
        } else {
            throw std::runtime_error("Only numeric variables can be incremented.");
        }
    }, variable->second);
}

void PxlRuntime::executeStatement(const Statement& statement) {
    if (const auto* declaration = std::get_if<VariableDeclaration>(&statement.value)) {
        variables_[declaration->name] = declaration->value;
        return;
    }

    if (const auto* update = std::get_if<UpdateAction>(&statement.value)) {
        applyUpdateAction(*update);
        return;
    }

    if (const auto* updateBlock = std::get_if<UpdateBlock>(&statement.value)) {
        for (const UpdateAction& action : updateBlock->actions) {
            applyUpdateAction(action);
        }
        return;
    }

    if (const auto* increment = std::get_if<IncrementStatement>(&statement.value)) {
        incrementVariable(increment->variable);
        return;
    }

    if (const auto* ifStatement = std::get_if<std::shared_ptr<IfStatement>>(
            &statement.value)) {
        if (evaluateCondition((*ifStatement)->condition)) {
            executeStatements((*ifStatement)->body);
        }
        return;
    }

    if (const auto* whileStatement = std::get_if<std::shared_ptr<WhileStatement>>(
            &statement.value)) {
        std::size_t iterations = 0;
        while (evaluateCondition((*whileStatement)->condition)) {
            if (++iterations > 100000) {
                throw std::runtime_error("While loop exceeded 100000 iterations.");
            }
            executeStatements((*whileStatement)->body);
        }
        return;
    }

    const auto& forStatement = std::get<std::shared_ptr<ForStatement>>(statement.value);
    variables_[forStatement->initializer.name] = forStatement->initializer.value;

    std::size_t iterations = 0;
    while (evaluateCondition(forStatement->condition)) {
        if (++iterations > 100000) {
            throw std::runtime_error("For loop exceeded 100000 iterations.");
        }
        executeStatements(forStatement->body);
        incrementVariable(forStatement->incrementVariable);
    }
}

void PxlRuntime::executeStatements(
    const std::vector<std::shared_ptr<Statement>>& statements
) {
    for (const auto& statement : statements) {
        executeStatement(*statement);
    }
}

PxlRuntime::PxlRuntime() = default;
PxlRuntime::~PxlRuntime() = default;

bool PxlRuntime::loadFile(
    const std::string& path,
    std::string& error
) {
    try {
        Interpreter interpreter;
        ProgramNode program = interpreter.parseFile(path);

        AppOptions options;
        options.title = widen(program.title);
        options.width = program.width;
        options.height = program.height;
        options.iconPath = widen(program.iconPath);
        options.backgroundColor = {
            program.background.red,
            program.background.green,
            program.background.blue
        };

        if (program.closeTrigger) {
            options.closeTrigger.enabled = true;
            options.closeTrigger.message = widen(
                program.closeTrigger->message
            );
            options.closeTrigger.option = CloseDialogOption::YesNo;
        }

        auto app = std::make_unique<App>(options);
        elements_.clear();
        variables_.clear();
        statements_.clear();
        triggers_.clear();
        triggerStates_.clear();

        for (const VariableDeclaration& variable : program.variables) {
            if (variables_.find(variable.name) != variables_.end()) {
                throw std::runtime_error(
                    "Variable already declared: " + variable.name
                );
            }

            variables_[variable.name] = variable.value;
        }

        for (const ElementNode& node : program.elements) {
            if (node.type == ElementType::Label) {
                auto label = std::make_unique<Label>(widen(node.text));
                label->setPosition(node.x, node.y);
                label->setSize(node.width, node.height);

                elements_[node.id] = label.get();
                app->add(std::move(label));
                continue;
            }

            auto button = std::make_unique<Button>(widen(node.text));
            button->setPosition(node.x, node.y);
            button->setSize(node.width, node.height);

            elements_[node.id] = button.get();

            if (node.onClick) {
                std::vector<UpdateAction> actions = *node.onClick;

                button->setOnClick([this, actions] {
                    for (const UpdateAction& action : actions) {
                        applyUpdateAction(action);
                    }

                    if (app_) {
                        app_->requestRedraw();
                    }
                });
            }

            app->add(std::move(button));
        }

        app_ = std::move(app);
        statements_ = std::move(program.statements);
        triggers_ = std::move(program.triggers);
        triggerStates_.assign(triggers_.size(), false);
        return true;
    } catch (const std::exception& exception) {
        error = exception.what();
        app_.reset();
        elements_.clear();
        variables_.clear();
        statements_.clear();
        triggers_.clear();
        triggerStates_.clear();
        return false;
    }
}

int PxlRuntime::run() {
    if (!app_) {
        return 1;
    }

    if (!app_->create()) {
        return 1;
    }

    app_->setOnMouseMove([this](MousePosition) {
        try {
            fireMouseMoveTriggers();
            app_->requestRedraw();
        } catch (const std::exception&) {
        }
    });
    app_->setOnTick([this] {
        try {
            checkTriggers();
            app_->requestRedraw();
        } catch (const std::exception&) {
        }
    });

    try {
        executeStatements(statements_);
        app_->requestRedraw();
    } catch (const std::exception&) {
        return 1;
    }

    return app_->run();
}
