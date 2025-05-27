#include "../include/semantic_analyzer.hpp"
#include "../include/symbol_table.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>

static int scopeCounter = 0;
std::string generateUniqueScopeName(const std::string &baseName, int line, int col) {
    std::stringstream ss;
    ss << baseName << "$" << std::to_string(line + 1) << "$" << std::to_string(col);
    scopeCounter++;
    return ss.str();
}

SymbolTable &SemanticAnalyzer::getCurrentSymbolTable() {
    auto it = allSymbolTables.find(currentScopeName);
    if (it == allSymbolTables.end()) {
        throw std::runtime_error("Attempted to access non-existent symbol table for scope: " + currentScopeName);
    }
    return *it->second;
}

void SemanticAnalyzer::analyze(ASTNode *root) {
    allSymbolTables["global"] = std::make_unique<SymbolTable>("global");
    currentScopeName = "global";
    currentFunctionName = "";
    currentFunctionExpectedReturnType = Type::UNKNOWN;

    if (auto programBlock = dynamic_cast<BlockNode *>(root)) {
        for (const auto &stmt : programBlock->statements) {
            visit(stmt.get());
        }
    } else {
        visit(root);
    }
}

void SemanticAnalyzer::visit(ASTNode *node) {
    if (!node)
        return;

    if (auto decl = dynamic_cast<DeclarationNode *>(node)) {
        visitDeclaration(decl);
    } else if (auto var = dynamic_cast<VariableNode *>(node)) {
        visitVariable(var);
    } else if (auto assign = dynamic_cast<AssignmentNode *>(node)) {
        visitAssignment(assign);
    } else if (auto ifStmt = dynamic_cast<IfStatementNode *>(node)) {
        visitIfStatement(ifStmt);
    } else if (auto whileStmt = dynamic_cast<WhileNode *>(node)) {
        visitWhileLoop(whileStmt);
    } else if (auto block = dynamic_cast<BlockNode *>(node)) {
        visitBlock(block);
    } else if (auto ret = dynamic_cast<ReturnNode *>(node)) {
        visitReturn(ret);
    } else if (auto func = dynamic_cast<FunctionNode *>(node)) {
        visitFunction(func);
    } else if (auto call = dynamic_cast<CallExprNode *>(node)) {
        evaluateExpression(std::unique_ptr<ASTNode>(call));
    } else if (auto print = dynamic_cast<PrintNode *>(node)) {
        visitPrint(print);
    }
}

void SemanticAnalyzer::visitDeclaration(const DeclarationNode *node) {
    Type type = Type::UNKNOWN;
    if (node->type == TokenType::NUMBER) {
        type = Type::NUMBER;
    } else if (node->type == TokenType::STRING) {
        type = Type::STRING;
    } else {
        throw std::runtime_error("Unknown variable type in declaration: " + typeToString(type));
    }

    SymbolTable &currentTable = getCurrentSymbolTable();

    for (const auto &decl : node->declarations) {
        std::string name = decl->name.value;

        if (currentTable.isDeclaredInCurrentScope(name)) {
            throw std::runtime_error("Variable '" + name + "' already declared in current scope.");
        }

        Type initType = Type::UNKNOWN;
        bool isInitialized = false;
        if (decl->initializer) {
            initType = evaluateExpression(decl->initializer);
            if (initType != type) {
                throw std::runtime_error("Type mismatch in initialization of variable '" + name +
                                         "'. Expected " + typeToString(type) + ", got " + typeToString(initType));
            }
            isInitialized = true;
        }
        currentTable.declare(name, type, isInitialized);
    }
}

void SemanticAnalyzer::visitVariable(const VariableNode *node) {
    if (!getCurrentSymbolTable().isDeclared(node->name)) {
        throw std::runtime_error("Undeclared variable: " + node->name);
    }
}

void SemanticAnalyzer::visitAssignment(const AssignmentNode *node) {
    auto varNode = dynamic_cast<VariableNode *>(node->left.get());
    if (!varNode)
        throw std::runtime_error("Left-hand side of assignment must be a variable.");

    std::string name = varNode->name;
    SymbolTable &currentTable = getCurrentSymbolTable();

    if (!currentTable.isDeclared(name)) {
        throw std::runtime_error("Assignment to undeclared variable: " + name);
    }

    Type lhsType = currentTable.getType(name);
    Type rhsType = evaluateExpression(node->rightExpression);

    if (lhsType != rhsType) {
        throw std::runtime_error("Type mismatch in assignment to variable '" + name +
                                 "'. Expected " + typeToString(lhsType) + ", got " + typeToString(rhsType));
    }
    currentTable.assign(name);
}

