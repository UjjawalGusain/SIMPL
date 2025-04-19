#pragma once
#include<iostream>
#include<memory>
#include<vector>
#include<string>
#include<Lexer/token.hpp>

class ASTNode {
    public:
        virtual ~ASTNode() = default;
};

class NumberLiteralNode : public ASTNode {
    public:
        int value;
        int line, col;
        NumberLiteralNode(int value, int line, int col) : value(value), line(line), col(col) {}
};

class StringLiteralNode : public ASTNode {
    public:
        std::string value;
        int line, col;
        StringLiteralNode(std::string value, int line, int col) : value(value), line(line), col(col) {}
};

class IfStatementNode : public ASTNode {
    public:
        int line, col;

        std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> conditionBlocks;
        std::unique_ptr<ASTNode> elseBranch;

        IfStatementNode(
            std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> conditionBlocks,
            std::unique_ptr<ASTNode> elseBranch,
            int line, int col
        ) : conditionBlocks(std::move(conditionBlocks)),
            elseBranch(std::move(elseBranch)),
            line(line), col(col) {}
};

class ReturnNode : public ASTNode {
    public:
        int line, col;
        std::unique_ptr<ASTNode> returnExpression;
    
        ReturnNode(
            std::unique_ptr<ASTNode> returnExpression, int line, int col
        ) : returnExpression(std::move(returnExpression)), line(line), col(col) {}
};

class ComparisonNode : public ASTNode {
    public:
        int line, col;
        std::unique_ptr<ASTNode> leftExpression, rightExpression;
        TokenType op;

        ComparisonNode(
            std::unique_ptr<ASTNode> leftExpression, std::unique_ptr<ASTNode> rightExpression,
            TokenType op, int line, int col
        ) : leftExpression(std::move(leftExpression)), rightExpression(std::move(rightExpression)), op(op), line(line), col(col) {}
    
};



class LogicalExprNode : public ASTNode {
    public:
        int line, col;
        std::unique_ptr<ASTNode> leftExpression, rightExpression;
        TokenType op;

        LogicalExprNode(
            std::unique_ptr<ASTNode> leftExpression, std::unique_ptr<ASTNode> rightExpression,
            TokenType op, int line, int col
        ) : leftExpression(std::move(leftExpression)), rightExpression(std::move(rightExpression)), op(op), line(line), col(col) {}
};

class AssignmentNode : public ASTNode {
    public:
        int line, col;
        std::unique_ptr<ASTNode> left, rightExpression;

        AssignmentNode(
            std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> rightExpression, int line, int col
        ) : left(std::move(left)), rightExpression(std::move(rightExpression)), line(line), col(col) {}
};

class VariableNode : public ASTNode {
    public:
        std::string name;
        int line, col;
    
        VariableNode(const std::string& name, int line, int col)
            : name(name), line(line), col(col) {}
};

class PrintNode : public ASTNode {
    public:
        std::unique_ptr<ASTNode> expression;
        int line, col;
    
        PrintNode(std::unique_ptr<ASTNode> expression, int line, int col)
            : expression(std::move(expression)), line(line), col(col) {}
};

class FunctionNode : public ASTNode {
    public:
        std::string name;
        int line, col;
        std::vector<std::pair<std::string, std::string>> parameters;
        std::unique_ptr<ASTNode> functionBlock;

        FunctionNode(
            const std::string& name,
            std::vector<std::pair<std::string, std::string>> parameters,
            std::unique_ptr<ASTNode> functionBlock,
            int line, int col
        ) : name(name),
            parameters(std::move(parameters)),
            functionBlock(std::move(functionBlock)),
            line(line), col(col) {}

};


class WhileNode : public ASTNode {
    public:
        int line, col;
        std::unique_ptr<ASTNode> conditionStatement, whileBlock;

        WhileNode(
            std::unique_ptr<ASTNode> conditionStatement,
            std::unique_ptr<ASTNode> whileBlock,
            int line, int col
        ) : conditionStatement(std::move(conditionStatement)),
            whileBlock(std::move(whileBlock)),
            line(line), col(col) {}
};
    
class BlockNode : public ASTNode {
    public:
        std::vector<std::unique_ptr<ASTNode>> statements;
        int line, col;
    
        BlockNode(std::vector<std::unique_ptr<ASTNode>> statements, int line, int col)
            : statements(std::move(statements)), line(line), col(col) {}
};


