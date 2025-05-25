#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer/lexer.hpp"
#include "Parser/parser.hpp" 
#include "Parser/astPrinter.hpp"
#include "Semantic_analyzer/include/semantic_analyzer.hpp"
// #include "IR/ir_generator.hpp"   // For future to add ir
// #include "Interpreter/interpreter.hpp"   // For future to add interpreter

void printSymbolTable(const std::unordered_map<std::string, SymbolInfo>& symbolTable) { 
    std::cout << "Symbol Table:\n";
    for (const auto& [varName, info] : symbolTable) {
        std::string typeStr;
        switch (info.type) {
            case Type::NUMBER: typeStr = "int"; break;
            case Type::STRING: typeStr = "string"; break;
            default: typeStr = "unknown"; break;
        }
        std::cout << "  " << varName << " -> Type: " << typeStr
                  << ", Initialized: " << (info.isInitialized ? "Yes" : "No") << "\n";
    }
    std::cout << std::endl;
}



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

    //  Print Tokens
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

    try {
        SemanticAnalyzer semanticAnalyzer;
        semanticAnalyzer.analyze(root.get());  // Perform semantic checks
        std::cout << "\n--- Semantic Analysis ---\n";
        std::cout << "Semantic analysis completed successfully!\n";

        semanticAnalyzer.getSymbolTable().print();
        semanticAnalyzer.printFunctionTable();

        // const std::unordered_map<std::string, SymbolInfo> symbolTable = semanticAnalyzer.getSymbolTable().getScope();
        // printSymbolTable(symbolTable);
    } catch (const std::runtime_error& e) {
        std::cerr << "Semantic error: " << e.what() << "\n";
        return 1;
    }

    // // Step 1: Generate Intermediate Representation (IR)
    // IRGenerator irGenerator;           // Create an instance of IRGenerator
    // IR ir = irGenerator.generate(root.get());  // Generate IR from the AST

    // std::cout << "\n--- Intermediate Representation (IR) ---\n";
    // // Print out the IR instructions
    // for (const auto& instruction : ir) {
    //     std::cout << instruction.op << " " << instruction.arg1 << " " << instruction.arg2 << " -> " << instruction.result << "\n";
    // }

    // // Step 2: Execute IR using Interpreter
    // Interpreter interpreter;           // Create an instance of the Interpreter
    // interpreter.execute(ir);          // Execute the IR

    return 0;
}

