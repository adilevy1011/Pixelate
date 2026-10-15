#include "interpreter.h"

#include "lexer.h"
#include "parser.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

ProgramNode Interpreter::parseSource(const std::string& source) const {
    Lexer lexer(source);
    Parser parser(lexer.tokenize());
    return parser.parse();
}

ProgramNode Interpreter::parseFile(const std::string& path) const {
    std::ifstream file(path);

    if (!file) {
        throw std::runtime_error("Could not open PXL file: " + path);
    }

    std::ostringstream source;
    source << file.rdbuf();
    return parseSource(source.str());
}
