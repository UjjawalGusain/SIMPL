#pragma once
#include <iostream>
#include "ast.hpp"

void printAST(const ASTNode* node, int indent = 0);

void printIndent(int indent) {
    for(int i = 0; i < indent; ++i) std::cout << "  ";
}

void printAST(const ASTNode* node, int indent) {
    if (!node) {
        printIndent(indent);
        std::cout << "null\n";
        return;
    }

    if (auto num = dynamic_cast<const NumberLiteralNode*>(node)) {
        printIndent(indent);
        std::cout << "NumberLiteral(" << num->value << ")\n";
    }
    else if (auto str = dynamic_cast<const StringLiteralNode*>(node)) {
        printIndent(indent);
        std::cout << "StringLiteral(\"" << str->value << "\")\n";
    }
    else if (auto var = dynamic_cast<const VariableNode*>(node)) {
        printIndent(indent);
        std::cout << "Variable(" << var->name << ")\n";
    }
    else if (auto bin = dynamic_cast<const BinaryExprNode*>(node)) {
        printIndent(indent);
        std::cout << "BinaryExpr(" << (int)bin->op << ")\n";
        printAST(bin->left.get(), indent + 1);
        printAST(bin->right.get(), indent + 1);
    }
    else if (auto assign = dynamic_cast<const AssignmentNode*>(node)) {
        printIndent(indent);
        std::cout << "Assignment\n";
        printAST(assign->left.get(), indent + 1);
        printAST(assign->rightExpression.get(), indent + 1);
    }
    else if (auto decl = dynamic_cast<const DeclarationNode*>(node)) {
        printIndent(indent);
        std::cout << "Declaration(" << (decl->type == TokenType::NUMBER ? "number" : "string") << ")\n";
        for (const auto& d : decl->declarations)
            printAST(d.get(), indent + 1);
    }
    else if (auto vardecl = dynamic_cast<const VarDeclareNode*>(node)) {
        printIndent(indent);
        std::cout << "VarDeclare(" << vardecl->name.value << ")\n";
        if (vardecl->initializer)
            printAST(vardecl->initializer.get(), indent + 1);
    }
    else if (auto ret = dynamic_cast<const ReturnNode*>(node)) {
        printIndent(indent);
        std::cout << "Return\n";
        printAST(ret->returnExpression.get(), indent + 1);
    }
    else if (auto print = dynamic_cast<const PrintNode*>(node)) {
        printIndent(indent);
        std::cout << "Print\n";
        printAST(print->expression.get(), indent + 1);
    }
    else if (auto block = dynamic_cast<const BlockNode*>(node)) {
        printIndent(indent);
        std::cout << "Block\n";
        for (const auto& stmt : block->statements)
            printAST(stmt.get(), indent + 1);
    }
    else if (auto func = dynamic_cast<const FunctionNode*>(node)) {
        printIndent(indent);
        std::cout << "Function(" << func->name << ")\n";
        for (const auto& [type, name] : func->parameters) {
            printIndent(indent + 1);
            std::cout << "Param(" << type << " " << name << ")\n";
        }
        printAST(func->functionBlock.get(), indent + 1);
    }
    else if (auto call = dynamic_cast<const CallExprNode*>(node)) {
        printIndent(indent);
        std::cout << "Call(" << call->functionName << ")\n";
        for (const auto& arg : call->arguments)
            printAST(arg.get(), indent + 1);
    }
    else if (auto ifstmt = dynamic_cast<const IfStatementNode*>(node)) {
        printIndent(indent);
        std::cout << "IfStatement\n";
        for (const auto& [cond, block] : ifstmt->conditionBlocks) {
            printIndent(indent + 1);
            std::cout << "Condition:\n";
            printAST(cond.get(), indent + 2);
            printIndent(indent + 1);
            std::cout << "Block:\n";
            printAST(block.get(), indent + 2);
        }
        if (ifstmt->elseBranch) {
            printIndent(indent + 1);
            std::cout << "Else:\n";
            printAST(ifstmt->elseBranch.get(), indent + 2);
        }
    }
    else if (auto whilestmt = dynamic_cast<const WhileNode*>(node)) {
        printIndent(indent);
        std::cout << "While\n";
        printIndent(indent + 1);
        std::cout << "Condition:\n";
        printAST(whilestmt->conditionStatement.get(), indent + 2);
        printIndent(indent + 1);
        std::cout << "Block:\n";
        printAST(whilestmt->whileBlock.get(), indent + 2);
    }
    else {
        printIndent(indent);
        std::cout << "Unknown AST Node\n";
    }
}
