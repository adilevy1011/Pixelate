#include "win32_draw_context.h"

void Win32DrawContext::fillRectangle(
    int x,
    int y,
    int width,
    int height
) {
    RECT rectangle{x, y, x + width, y + height};
    HBRUSH brush = CreateSolidBrush(RGB(70, 120, 220));

    FillRect(deviceContext_, &rectangle, brush);
    DeleteObject(brush);
}

void Win32DrawContext::drawText(
    const std::wstring& text,
    int x,
    int y,
    int width,
    int height
) {
    RECT area{x, y, x + width, y + height};
    int oldBackgroundMode = SetBkMode(deviceContext_, TRANSPARENT);
    COLORREF oldTextColor = SetTextColor(
        deviceContext_,
        RGB(0, 0, 0)
    );

    DrawTextW(
        deviceContext_,
        text.c_str(),
        -1,
        &area,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    SetBkMode(deviceContext_, oldBackgroundMode);
    SetTextColor(deviceContext_, oldTextColor);
}
