#pragma once
#include<iostream>
#include<../Lexer/lexer.hpp>
#include<../Parser/ast.hpp>
#include<vector>
#include <memory>


class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;
    Token peek();
    Token advance();
    Token previous();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseReturnStatement();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::vector<std::unique_ptr<VarDeclareNode>> parseVarList(TokenType variableType);
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseWhileLoop();
    std::vector<std::pair<std::string, std::string>> parseParameterList();
    std::unique_ptr<ASTNode> parseFunctionDecl();
    std::unique_ptr<ASTNode> parsePrintStatement();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseRelational();
    std::unique_ptr<ASTNode> parseAdditive();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseCallExpression(Token &functionToken);
    std::vector<std::unique_ptr<ASTNode>> parseArgumentList();
    std::unique_ptr<VarDeclareNode> parseNumber();
    std::unique_ptr<VarDeclareNode> parseString();
    std::unique_ptr<ASTNode> parseIdentifier();
    std::unique_ptr<ASTNode> parseNumberLiteral();
    std::unique_ptr<ASTNode> parseStringLiteral();

    void reportError(const std::string& message);
    bool match(TokenType type);

public:
    std::unique_ptr<BlockNode> parseProgram();

    Parser(std::vector<Token> tokens);

};

