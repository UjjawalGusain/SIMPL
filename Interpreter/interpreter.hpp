#pragma once
#include "ir.hpp"
#include <unordered_map>
#include <string>

class Interpreter {
public:
    void execute(const IR& ir);
private:
    std::unordered_map<std::string, int> variables;
};
