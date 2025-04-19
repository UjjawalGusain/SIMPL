#pragma once
#include <iostream>
#include <string>
#include <token.hpp>
#include <cctype>

class Lexer {
public:
    Lexer(const std::string& source) {
        this->source = source;
        this->pos = 0;
        this->line = 0;
        this->col = 0;
    };

    Token getNextToken() {
        while (std::isspace(peek())) advance();

        int startLine = line;
        int startCol  = col;
        char current  = peek();

        if (current == '\0') {
            return Token(TokenType::END_OF_FILE, "\0", line, col);
        }
        if (std::isdigit(current)) {
            return tokenizeNumber(startLine, startCol);
        }
        if (std::isalpha(current)) {
            return tokenizeIdentifierOrKeyword(startLine, startCol);
        }
        if (current == '"') {
            return tokenizeString(startLine, startCol);
        }
        return tokenizeSymbol(startLine, startCol);
    }

private:
    std::string source;
    size_t      pos;
    int         line, col;

    char peek() {
        return pos < source.size() ? source[pos] : '\0';
    }

    void advance() {
        char c = peek();
        if (c == '\0') return;
        pos++;
        if (c == '\n') {
            line++;
            col = 0;
        } else {
            col++;
        }
    }

    Token tokenizeNumber(int startLine, int startCol) {
        std::string val;
        while (std::isdigit(peek())) {
            val.push_back(peek());
            advance();
        }

        /*
            If I want to extend the functionality to accomodate decimal numbers too,
            The code is here to be changed
        */
       
        while (std::isdigit(peek())) {
            val.push_back(peek());
            advance();
        }
        
        if (std::isalpha(peek()) || peek() == '.') {
            val.push_back(peek());
            advance();
            return Token(TokenType::UNKNOWN, val, startLine, startCol);
        }
        
        return Token(TokenType::NUMBER_LITERAL, val, startLine, startCol);
    }

    Token tokenizeIdentifierOrKeyword(int startLine, int startCol) {
        std::string val;
        while (std::isalnum(peek())) {
            val.push_back(peek());
            advance();
        }
        if (val == "number") {
            return Token(TokenType::NUMBER, val, startLine, startCol);
        } else if (val == "string") {
            return Token(TokenType::STRING, val, startLine, startCol);
        } else if (val == "if") {
            return Token(TokenType::IF, val, startLine, startCol);
        } else if (val == "else") {
            return Token(TokenType::ELSE, val, startLine, startCol);
        } else if (val == "while") {
            return Token(TokenType::WHILE, val, startLine, startCol);
        } else if (val == "func") {
            return Token(TokenType::FUNC, val, startLine, startCol);
        } else if (val == "return") {
            return Token(TokenType::RETURN, val, startLine, startCol);
        } else if (val == "print") {
            return Token(TokenType::PRINT, val, startLine, startCol);
        }
        // Otherwise it's an identifier
        return Token(TokenType::IDENTIFIER, val, startLine, startCol);
    }

    Token tokenizeString(int startLine, int startCol) {
        // skip opening quote
        advance();
        std::string val;
        while (peek() != '"' && peek() != '\0') {
            val.push_back(peek());
            advance();
        }
        if (peek() == '"') {
            advance(); // skip closing quote
            return Token(TokenType::STRING_LITERAL, val, startLine, startCol);
        }
        return Token(TokenType::UNKNOWN, val, startLine, startCol);
    }

    Token tokenizeSymbol(int startLine, int startCol) {
        char c = peek();
        if (c == '=') {
            advance();
            if (peek() == '=') { advance(); return Token(TokenType::EQ, "==", startLine, startCol); }
            return Token(TokenType::ASSIGN, "=", startLine, startCol);
        }
        if (c == '!') {
            advance();
            if (peek() == '=') { advance(); return Token(TokenType::NEQ, "!=", startLine, startCol); }
            return Token(TokenType::NOT, "!", startLine, startCol);
        }
        if (c == '<') {
            advance();
            if (peek() == '=') { advance(); return Token(TokenType::LEQ, "<=", startLine, startCol); }
            return Token(TokenType::LT, "<", startLine, startCol);
        }
        if (c == '>') {
            advance();
            if (peek() == '=') { advance(); return Token(TokenType::GEQ, ">=", startLine, startCol); }
            return Token(TokenType::GT, ">", startLine, startCol);
        }
        if (c == '&') {
            advance();
            if (peek() == '&') { advance(); return Token(TokenType::AND, "&&", startLine, startCol); }
            return Token(TokenType::UNKNOWN, "&", startLine, startCol);
        }
        if (c == '|') {
            advance();
            if (peek() == '|') { advance(); return Token(TokenType::OR, "||", startLine, startCol); }
            return Token(TokenType::UNKNOWN, "|", startLine, startCol);
        }

        // Single-char tokens
        std::string single(1, c);
        switch (c) {
            case '(': advance(); return Token(TokenType::LPAREN, single, startLine, startCol);
            case ')': advance(); return Token(TokenType::RPAREN, single, startLine, startCol);
            case '{': advance(); return Token(TokenType::LBRACE, single, startLine, startCol);
            case '}': advance(); return Token(TokenType::RBRACE, single, startLine, startCol);
            case '+': advance(); return Token(TokenType::PLUS, single, startLine, startCol);
            case '-': advance(); return Token(TokenType::MINUS, single, startLine, startCol);
            case '*': advance(); return Token(TokenType::MULTIPLY, single, startLine, startCol);
            case '/': advance(); return Token(TokenType::DIVIDE, single, startLine, startCol);
            case ',': advance(); return Token(TokenType::COMMA, single, startLine, startCol);
            case ';': advance(); return Token(TokenType::SEMICOLON, single, startLine, startCol);
            default:
                advance();
                return Token(TokenType::UNKNOWN, single, startLine, startCol);
        }
    }
};
