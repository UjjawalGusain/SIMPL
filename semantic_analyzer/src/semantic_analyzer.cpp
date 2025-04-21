#include "../include/semantic_analyzer.hpp"
#include <stdexcept>
#include <bits/stdc++.h>
#include <iostream>
#include <../Parser/ast.hpp>


void SemanticAnalyzer::analyze(ASTNode* root) {
    visit(root);
}

void SemanticAnalyzer::visit(ASTNode* node) {
    if (!node) return;

    if (auto decl = dynamic_cast<DeclarationNode*>(node)) {
        visitDeclaration(decl);
    } else if (auto var = dynamic_cast<VariableNode*>(node)) {
        visitVariable(var);
    } else if (auto assign = dynamic_cast<AssignmentNode*>(node)) {
        visitAssignment(assign);
    } else if (auto ifStmt = dynamic_cast<IfStatementNode*>(node)) {
        visitIfStatement(ifStmt);
    } else if (auto whileStmt = dynamic_cast<WhileNode*>(node)) {
        visitWhileLoop(whileStmt);
    } else if (auto block = dynamic_cast<BlockNode*>(node)) {
        visitBlock(block);
    } else if (auto ret = dynamic_cast<ReturnNode*>(node)) {
        visitReturn(ret);
    } else if (auto func = dynamic_cast<FunctionNode*>(node)) {
        visitFunction(func);
    } else if (auto call = dynamic_cast<CallExprNode*>(node)) {
        visitCallExpr(call);
    } else if (auto print = dynamic_cast<PrintNode*>(node)) {
        visitPrint(print);
    }
}

void SemanticAnalyzer::visitDeclaration(const DeclarationNode* node) {
    Type type = node->type == TokenType::NUMBER ? Type::NUMBER : Type::STRING;

    for (const auto& decl : node->declarations) {
        std::string name = decl->name.value;
        if (symbolTable.isDeclared(name)) {
            throw std::runtime_error("Variable already declared: " + name);
        }

        Type initType = Type::UNKNOWN;
        if (decl->initializer) {
            initType = evaluateExpression(decl->initializer);
            if (initType != type) {
                throw std::runtime_error("Type mismatch in initialization of variable: " + name);
            }
        }

        symbolTable.declare(name, type);
    }
}

void SemanticAnalyzer::visitVariable(const VariableNode* node) {
    if (!symbolTable.isDeclared(node->name)) {
        throw std::runtime_error("Undeclared variable: " + node->name);
    }
}

void SemanticAnalyzer::visitAssignment(const AssignmentNode* node) {
    auto var = dynamic_cast<VariableNode*>(node->left.get());
    if (!var) throw std::runtime_error("Left-hand side of assignment must be a variable.");

    std::string name = var->name;
    if (!symbolTable.isDeclared(name)) {
        throw std::runtime_error("Assignment to undeclared variable: " + name);
    }

    Type lhs = symbolTable.getType(name);
    Type rhs = evaluateExpression(node->rightExpression);

    if (lhs != rhs) {
        throw std::runtime_error("Type mismatch in assignment to variable: " + name);
    }
}

void SemanticAnalyzer::visitIfStatement(const IfStatementNode* node) {
    for (const auto& pair : node->conditionBlocks) {
        const auto& cond = pair.first;
        const auto& body = pair.second;
        Type condType = evaluateExpression(cond);
        if (condType != Type::NUMBER) {
            throw std::runtime_error("Condition in if-statement must be numeric (boolean logic).");
        }
        visit(body.get());
    }

    if (node->elseBranch) {
        visit(node->elseBranch.get());
    }
}

void SemanticAnalyzer::visitWhileLoop(const WhileNode* node) {
    Type condType = evaluateExpression(node->conditionStatement);
    if (condType != Type::NUMBER) {
        throw std::runtime_error("Condition in while-loop must be numeric (boolean logic).");
    }
    visit(node->whileBlock.get());
}

void SemanticAnalyzer::visitBlock(const BlockNode* node) {
    symbolTable.enterScope();
    for (const auto& stmt : node->statements) {
        visit(stmt.get());
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::visitReturn(const ReturnNode* node) {
    evaluateExpression(node->returnExpression);
}

void SemanticAnalyzer::visitFunction(const FunctionNode* node) {
    symbolTable.enterScope();
    for (const auto& param : node->parameters) {
        std::string typeStr = param.first;
        std::string name = param.second;
        Type t = typeStr == "number" ? Type::NUMBER : Type::STRING;
        symbolTable.declare(name, t);
    }
    visit(node->functionBlock.get());
    symbolTable.exitScope();
}

void SemanticAnalyzer::visitCallExpr(const CallExprNode* node) {
    for (const auto& arg : node->arguments) {
        evaluateExpression(arg);
    }
}

void SemanticAnalyzer::visitPrint(const PrintNode* node) {
    evaluateExpression(node->expression);
}

Type SemanticAnalyzer::evaluateExpression(const std::unique_ptr<ASTNode>& node) {
    if (auto bin = dynamic_cast<BinaryExprNode*>(node.get())) {
        return visitBinaryExpr(bin);
    } else if (auto un = dynamic_cast<UnaryExprNode*>(node.get())) {
        return visitUnaryExpr(un);
    } else if (auto num = dynamic_cast<NumberLiteralNode*>(node.get())) {
        return visitNumberLiteral(num);
    } else if (auto str = dynamic_cast<StringLiteralNode*>(node.get())) {
        return visitStringLiteral(str);
    } else if (auto var = dynamic_cast<VariableNode*>(node.get())) {
        return symbolTable.getType(var->name);
    } else if (auto comp = dynamic_cast<ComparisonNode*>(node.get())) {
        return visitComparisonExpr(comp);
    } else if (auto logic = dynamic_cast<LogicalExprNode*>(node.get())) {
        return visitLogicalExpr(logic);
    } else if (auto call = dynamic_cast<CallExprNode*>(node.get())) {
        visitCallExpr(call);
        return Type::UNKNOWN;
    }

    return Type::UNKNOWN;
}

Type SemanticAnalyzer::visitBinaryExpr(const BinaryExprNode* node) {
    Type lhs = evaluateExpression(node->left);
    Type rhs = evaluateExpression(node->right);

    if (lhs != rhs) {
        throw std::runtime_error("Type mismatch in binary expression");
    }

    return lhs;
}

Type SemanticAnalyzer::visitUnaryExpr(const UnaryExprNode* node) {
    return evaluateExpression(node->operand);
}

Type SemanticAnalyzer::visitNumberLiteral(const NumberLiteralNode* node) {
    return Type::NUMBER;
}

Type SemanticAnalyzer::visitStringLiteral(const StringLiteralNode* node) {
    return Type::STRING;
}

Type SemanticAnalyzer::visitComparisonExpr(const ComparisonNode* node) {
    Type lhs = evaluateExpression(node->leftExpression);
    Type rhs = evaluateExpression(node->rightExpression);
    if (lhs != rhs) {
        throw std::runtime_error("Type mismatch in comparison expression");
    }
    return Type::NUMBER;
}

Type SemanticAnalyzer::visitLogicalExpr(const LogicalExprNode* node) {
    Type lhs = evaluateExpression(node->leftExpression);
    Type rhs = evaluateExpression(node->rightExpression);
    if (lhs != Type::NUMBER || rhs != Type::NUMBER) {
        throw std::runtime_error("Logical operations require numeric (boolean) expressions");
    }
    return Type::NUMBER;
}