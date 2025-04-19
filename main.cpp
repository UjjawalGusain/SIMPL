#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer/lexer.hpp"

int main(int argc, char **argv) {
    if(argc == 1) {
        std::cerr << "Enter filename also\n";
    } else if(argc > 2) {
        std::cerr << "Too many arguments\n";
    }
    std::string filename = argv[1];
    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        std::cerr << "Failed to " << filename << "\n";
        return 1;
    }

    // Read entire file content
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string code = buffer.str();

    Lexer lexer(code);
    Token token;

    while ((token = lexer.getNextToken()).type != TokenType::END_OF_FILE) {
        std::cout << "Token(" << token.type << ", \"" 
                  << token.value << "\", line=" << token.line 
                  << ", col=" << token.column << ")" << "\n";
    }

    return 0;
}
