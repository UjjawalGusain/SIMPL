#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer/lexer.hpp"

int main() {
    std::ifstream inputFile("test_file.simpl");

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open test_file.simpl\n";
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
