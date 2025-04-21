#pragma once
#include<iostream>

enum class TokenType {
    IF, ELIF, ELSE,
    WHILE, FUNC, RETURN, PRINT,
    IDENTIFIER, NUMBER, STRING,
    NUMBER_LITERAL, STRING_LITERAL,
    ASSIGN, PLUS, MINUS, DIVIDE, MULTIPLY, LPAREN, RPAREN, LBRACE, RBRACE, COMMA, SEMICOLON, 
    END_OF_FILE, UNKNOWN,
    EQ, NEQ, LT, GT, LEQ, GEQ, AND, OR, NOT
};

inline std::ostream& operator<<(std::ostream& os, TokenType type) {
    switch (type) {
        case TokenType::IF: return os << "IF";
        case TokenType::ELIF: return os << "ELIF";
        case TokenType::ELSE: return os << "ELSE";
        case TokenType::WHILE: return os << "WHILE";
        case TokenType::FUNC: return os << "FUNC";
        case TokenType::RETURN: return os << "RETURN";
        case TokenType::PRINT: return os << "PRINT";
        case TokenType::IDENTIFIER: return os << "IDENTIFIER";
        case TokenType::NUMBER: return os << "NUMBER";
        case TokenType::STRING: return os << "STRING";
        case TokenType::NUMBER_LITERAL: return os << "NUMBER_LITERAL";
        case TokenType::STRING_LITERAL: return os << "STRING_LITERAL";
        case TokenType::ASSIGN: return os << "ASSIGN";
        case TokenType::PLUS: return os << "PLUS";
        case TokenType::MINUS: return os << "MINUS";
        case TokenType::DIVIDE: return os << "DIVIDE";
        case TokenType::MULTIPLY: return os << "MULTIPLY";
        case TokenType::LPAREN: return os << "LPAREN";
        case TokenType::RPAREN: return os << "RPAREN";
        case TokenType::LBRACE: return os << "LBRACE";
        case TokenType::RBRACE: return os << "RBRACE";
        case TokenType::COMMA: return os << "COMMA";
        case TokenType::SEMICOLON: return os << "SEMICOLON";
        case TokenType::END_OF_FILE: return os << "END_OF_FILE";
        case TokenType::UNKNOWN: return os << "UNKNOWN";
        case TokenType::EQ: return os << "EQ";
        case TokenType::NEQ: return os << "NEQ";
        case TokenType::LT: return os << "LT";
        case TokenType::GT: return os << "GT";
        case TokenType::LEQ: return os << "LEQ";
        case TokenType::GEQ: return os << "GEQ";
        case TokenType::AND: return os << "AND";
        case TokenType::OR: return os << "OR";
        case TokenType::NOT: return os << "NOT";
        default: return os << "UNRECOGNIZED_TOKEN";
    }
}


struct Token {
    TokenType type;
    std::string value;
    int line, column;

    Token() = default;

    Token(TokenType type, std::string value, int line, int column) {
        this -> type = type;
        this -> value = value;
        this -> line = line;
        this -> column = column;
    }
};