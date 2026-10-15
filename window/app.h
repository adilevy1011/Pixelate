#pragma once
#include <vector>
#include "ui/widget.h"
#include <memory>
#include <functional>
#include <string>

struct AppColor {
    int red = 255;
    int green = 255;
    int blue = 255;
};

enum class CloseDialogOption {
    YesNo
};

struct CloseTrigger {
    bool enabled = false;
    std::wstring message = L"Do you really want to exit?";
    CloseDialogOption option = CloseDialogOption::YesNo;
};

enum class MouseShape {
    Arrow,
    IBeam,
    Wait,
    Cross,
    UpArrow,
    Size,
    Icon,
    SizeNWSE,
    SizeNESW,
    SizeWE,
    SizeNS,
    SizeAll,
    No,
    Hand,
    AppStarting,
    Help,
    Pin,
    Person
};

struct MousePosition {
    int x = 0;
    int y = 0;
};
/**
 * Define your app's structure by settings it's options
 * Possible options:
 * - title: std::wstring
 * - width: int
 * - height: int
 * - iconPath: std:wstring
 */
struct AppOptions {
    std::wstring title = L"Pixelate App";
    /**The width of the window */
    int width = 800;
    /**The height of the window */
    int height = 600;
    /**Path to your app icon*/
    std::wstring iconPath = L"window\\pixelate.ico";
    /**Window background color, with channels from 0 to 255*/
    AppColor backgroundColor{};
    CloseTrigger closeTrigger{};
};

class App {
public:
    using MouseMoveHandler = std::function<void(MousePosition)>;
    using TickHandler = std::function<void()>;

    explicit App(AppOptions options = {});
    ~App();
    /**
     * Creates an app
     */
    bool create();
    int run();
    void add(std::unique_ptr<Widget> element);
    void requestRedraw();
    void setTitle(const std::wstring& title);
    void setBackgroundColor(AppColor color);
    void setMouseShape(MouseShape shape);
    MousePosition mousePosition() const;
    void setOnMouseMove(MouseMoveHandler handler);
    void setOnTick(TickHandler handler);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    std::vector<std::unique_ptr<Widget>> elements_;
};