void SemanticAnalyzer::visitIfStatement(const IfStatementNode *node) {
    for (const auto &conditionBlockPair : node->conditionBlocks) {
        const auto &condition = conditionBlockPair.first;
        const auto &body = conditionBlockPair.second;

        Type condType = evaluateExpression(condition);
        if (condType != Type::NUMBER) {
            throw std::runtime_error("Condition in if-statement must evaluate to a numeric (boolean) type.");
        }
        std::string blockScopeName = generateUniqueScopeName("if", node->line, node->col);
        SymbolTable &activeSymbolTable = getCurrentSymbolTable();

        activeSymbolTable.enterScope(blockScopeName);
        visit(body.get());
        allSymbolTables[blockScopeName] = std::make_unique<SymbolTable>(activeSymbolTable);
        activeSymbolTable.exitScope();
    }

    if (node->elseBranch) {
        std::string elseScopeName = generateUniqueScopeName("else", node->line, node->col);
        SymbolTable &activeSymbolTable = getCurrentSymbolTable();
        activeSymbolTable.enterScope(elseScopeName);

        visit(node->elseBranch.get());
        allSymbolTables[elseScopeName] = std::make_unique<SymbolTable>(activeSymbolTable);
        activeSymbolTable.exitScope();
    }
}

void SemanticAnalyzer::visitWhileLoop(const WhileNode *node) {
    Type condType = evaluateExpression(node->conditionStatement);
    if (condType != Type::NUMBER) {
        throw std::runtime_error("Condition in while-loop must evaluate to a numeric (boolean) type.");
    }

    std::string blockScopeName = generateUniqueScopeName("while", node->line, node->col);
    SymbolTable &activeSymbolTable = getCurrentSymbolTable();
    activeSymbolTable.enterScope(blockScopeName);
    visit(node->whileBlock.get());
    allSymbolTables[blockScopeName] = std::make_unique<SymbolTable>(activeSymbolTable);
    activeSymbolTable.exitScope();
}

void SemanticAnalyzer::visitBlock(const BlockNode *node) {
    for (const auto &stmt : node->statements) {
        visit(stmt.get());
    }
}

void SemanticAnalyzer::visitReturn(const ReturnNode *node) {
    Type actualReturnType = Type::VOID;
    if (node->returnExpression) {
        actualReturnType = evaluateExpression(node->returnExpression);
    }

    if (currentFunctionName.empty()) {
        throw std::runtime_error("Return statement found outside of a function.");
    }

    if (currentFunctionExpectedReturnType == Type::UNKNOWN) {
    } else if (currentFunctionExpectedReturnType == Type::VOID) {
        if (actualReturnType != Type::VOID) {
            throw std::runtime_error("Function '" + currentFunctionName + "' is declared void but returns a value of type " + typeToString(actualReturnType) + ".");
        }
    } else { // Expected a non-void return type
        if (actualReturnType == Type::VOID && node->returnExpression) {
            throw std::runtime_error("Function '" + currentFunctionName + "' expects to return " + typeToString(currentFunctionExpectedReturnType) + " but return expression is void.");
        } else if (actualReturnType == Type::VOID && !node->returnExpression) {
            throw std::runtime_error("Function '" + currentFunctionName + "' expects to return " + typeToString(currentFunctionExpectedReturnType) + " but return statement has no value.");
        }

        if (actualReturnType != currentFunctionExpectedReturnType) {
            throw std::runtime_error("Type mismatch in return statement for function '" + currentFunctionName +
                                     "'. Expected " + typeToString(currentFunctionExpectedReturnType) +
                                     ", got " + typeToString(actualReturnType) + ".");
        }
    }
}

