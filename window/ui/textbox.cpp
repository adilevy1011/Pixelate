#include "textbox.h"

#include <windows.h>
#include <utility>

#ifndef EM_SETCUEBANNER
#define EM_SETCUEBANNER (WM_USER + 1)
#endif

TextBox::TextBox(std::wstring text)
    : text_(std::move(text)) {
}

TextBox::~TextBox() {
    if (edit_) {
        DestroyWindow(edit_);
    }
}

void TextBox::attachToWindow(HWND parent) {
    edit_ = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        text_.c_str(),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
        x_,
        y_,
        width_,
        height_,
        parent,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr
    );

    if (edit_) {
        SendMessageW(
            edit_,
            WM_SETFONT,
            reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)),
            TRUE
        );

        SetWindowLongPtrW(
            edit_,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(this)
        );
        previousEditProc_ = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
            edit_,
            GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(&TextBox::EditProc)
        ));

        showPlaceholder();
    }
}

void TextBox::draw(DrawContext&) {
    // The native EDIT control draws itself.
}

void TextBox::setText(std::wstring text) {
    text_ = std::move(text);

    if (edit_) {
        showingPlaceholder_ = false;

        SetWindowTextW(edit_, text_.c_str());
        if (text_.empty() && GetFocus() != edit_) {
            showPlaceholder();
        }
    }
}

std::wstring TextBox::text() const {
    if (showingPlaceholder_) {
        return {};
    }

    if (!edit_) {
        return text_;
    }

    int length = GetWindowTextLengthW(edit_);
    std::wstring value(static_cast<std::size_t>(length) + 1, L'\0');

    if (length >= 0) {
        GetWindowTextW(edit_, value.data(), length + 1);
    }

    value.resize(static_cast<std::size_t>(length));

    return value;
}

void TextBox::setPlaceholder(std::wstring placeholder) {
    placeholder_ = std::move(placeholder);

    if (edit_) {
        if (GetFocus() == edit_) {
            hidePlaceholder();
        } else {
            showPlaceholder();
        }
    }
}

void TextBox::showPlaceholder() {
    if (!edit_ || placeholder_.empty() || showingPlaceholder_ ||
        GetFocus() == edit_) {
        return;
    }

    // Only show the placeholder when the actual native control is empty.
    // This also handles a user deleting text that was present initially.
    if (GetWindowTextLengthW(edit_) > 0) {
        return;
    }

    showingPlaceholder_ = true;
    SetWindowTextW(edit_, placeholder_.c_str());
}

void TextBox::hidePlaceholder() {
    if (!edit_ || !showingPlaceholder_) {
        return;
    }

    showingPlaceholder_ = false;
    SetWindowTextW(edit_, text_.c_str());
}

LRESULT CALLBACK TextBox::EditProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    auto* textBox = reinterpret_cast<TextBox*>(
        GetWindowLongPtrW(hwnd, GWLP_USERDATA)
    );

    if (textBox && message == WM_SETFOCUS) {
        textBox->hidePlaceholder();
    }

    if (textBox && message == WM_SETCURSOR) {
        SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(32513)));
        return TRUE;
    }

    LRESULT result = textBox && textBox->previousEditProc_
        ? CallWindowProcW(
            textBox->previousEditProc_,
            hwnd,
            message,
            wParam,
            lParam
        )
        : DefWindowProcW(hwnd, message, wParam, lParam);

    if (textBox && message == WM_KILLFOCUS) {
        textBox->showPlaceholder();
    }

    return result;
}

void TextBox::syncNativeControl() {
    if (edit_) {
        SetWindowPos(
            edit_,
            nullptr,
            x_,
            y_,
            width_,
            height_,
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }
}
