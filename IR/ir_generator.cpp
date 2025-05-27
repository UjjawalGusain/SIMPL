#include "ir_generator.hpp"
#include "ast.hpp"
#include <iostream>
#include <cassert>

std::string IRGenerator::newLabel() {
    return "L" + std::to_string(labelCounter++);
}

std::string IRGenerator::newTemp() {
    return "t" + std::to_string(tempVarCounter++);
}

const IR& IRGenerator::getIR() const {
    return ir;
}

void IRGenerator::generate(ASTNode* node) {
    if (!node) return;

    if (auto* retNode = dynamic_cast<ReturnNode*>(node)) {
        std::string val = generateExpression(retNode->returnExpression.get());
        ir.add({"ret", val});
    } 
    else if (auto* declNode = dynamic_cast<DeclarationNode*>(node)) {
        for (auto& varDecl : declNode->declarations) {
            std::string varName = varDecl->name.value;
            std::string initVal = varDecl->initializer ? generateExpression(varDecl->initializer.get()) : "0";
            ir.add({"var", varName});  
            ir.add({"assign", varName, initVal});
        }
    }
    else if (auto* assignNode = dynamic_cast<AssignmentNode*>(node)) {
        std::string lhs = dynamic_cast<VariableNode*>(assignNode->left.get())->name;
        std::string rhs = generateExpression(assignNode->rightExpression.get());
        ir.add({"assign", lhs, rhs}); 
    }
    else if (auto* printNode = dynamic_cast<PrintNode*>(node)) {
        std::string val = generateExpression(printNode->expression.get());
        ir.add({"print", val}); 
    }
    else if (auto* blockNode = dynamic_cast<BlockNode*>(node)) {
        for (auto& stmt : blockNode->statements) {
            generate(stmt.get());
        }
    }
    else if (auto* ifNode = dynamic_cast<IfStatementNode*>(node)) {
        std::string endLabel = newLabel();
        for (size_t i = 0; i < ifNode->conditionBlocks.size(); ++i) {
            auto& cond = ifNode->conditionBlocks[i].first;
            auto& block = ifNode->conditionBlocks[i].second;

            std::string condVal = generateExpression(cond.get());
            std::string nextLabel = newLabel();

            ir.add({"ifz_goto", condVal, nextLabel});  
            generate(block.get());
            ir.add({"goto", endLabel});  
            ir.add({"label", nextLabel});  
        }

        if (ifNode->elseBranch) {
            generate(ifNode->elseBranch.get()); 
        }

        ir.add({"label", endLabel}); 
    }
    else if (auto* whileNode = dynamic_cast<WhileNode*>(node)) {
        std::string startLabel = newLabel();
        std::string endLabel = newLabel();

        ir.add({"label", startLabel});
        std::string condVal = generateExpression(whileNode->conditionStatement.get());
        ir.add({"ifz_goto", condVal, endLabel});
        generate(whileNode->whileBlock.get());
        ir.add({"goto", startLabel});  
        ir.add({"label", endLabel});  
    }
    else if (auto* funcNode = dynamic_cast<FunctionNode*>(node)) {
        ir.add({"func_start", funcNode->name});
        for (auto& param : funcNode->parameters) {
            ir.add({"param", param.second});  
        }
        generate(funcNode->functionBlock.get());
        ir.add({"func_end", funcNode->name});
    }
    else if (auto* callNode = dynamic_cast<CallExprNode*>(node)) {
        for (auto& arg : callNode->arguments) {
            std::string val = generateExpression(arg.get());
            ir.add({"arg", val});
        }
        ir.add({"call", callNode->functionName, std::to_string(callNode->arguments.size())});
    }
}

std::string IRGenerator::generateExpression(ASTNode* node) {
    if (!node) return "";

    if (auto* numNode = dynamic_cast<NumberLiteralNode*>(node)) {
        return numNode->value;
    }
    else if (auto* strNode = dynamic_cast<StringLiteralNode*>(node)) {
        return "\"" + strNode->value + "\"";
    }
    else if (auto* varNode = dynamic_cast<VariableNode*>(node)) {
        return varNode->name;  
    }
    else if (auto* binaryNode = dynamic_cast<BinaryExprNode*>(node)) {
        std::string left = generateExpression(binaryNode->left.get());
        std::string right = generateExpression(binaryNode->right.get());
        std::string temp = newTemp();
        std::string op;

        switch (binaryNode->op) {
            case TokenType::PLUS: op = "add"; break;
            case TokenType::MINUS: op = "sub"; break;
            case TokenType::MULTIPLY: op = "mul"; break;
            case TokenType::DIVIDE: 
                op = "div"; 
                break;
            default: op = "unknown"; break;
        }
        ir.add({op, left, right, temp});
        return temp;
    }
    else if (auto* compNode = dynamic_cast<ComparisonNode*>(node)) {
        std::string left = generateExpression(compNode->leftExpression.get());
        std::string right = generateExpression(compNode->rightExpression.get());
        std::string temp = newTemp();
        std::string op;

        switch (compNode->op) {
            case TokenType::EQ: op = "eq"; break;
            case TokenType::NEQ : op = "neq"; break;
            case TokenType::LT: op = "lt"; break;
            case TokenType::LEQ : op = "le"; break;
            case TokenType::GT: op = "gt"; break;
            case TokenType::GEQ: op = "ge"; break;
            default: op = "unknown"; break;
        }
        ir.add({op, left, right, temp});
        return temp;
    }
    else if (auto* logicalNode = dynamic_cast<LogicalExprNode*>(node)) {
        std::string left = generateExpression(logicalNode->leftExpression.get());
        std::string right = generateExpression(logicalNode->rightExpression.get());
        std::string temp = newTemp();
        std::string op;

        switch (logicalNode->op) {
            case TokenType::AND: op = "and"; break;
            case TokenType::OR: op = "or"; break;
            default: op = "unknown"; break;
        }
        ir.add({op, left, right, temp});
        return temp;
    }
    else if (auto* unaryNode = dynamic_cast<UnaryExprNode*>(node)) {
        std::string operand = generateExpression(unaryNode->operand.get());
        std::string temp = newTemp();
        std::string op;

        switch (unaryNode->op) {
            case TokenType::MINUS: op = "neg"; break;
            case TokenType::NOT: op = "not"; break;
            default: op = "unknown"; break;
        }
        ir.add({op, operand, "", temp});
        return temp;
    }
    else if (auto* callNode = dynamic_cast<CallExprNode*>(node)) {
        for (auto& arg : callNode->arguments) {
            std::string val = generateExpression(arg.get());
            ir.add({"arg", val});
        }
        ir.add({"call", callNode->functionName, std::to_string(callNode->arguments.size())});
        std::string temp = newTemp();
        ir.add({"move", "retval", "", temp});
        return temp;
    }

    return "";
}