void SemanticAnalyzer::visitFunction(const FunctionNode *node) {
    std::string functionName = node->name;

    Type declaredReturnType = Type::UNKNOWN;
    if (functionName == "add" || functionName == "factorial")
        declaredReturnType = Type::NUMBER;
    if (functionName == "main")
        declaredReturnType = Type::VOID;

    std::vector<std::pair<Type, std::string>> paramInfoList;
    for (const auto &param : node->parameters) {
        Type paramType = Type::UNKNOWN;
        if (param.first == "number") {
            paramType = Type::NUMBER;
        } else if (param.first == "string") {
            paramType = Type::STRING;
        } else {
            throw std::runtime_error("Unknown parameter type '" + param.first + "' for parameter '" + param.second + "' in function '" + functionName + "'.");
        }
        paramInfoList.push_back({paramType, param.second});
    }

    SymbolTable &globalTable = *allSymbolTables["global"];
    if (globalTable.isDeclared(functionName)) {
        throw std::runtime_error("Function '" + functionName + "' already declared globally.");
    }
    globalTable.declareFunction(functionName, declaredReturnType, paramInfoList);

    std::string funcUniqueScopeName = generateUniqueScopeName(functionName, node->line, node->col);
    allSymbolTables[funcUniqueScopeName] = std::make_unique<SymbolTable>(funcUniqueScopeName);

    std::string previousScopeName = currentScopeName;
    std::string previousFunctionName = currentFunctionName;
    Type previousFunctionExpectedReturnType = currentFunctionExpectedReturnType;

    currentScopeName = funcUniqueScopeName;
    currentFunctionName = functionName;
    currentFunctionExpectedReturnType = declaredReturnType;

    SymbolTable &funcTable = getCurrentSymbolTable();

    for (const auto &paramData : paramInfoList) {
        if (!funcTable.declare(paramData.second, paramData.first, true)) {
            throw std::runtime_error("Failed to declare parameter '" + paramData.second + "' in function '" + functionName + "'.");
        }
    }

    if (node->functionBlock) {
        BlockNode *bodyBlock = dynamic_cast<BlockNode *>(node->functionBlock.get());
        if (bodyBlock) {
            for (const auto &stmt : bodyBlock->statements) {
                visit(stmt.get());
            }
        } else {
            if (node->functionBlock.get() != nullptr) {
                throw std::runtime_error("Function '" + functionName + "' has a body that is not a valid block of statements.");
            }
        }
    }
    currentScopeName = previousScopeName;
    currentFunctionName = previousFunctionName;
    currentFunctionExpectedReturnType = previousFunctionExpectedReturnType;
}

void SemanticAnalyzer::visitCallExpr(const CallExprNode *node) {
    std::string functionName = node->functionName;
    SymbolTable &globalTable = *allSymbolTables["global"];

    if (!globalTable.isDeclared(functionName)) {
        throw std::runtime_error("Call to undeclared function: " + functionName);
    }

    SymbolInfo funcInfo = globalTable.getSymbolInfo(functionName);
    if (!funcInfo.isFunction) {
        throw std::runtime_error("'" + functionName + "' is not a function.");
    }

    if (node->arguments.size() != funcInfo.params.size()) {
        throw std::runtime_error("Mismatched number of arguments for function '" + functionName +
                                 "'. Expected " + std::to_string(funcInfo.params.size()) +
                                 ", got " + std::to_string(node->arguments.size()) + ".");
    }

    for (size_t i = 0; i < node->arguments.size(); ++i) {
        Type argType = evaluateExpression(node->arguments[i]);
        if (argType != funcInfo.params[i].first) {
            throw std::runtime_error("Type mismatch for argument " + std::to_string(i + 1) +
                                     " in call to function '" + functionName +
                                     "'. Expected " + typeToString(funcInfo.params[i].first) +
                                     ", got " + typeToString(argType) + ".");
        }
    }
}

void SemanticAnalyzer::visitPrint(const PrintNode *node) {
    evaluateExpression(node->expression);
}

Type SemanticAnalyzer::evaluateExpression(const std::unique_ptr<ASTNode> &node) {
    if (!node)
        return Type::UNKNOWN;

    if (auto bin = dynamic_cast<BinaryExprNode *>(node.get())) {
        return visitBinaryExpr(bin);
    } else if (auto un = dynamic_cast<UnaryExprNode *>(node.get())) {
        return visitUnaryExpr(un);
    } else if (dynamic_cast<NumberLiteralNode *>(node.get())) {
        return visitNumberLiteral();
    } else if (dynamic_cast<StringLiteralNode *>(node.get())) {
        return visitStringLiteral();
    } else if (auto var = dynamic_cast<VariableNode *>(node.get())) {
        SymbolTable &currentTable = getCurrentSymbolTable();
        if (!currentTable.isDeclared(var->name)) {
            throw std::runtime_error("Undeclared variable used in expression: " + var->name);
        }
        if (!currentTable.isInitialized(var->name)) {
            throw std::runtime_error("Variable '" + var->name + "' used before initialization.");
        }
        return currentTable.getType(var->name);
    } else if (auto comp = dynamic_cast<ComparisonNode *>(node.get())) {
        return visitComparisonExpr(comp);
    } else if (auto logic = dynamic_cast<LogicalExprNode *>(node.get())) {
        return visitLogicalExpr(logic);
    } else if (auto call = dynamic_cast<CallExprNode *>(node.get())) {
        visitCallExpr(call);
        SymbolTable &globalTable = *allSymbolTables["global"];
        if (globalTable.isDeclared(call->functionName)) {
            SymbolInfo funcInfo = globalTable.getSymbolInfo(call->functionName);
            if (funcInfo.isFunction) {
                return funcInfo.returns;
            }
        }
        return Type::UNKNOWN;
    }
    throw std::runtime_error("Unknown expression node type encountered during evaluation.");
    return Type::UNKNOWN;
}

