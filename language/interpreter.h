#pragma once

#include "ast.h"

#include <string>

class Interpreter {
public:
    ProgramNode parseSource(const std::string& source) const;
    ProgramNode parseFile(const std::string& path) const;
};
