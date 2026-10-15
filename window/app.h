#pragma once
#include <vector>
#include "ui/widget.h"
#include <memory>
#include <string>
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
};

class App {
public:
    explicit App(AppOptions options = {});
    ~App();
    /**
     * Creates an app
     */
    bool create();
    int run();
    void add(std::unique_ptr<Widget> element);
    void requestRedraw();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    std::vector<std::unique_ptr<Widget>> elements_;
};
