#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "../Parser/ast.hpp"
#include "symbol_table.hpp"
#include <memory>
#include <string>
#include <stdexcept>

class SemanticAnalyzer {
public:
    void analyze(ASTNode* root);

private:
    SymbolTable symbolTable;

    void visit(ASTNode* node);

    void visitVariable(const VariableNode* node);
    void visitDeclaration(const DeclarationNode* node);
    void visitAssignment(const AssignmentNode* node);
    void visitIfStatement(const IfStatementNode* node);
    void visitWhileLoop(const WhileNode* node);
    void visitBlock(const BlockNode* node);
    void visitReturn(const ReturnNode* node);
    void visitFunction(const FunctionNode* node);
    void visitCallExpr(const CallExprNode* node);
    void visitPrint(const PrintNode* node);

    Type evaluateExpression(const std::unique_ptr<ASTNode>& node);
    Type visitBinaryExpr(const BinaryExprNode* node);
    Type visitUnaryExpr(const UnaryExprNode* node);
    Type visitNumberLiteral(const NumberLiteralNode* node);
    Type visitStringLiteral(const StringLiteralNode* node);
    Type visitComparisonExpr(const ComparisonNode* node);
    Type visitLogicalExpr(const LogicalExprNode* node);
};

#endif // SEMANTIC_ANALYZER_HPP