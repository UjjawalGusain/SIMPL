#pragma once
#include <string>
#include <unordered_map>
#include "ast.hpp"
#include "ir.hpp"

class IRGenerator {
    IR ir;
    int labelCounter = 0;
    int tempVarCounter = 0;

    std::string newLabel();
    std::string newTemp();

    std::string generateExpression(ASTNode* node);

public:
    IRGenerator() = default;


    void generate(ASTNode* node);
    const IR& getIR() const;
};