#pragma once

#include "widget.h"

#include <string>

class TextBox : public Widget {
public:
    explicit TextBox(std::wstring text);
    ~TextBox() override;

    void attachToWindow(HWND parent) override;
    void draw(DrawContext& context) override;

    void setText(std::wstring text);
    std::wstring text() const;
    void setPlaceholder(std::wstring placeholder);

protected:
    void syncNativeControl() override;

private:
    static LRESULT CALLBACK EditProc(
        HWND hwnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );
    void showPlaceholder();
    void hidePlaceholder();

    HWND edit_ = nullptr;
    WNDPROC previousEditProc_ = nullptr;
    std::wstring text_;
    std::wstring placeholder_;
    bool showingPlaceholder_ = false;
};
