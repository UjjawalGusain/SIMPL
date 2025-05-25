#pragma once
#include <string>
#include <unordered_map>
#include "ir.hpp"

class IRGenerator {
    IR ir;
    int labelCounter = 0;
    int tempVarCounter = 0;

    std::unordered_map<std::string, std::string> symbolTable;

    std::string newLabel();
    std::string newTemp();

    std::string generateExpression(ASTNode* node);

public:
    IRGenerator() = default;

    void generate(ASTNode* node);
    const IR& getIR() const;
};