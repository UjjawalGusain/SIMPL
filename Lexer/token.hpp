#pragma once
#include<iostream>

enum class TokenType {
    LET, IF, ELSE, WHILE, FUNC, RETURN, PRINT,
    IDENTIFIER, NUMBER, STRING,
    ASSIGN, PLUS, MINUS, LPAREN, RPAREN, LBRACE, RBRACE, COMMA, SEMICOLON,
    END_OF_FILE, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line, column;
};