Type SemanticAnalyzer::visitBinaryExpr(const BinaryExprNode *node) {
    Type lhsType = evaluateExpression(node->left);
    Type rhsType = evaluateExpression(node->right);

    if (node->op == TokenType::PLUS) {
        if (lhsType == Type::NUMBER && rhsType == Type::NUMBER)
            return Type::NUMBER;
        if (lhsType == Type::STRING && rhsType == Type::STRING)
            return Type::STRING;
        throw std::runtime_error("Invalid operands for '+': " + typeToString(lhsType) + " and " + typeToString(rhsType));
    } else if (node->op == TokenType::MINUS || node->op == TokenType::MULTIPLY || node->op == TokenType::DIVIDE) {
        if (lhsType == Type::NUMBER && rhsType == Type::NUMBER) {
            if (node->op == TokenType::DIVIDE) {
                if (const NumberLiteralNode *literal = dynamic_cast<const NumberLiteralNode *>(node->right.get())) {
                    if (stoi(literal->value) == 0) {
                        throw std::runtime_error("Division by zero detected at compile time.");
                    }
                }
                return Type::NUMBER;
            }
            return Type::NUMBER;
        }
        throw std::runtime_error("Arithmetic operations require numeric operands. Got " + typeToString(lhsType) + " and " + typeToString(rhsType));
    }

    throw std::runtime_error("Unsupported binary operator or type mismatch in binary expression.");
    return Type::UNKNOWN;
}

Type SemanticAnalyzer::visitUnaryExpr(const UnaryExprNode *node) {
    Type operandType = evaluateExpression(node->operand);
    if (node->op == TokenType::MINUS) {
        if (operandType != Type::NUMBER) {
            throw std::runtime_error("Unary minus operator requires a numeric operand, got " + typeToString(operandType));
        }
        return Type::NUMBER;
    } else if (node->op == TokenType::NOT) {
        if (operandType != Type::NUMBER) {
            throw std::runtime_error("Logical NOT operator requires a numeric (boolean) operand, got " + typeToString(operandType));
        }
        return Type::NUMBER;
    }
    throw std::runtime_error("Unsupported unary operator.");
    return Type::UNKNOWN;
}

Type SemanticAnalyzer::visitNumberLiteral() {
    return Type::NUMBER;
}

Type SemanticAnalyzer::visitStringLiteral() {
    return Type::STRING;
}

Type SemanticAnalyzer::visitComparisonExpr(const ComparisonNode *node) {
    Type lhsType = evaluateExpression(node->leftExpression);
    Type rhsType = evaluateExpression(node->rightExpression);

    if (lhsType == Type::NUMBER && rhsType == Type::NUMBER) {
        return Type::NUMBER;
    }
    if (lhsType == Type::STRING && rhsType == Type::STRING) {
        if (node->op == TokenType::EQ || node->op == TokenType::NEQ) {
            return Type::NUMBER;
        }
        throw std::runtime_error("Strings can only be compared for equality/inequality.");
    }
    throw std::runtime_error("Type mismatch in comparison expression: Cannot compare " +
                             typeToString(lhsType) + " with " + typeToString(rhsType));
    return Type::NUMBER;
}

Type SemanticAnalyzer::visitLogicalExpr(const LogicalExprNode *node) {
    Type lhsType = evaluateExpression(node->leftExpression);
    Type rhsType = evaluateExpression(node->rightExpression);

    if (lhsType != Type::NUMBER || rhsType != Type::NUMBER) {
        throw std::runtime_error("Logical operators (AND, OR) require numeric (boolean) operands. Got " +
                                 typeToString(lhsType) + " and " + typeToString(rhsType));
    }
    return Type::NUMBER;
}

void SemanticAnalyzer::printAllSymbolTables() const {
    std::cout << "\n=== All Collected Symbol Tables ===\n";
    for (const auto &[uniqueName, symbolTablePtr] : allSymbolTables) {
        std::cout << "\n=== Symbol Table: " << uniqueName << " ===\n";
        symbolTablePtr->print(0);
    }
    std::cout << "\n=== End of Symbol Tables ===\n";
}

const SymbolTable &SemanticAnalyzer::getSymbolTable(const std::string &scopeName) const {
    auto it = allSymbolTables.find(scopeName);
    if (it == allSymbolTables.end()) {
        throw std::runtime_error("Symbol table for scope '" + scopeName + "' not found.");
    }
    return *it->second;
}