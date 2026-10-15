#pragma once

#include <optional>
#include <string>
#include <vector>

enum class ElementType {
    Label,
    Button
};

struct SetTextAction {
    std::string targetId;
    std::string text;
};

struct ElementNode {
    ElementType type;
    std::string id;
    std::string text;
    int x = 0;
    int y = 0;
    int width = 100;
    int height = 30;
    std::optional<SetTextAction> onClick;
};

struct ProgramNode {
    std::string title = "Pixelate App";
    int width = 800;
    int height = 600;
    std::vector<ElementNode> elements;
};
