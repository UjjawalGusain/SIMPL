#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "../Parser/ast.hpp"
#include "symbol_table.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

class SemanticAnalyzer {
public:
    SemanticAnalyzer() : currentScopeName("global"), currentFunctionExpectedReturnType(Type::UNKNOWN) {}

    void analyze(ASTNode *root);
    const SymbolTable &getSymbolTable(const std::string &scopeName) const;
    void printAllSymbolTables() const;
    int currentLoopDepth = 0;

private:
    std::unordered_map<std::string, std::unique_ptr<SymbolTable>> allSymbolTables;
    std::string currentScopeName;
    std::string currentFunctionName;
    Type currentFunctionExpectedReturnType;

    void visit(ASTNode *node);

    void visitVariable(const VariableNode *node);
    void visitDeclaration(const DeclarationNode *node);
    void visitAssignment(const AssignmentNode *node);
    void visitIfStatement(const IfStatementNode *node);
    void visitWhileLoop(const WhileNode *node);
    void visitBlock(const BlockNode *node);
    void visitReturn(const ReturnNode *node);
    void visitFunction(const FunctionNode *node);
    void visitCallExpr(const CallExprNode *node);
    void visitPrint(const PrintNode *node);

    Type evaluateExpression(const std::unique_ptr<ASTNode> &node);
    Type visitBinaryExpr(const BinaryExprNode *node);
    Type visitUnaryExpr(const UnaryExprNode *node);
    Type visitNumberLiteral();
    Type visitStringLiteral();
    Type visitComparisonExpr(const ComparisonNode *node);
    Type visitLogicalExpr(const LogicalExprNode *node);

    SymbolTable &getCurrentSymbolTable();
};

#endif