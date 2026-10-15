#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class App;
class Label;

class PxlRuntime {
public:
    PxlRuntime();
    ~PxlRuntime();

    bool loadFile(const std::string& path, std::string& error);
    int run();

private:
    std::unique_ptr<App> app_;
    std::unordered_map<std::string, Label*> labels_;
};
