#include "../runtime/pxl_runtime.h"

#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: pixelate <file.pxl>\n";
        return 1;
    }

    PxlRuntime runtime;
    std::string error;

    if (!runtime.loadFile(argv[1], error)) {
        std::cerr << "PXL error: " << error << '\n';
        return 1;
    }

    return runtime.run();
}
