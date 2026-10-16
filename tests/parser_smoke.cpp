#include "../language/interpreter.h"

#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: pixelate_parser_smoke <demo-directory>\n";
        return 2;
    }

    Interpreter interpreter;
    int parsed = 0;
    for (const auto& entry : std::filesystem::directory_iterator(argv[1])) {
        if (!entry.is_regular_file() || entry.path().extension() != ".pxl") continue;
        try {
            interpreter.parseFile(entry.path().string());
            ++parsed;
        } catch (const std::exception& exception) {
            std::cerr << entry.path() << ": " << exception.what() << '\n';
            return 1;
        }
    }

    if (parsed == 0) {
        std::cerr << "No .pxl files found in " << argv[1] << '\n';
        return 1;
    }

    std::cout << "Parsed " << parsed << " demo programs.\n";
    return 0;
}
