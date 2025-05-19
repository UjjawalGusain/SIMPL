#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include "../IR/ir.hpp"

class Interpreter {
public:
    void execute(const std::vector<IRInstruction>& instructions);

private:
    std::unordered_map<std::string, std::string> variables;

    std::string evaluate(const std::string& operand);
    bool toBool(const std::string& val);
};

