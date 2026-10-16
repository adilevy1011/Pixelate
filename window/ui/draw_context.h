#pragma once

#include <string>
#include "../platform/platform_types.h"

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
        int height,
        const TextStyle& style
    ) = 0;

    virtual void drawParagraph(
        const std::wstring& text,
        int x,
        int y,
        int width,
        int height,
        const TextStyle& style
    ) = 0;

protected:
    DrawContext() = default;
};
