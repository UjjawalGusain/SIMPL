#pragma once
#include <string>
#include <unordered_map>
#include "ir.hpp"

// Forward declarations of AST nodes
class ASTNode;
class ReturnNode;
class VariableNode;
class DeclarationNode;
class VarDeclareNode;
class NumberLiteralNode;
class StringLiteralNode;
class IfStatementNode;
class ComparisonNode;
class LogicalExprNode;
class BinaryExprNode;
class UnaryExprNode;
class AssignmentNode;
class PrintNode;
class FunctionNode;
class CallExprNode;
class WhileNode;
class BlockNode;

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