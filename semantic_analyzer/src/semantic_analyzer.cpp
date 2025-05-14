#include "../include/semantic_analyzer.hpp"
#include <stdexcept>
#include <bits/stdc++.h>
#include <iostream>
#include <../Parser/ast.hpp>

struct FunctionSignature {
    std::vector<Type> paramTypes;
};

std::unordered_map<std::string, FunctionSignature> functionTable;

std::string typeToString(Type type) {
    switch (type) {
        case Type::NUMBER: return "number";
        case Type::STRING: return "string";
        case Type::UNKNOWN: return "unknown";
        default: return "invalid";
    }
}

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

// void SemanticAnalyzer::visitDeclaration(const DeclarationNode* node) {
//     Type type = node->type == TokenType::NUMBER ? Type::NUMBER : Type::STRING;

//     for (const auto& decl : node->declarations) {
//         std::string name = decl->name.value;
//         if (symbolTable.isDeclared(name)) {
//             throw std::runtime_error("Variable already declared: " + name);
//         }

//         Type initType = Type::UNKNOWN;
//         if (decl->initializer) {
//             initType = evaluateExpression(decl->initializer);
//             if (initType != type) {
//                 throw std::runtime_error("Type mismatch in initialization of variable: " + name);
//             }
//         }

//         symbolTable.declare(name, type);
//         if (decl->initializer) {
//             symbolTable.assign(name);  
//         }
//     }
// }

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

        if (inGlobalScope) {
            symbolTable.declareGlobal(name, type);
        } else {
            symbolTable.declare(name, type);
        }

        if (decl->initializer) {
            symbolTable.assign(name);
        }
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
    std::vector<Type> paramTypes;
    for (const auto& param : node->parameters) {
        std::string typeStr = param.first;
        Type t = typeStr == "number" ? Type::NUMBER : Type::STRING;
        paramTypes.push_back(t);
    }

    if (functionTable.find(node->name) != functionTable.end()) {
        std::cerr << "Error: Function " << node->name << " already declared!" << std::endl;
        return;
    }

    functionTable[node->name] = FunctionSignature{paramTypes};

    inGlobalScope = false;
    symbolTable.enterScope();
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        const auto& param = node->parameters[i];
        std::string name = param.second;
        Type t = paramTypes[i];
        symbolTable.declare(name, t);
    }

    visit(node->functionBlock.get());
    symbolTable.exitScope();

    inGlobalScope = true;
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

void SemanticAnalyzer::printFunctionTable() const {
    std::cout << "\n--- Function Table ---\n";
    for (const auto& [name, signature] : functionTable) {
        std::cout << "Function: " << name << "(";
        for (size_t i = 0; i < signature.paramTypes.size(); ++i) {
            std::cout << typeToString(signature.paramTypes[i]);
            if (i < signature.paramTypes.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << ")\n";
    }
}


void SymbolTable::print() const {
    std::cout << "\n--- Symbol Table ---\n";

    std::cout << "Global Scope:\nsize: ";


    for (const auto& [name, info] : globalScope) {
        std::string typeStr = (info.type == Type::NUMBER ? "NUMBER" :
                               info.type == Type::STRING ? "STRING" : "UNKNOWN");
        std::cout << "  " << name << " -> Type: " << typeStr
                  << ", Initialized: " << (info.isInitialized ? "Yes" : "No") << "\n";
    }

    int scopeLevel = scopes.size();
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it, --scopeLevel) {
        std::cout << "Scope Level " << scopeLevel << ":\n";
        for (const auto& [name, info] : *it) {
            std::string typeStr = (info.type == Type::NUMBER ? "NUMBER" :
                                   info.type == Type::STRING ? "STRING" : "UNKNOWN");
            std::cout << "  " << name << " -> Type: " << typeStr
                      << ", Initialized: " << (info.isInitialized ? "Yes" : "No") << "\n";
        }
    }
}



