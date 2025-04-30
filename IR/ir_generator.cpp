#include "ir_generator.hpp"
#include <sstream>

std::string IRGenerator::generateTemp() {
    return "t" + std::to_string(tempVarCounter++);
}

IR IRGenerator::generate(const ASTNode* root) {
    instructions.clear();
    tempVarCounter = 0;
    generateFromNode(root);
    return instructions;
}

std::string IRGenerator::generateFromNode(const ASTNode* node) {
    if (auto num = dynamic_cast<const NumberLiteralNode*>(node)) {
        std::string temp = generateTemp();
        instructions.push_back({"load_const", std::to_string(num->value), "", temp});
        return temp;
    }
    else if (auto var = dynamic_cast<const VariableNode*>(node)) {
        return var->name;
    }
    else if (auto bin = dynamic_cast<const BinaryExprNode*>(node)) {
        std::string left = generateFromNode(bin->left.get());
        std::string right = generateFromNode(bin->right.get());
        std::string result = generateTemp();
        std::string op;
        switch (bin->op) {
            case TokenType::PLUS: op = "add"; break;
            case TokenType::MINUS: op = "sub"; break;
            case TokenType::STAR: op = "mul"; break;
            case TokenType::SLASH: op = "div"; break;
            default: op = "unknown";
        }
        instructions.push_back({op, left, right, result});
        return result;
    }
    else if (auto assign = dynamic_cast<const AssignmentNode*>(node)) {
        std::string rhs = generateFromNode(assign->rightExpression.get());
        std::string lhs = generateFromNode(assign->left.get());
        instructions.push_back({"store", rhs, "", lhs});
        return lhs;
    }
    else if (auto block = dynamic_cast<const BlockNode*>(node)) {
        for (const auto& stmt : block->statements) {
            generateFromNode(stmt.get());
        }
    }
    return "";
}
