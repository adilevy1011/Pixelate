#include "app.h"
#include <utility>
#include <windows.h>
#include <utility>
#include "ui/draw_context.h"
#include <windowsx.h>
struct App::Impl {
    AppOptions options;
    App* owner = nullptr;
    HINSTANCE hInstance{};
    HWND hwnd{};

    bool create();
    int run();

    static LRESULT CALLBACK WindowProc(
        HWND hwnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );

    LRESULT handleMessage(
        HWND hwnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );
};

App::App(AppOptions options)
    : impl_(std::make_unique<Impl>()) {
    impl_->owner = this;
    impl_->options = std::move(options);
    impl_->hInstance = GetModuleHandleW(nullptr);
}

App::~App() = default;

bool App::create() {
    return impl_->create();
}

int App::run() {
    return impl_->run();
}

void App::add(std::unique_ptr<Widget> element) {
    elements_.push_back(std::move(element));
}

void App::requestRedraw() {
    if (impl_->hwnd) {
        InvalidateRect(impl_->hwnd, nullptr, TRUE);
    }
}
bool App::Impl::create() {
    WNDCLASSEXW windowClass{};

    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursorW(
        nullptr,
        MAKEINTRESOURCEW(32512)
    );
    windowClass.hIcon = static_cast<HICON>(
        LoadImageW(
            nullptr,
            options.iconPath.c_str(),
            IMAGE_ICON,
            0,
            0,
            LR_LOADFROMFILE | LR_DEFAULTSIZE
        )
    );
    windowClass.hIconSm = static_cast<HICON>(
        LoadImageW(
            nullptr,
            options.iconPath.c_str(),
            IMAGE_ICON,
            16,
            16,
            LR_LOADFROMFILE
        )
    );
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszClassName = L"Pixelate";

    if (!RegisterClassExW(&windowClass)) {
        MessageBoxW(
            nullptr,
            L"Window registration failed.",
            L"Error",
            MB_OK | MB_ICONERROR
        );

        return false;
    }

    hwnd = CreateWindowExW(
        0,
        L"Pixelate",
        options.title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        options.width,
        options.height,
        nullptr,
        nullptr,
        hInstance,
        this
    );

    if (!hwnd) {
        MessageBoxW(
            nullptr,
            L"Window creation failed.",
            L"Error",
            MB_OK | MB_ICONERROR
        );

        return false;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    return true;
}

int App::Impl::run() {
    MSG message{};

    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}

LRESULT CALLBACK App::Impl::WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    auto* app = reinterpret_cast<Impl*>(
        GetWindowLongPtrW(hwnd, GWLP_USERDATA)
    );

    if (message == WM_NCCREATE) {
        auto* createStruct =
            reinterpret_cast<CREATESTRUCTW*>(lParam);

        app = static_cast<Impl*>(createStruct->lpCreateParams);

        SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(app)
        );

        app->hwnd = hwnd;
    }

    if (app) {
        return app->handleMessage(
            hwnd,
            message,
            wParam,
            lParam
        );
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT App::Impl::handleMessage(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    switch (message) {
        case WM_SETCURSOR: {
            POINT cursorPosition{};
            GetCursorPos(&cursorPosition);
            ScreenToClient(hwnd, &cursorPosition);

            for (auto iterator = owner->elements_.rbegin();
                iterator != owner->elements_.rend();
                ++iterator) {
                Widget& element = **iterator;

                if (element.containsPoint(
                        cursorPosition.x,
                        cursorPosition.y
                    )) {
                    if (element.cursor() == CursorType::Hand) {
                        HCURSOR handCursor = LoadCursorW(
                            nullptr,
                            MAKEINTRESOURCEW(32649)
                        );

                        SetCursor(handCursor);
                        return TRUE;
                    }

                    break;
                }
            }

            HCURSOR defaultCursor = LoadCursorW(
                nullptr,
                MAKEINTRESOURCEW(32512)
            );

            SetCursor(defaultCursor);
            return TRUE;
        }
        case WM_LBUTTONDOWN: {
            int mouseX = GET_X_LPARAM(lParam);
            int mouseY = GET_Y_LPARAM(lParam);

            for (auto iterator = owner->elements_.rbegin();
                iterator != owner->elements_.rend();
                ++iterator) {
                Widget& element = **iterator;

                if (element.onMouseDown(mouseX, mouseY)) {
                    InvalidateRect(hwnd, nullptr, TRUE);
                    break;
                }
            }

            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT paint{};
            HDC deviceContext = BeginPaint(hwnd, &paint);

            DrawContext context(deviceContext);

            for (auto& element : owner->elements_) {
                element->draw(context);
            }

            EndPaint(hwnd, &paint);
            return 0;
        }
        case WM_CLOSE:
            if (MessageBoxW(
                    hwnd,
                    L"Do you really want to exit?",
                    L"Confirm",
                    MB_YESNO | MB_ICONQUESTION
                ) == IDYES) {
                DestroyWindow(hwnd);
            }

            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}
