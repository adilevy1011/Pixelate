#include "win32_draw_context.h"

namespace {

const wchar_t* fontName(FontFamily family) {
    switch (family) {
        case FontFamily::Arial: return L"Arial";
        case FontFamily::Calibri: return L"Calibri";
        case FontFamily::Consolas: return L"Consolas";
        case FontFamily::CourierNew: return L"Courier New";
        case FontFamily::TimesNewRoman: return L"Times New Roman";
        case FontFamily::Tahoma: return L"Tahoma";
        case FontFamily::Verdana: return L"Verdana";
        case FontFamily::Default: return L"Segoe UI";
    }
    return L"Segoe UI";
}

HFONT createFont(const TextStyle& style) {
    const int size = style.size > 0 ? style.size : 9;
    return CreateFontW(-size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontName(style.family));
}

}

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
    int height,
    const TextStyle& style
) {
    RECT area{x, y, x + width, y + height};
    int oldBackgroundMode = SetBkMode(deviceContext_, TRANSPARENT);
    COLORREF oldTextColor = SetTextColor(
        deviceContext_,
        RGB(0, 0, 0)
    );
    HFONT font = createFont(style);
    HFONT oldFont = static_cast<HFONT>(SelectObject(deviceContext_, font));

    DrawTextW(
        deviceContext_,
        text.c_str(),
        -1,
        &area,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    SetBkMode(deviceContext_, oldBackgroundMode);
    SetTextColor(deviceContext_, oldTextColor);
    SelectObject(deviceContext_, oldFont);
    DeleteObject(font);
}

void Win32DrawContext::drawParagraph(
    const std::wstring& text,
    int x,
    int y,
    int width,
    int height,
    const TextStyle& style
) {
    RECT area{x, y, x + width, y + height};
    int oldBackgroundMode = SetBkMode(deviceContext_, TRANSPARENT);
    COLORREF oldTextColor = SetTextColor(deviceContext_, RGB(0, 0, 0));
    HFONT font = createFont(style);
    HFONT oldFont = static_cast<HFONT>(SelectObject(deviceContext_, font));

    DrawTextW(
        deviceContext_,
        text.c_str(),
        -1,
        &area,
        DT_LEFT | DT_TOP | DT_WORDBREAK
    );

    SetBkMode(deviceContext_, oldBackgroundMode);
    SetTextColor(deviceContext_, oldTextColor);
    SelectObject(deviceContext_, oldFont);
    DeleteObject(font);
}
