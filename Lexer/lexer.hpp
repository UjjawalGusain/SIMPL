#pragma once
#include<iostream>
#include<string>
#include<token.hpp>

class Lexer {
    public:
        Lexer(const std::string& source) {
            this -> source = source;
            this -> line = 0;
            this -> col = 0;
        };
        Token getNextToken() {

        };
    private:
        std::string source;
        size_t pos;
        int line, col;
    
        char peek();
        void advance();
        Token tokenizeIdentifierOrKeyword();
        Token tokenizeNumber();
        Token tokenizeString();
        Token tokenizeSymbol();
    };