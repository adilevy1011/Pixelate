#include "pxl_runtime.h"

#include "../language/interpreter.h"
#include "../window/app.h"
#include "../window/ui/button.h"
#include "../window/ui/label.h"

#include <exception>
#include <utility>

namespace {

std::wstring widen(const std::string& value) {
    return std::wstring(value.begin(), value.end());
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

        auto app = std::make_unique<App>(options);
        labels_.clear();

        for (const ElementNode& node : program.elements) {
            if (node.type == ElementType::Label) {
                auto label = std::make_unique<Label>(widen(node.text));
                label->setPosition(node.x, node.y);
                label->setSize(node.width, node.height);

                labels_[node.id] = label.get();
                app->add(std::move(label));
                continue;
            }

            auto button = std::make_unique<Button>(widen(node.text));
            button->setPosition(node.x, node.y);
            button->setSize(node.width, node.height);

            if (node.onClick) {
                SetTextAction action = *node.onClick;

                button->setOnClick([this, action] {
                    auto label = labels_.find(action.targetId);

                    if (label != labels_.end()) {
                        label->second->setText(widen(action.text));

                        if (app_) {
                            app_->requestRedraw();
                        }
                    }
                });
            }

            app->add(std::move(button));
        }

        app_ = std::move(app);
        return true;
    } catch (const std::exception& exception) {
        error = exception.what();
        app_.reset();
        labels_.clear();
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

    return app_->run();
}
