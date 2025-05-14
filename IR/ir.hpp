#pragma once
#include <string>
#include <vector>
#include <variant>

struct IRInstruction {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
};

using IR = std::vector<IRInstruction>;
