#include "../runtime/pxl_runtime.h"
#include "../language/docs.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc == 2 && std::string(argv[1]) == "--docs") {
        std::cout << PixelateDocs::embeddedPxldocs();
        return 0;
    }

    if (argc != 2) {
        std::cerr << "Usage: pixelate <file.pxl> | pixelate --docs\n";
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
