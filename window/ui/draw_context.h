#pragma once

#include <string>

class DrawContext {
public:
    virtual ~DrawContext() = default;

    virtual void fillRectangle(
        int x,
        int y,
        int width,
        int height
    ) = 0;

    virtual void drawText(
        const std::wstring& text,
        int x,
        int y,
        int width,
        int height
    ) = 0;

protected:
    DrawContext() = default;
};
