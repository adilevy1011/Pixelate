#pragma once

#include <windows.h>
#include <string>

class DrawContext {
public:
    explicit DrawContext(HDC deviceContext)
        : deviceContext_(deviceContext) {
    }
    void fillRectangle(
        int x,
        int y,
        int width,
        int height
    );

    void drawText(
        const std::wstring& text,
        int x,
        int y,
        int width,
        int height
    );
private:
    HDC deviceContext_;
};
