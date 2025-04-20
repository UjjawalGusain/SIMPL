#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer/lexer.hpp"
#include "Parser/parser.hpp" 
#include "Parser/parser.cpp" 
#include "Parser/astPrinter.hpp"

int main(int argc, char **argv) {
    if(argc == 1) {
        std::cerr << "Enter filename also\n";
        return 1;
    } else if(argc > 2) {
        std::cerr << "Too many arguments\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open " << filename << "\n";
        return 1;
    }

    // Read entire file content
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string code = buffer.str();

    // Run lexer
    Lexer lexer(code);
    std::vector<Token> tokens;
    Token token;

    do {
        token = lexer.getNextToken();
        tokens.push_back(token);
    } while(token.type != TokenType::END_OF_FILE);

    // 🔸 Print Tokens
    std::cout << "\n--- Tokens ---\n";
    for (const Token& t : tokens) {
        std::cout << "Token(" << static_cast<int>(t.type) << ", \"" << t.value << "\", line: " << t.line << ", col: " << t.column << ")\n";
    }

    // Run parser
    Parser parser(tokens);
    std::unique_ptr<ASTNode> root = parser.parseProgram(); 

    std::cout << "\n--- AST ---\n";
    if (root) {
        std::cout << "Parsing completed successfully!\n";
        printAST(root.get()); 
    } else {
        std::cerr << "Parsing failed.\n";
    }

    return 0;
}
