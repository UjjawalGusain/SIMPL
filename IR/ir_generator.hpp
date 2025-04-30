#pragma once
#include "Parser/ast.hpp"
#include "ir.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class IRGenerator {
public:
    IR generate(const ASTNode* root);

private:
    IR instructions;
    int tempVarCounter = 0;
    std::string generateTemp();
    std::string generateFromNode(const ASTNode* node);
};
