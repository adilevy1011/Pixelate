#pragma once

#include "../language/ast.h"

#include <memory>
#include <string>
#include <unordered_map>

class App;
class Label;
class Widget;
struct MousePosition;

class PxlRuntime {
public:
    PxlRuntime();
    ~PxlRuntime();

    bool loadFile(const std::string& path, std::string& error);
    int run();

private:
    struct RuntimeView {
        std::string id;
        std::unique_ptr<App> app;
        std::unordered_map<std::string, Widget*> elements;
    };

    VariableValue evaluateExpression(const ValueExpression& expression) const;
    bool evaluateCondition(const Condition& condition) const;
    void executeStatements(const std::vector<std::shared_ptr<Statement>>& statements);
    void executeStatement(const Statement& statement);
    void incrementVariable(const std::string& name);
    void applyUpdateAction(const UpdateAction& action);
    std::string renderText(const UpdateAction::TextTemplate& text) const;
    void checkTriggers();
    void fireMouseMoveTriggers();
    void switchView(const SwitchViewStatement& statement);
    void configureView(RuntimeView& view, const WindowNode& window);

    App* app_ = nullptr;
    std::vector<std::unique_ptr<RuntimeView>> views_;
    RuntimeView* activeView_ = nullptr;
    std::unordered_map<std::string, Widget*> elements_;
    std::unordered_map<std::string, VariableValue> variables_;
    std::vector<std::shared_ptr<Statement>> statements_;
    std::vector<std::shared_ptr<TriggerStatement>> triggers_;
    std::vector<bool> triggerStates_;
};
