/*

(* Top‐level *)
Program         ::= { Statement } END_OF_FILE

(* Statements *)
Statement       ::= Declaration
                  | Assignment
                  | IfStatement
                  | WhileLoop
                  | FunctionDecl
                  | PrintStatement
                  | Block
                  | ReturnStatement

ReturnStatement ::= RETURN Expression SEMICOLON

(* Variable Declaration *)
Declaration     ::= ( NUMBER | STRING ) VarList SEMICOLON
VarList         ::= ( IDENTIFIER [ ASSIGN Expression ] )
                  { COMMA IDENTIFIER [ ASSIGN Expression ] }

(* Assignment *)
Assignment      ::= IDENTIFIER ASSIGN Expression SEMICOLON

(* If / Elif / Else *)
IfStatement     ::= IF LPAREN Expression RPAREN Statement
                    { ELIF LPAREN Expression RPAREN Statement }
                    [ ELSE Statement ]

(* While Loop *)
WhileLoop       ::= WHILE LPAREN Expression RPAREN Statement

(* Function Declaration *)
FunctionDecl    ::= FUNC IDENTIFIER LPAREN [ ParameterList ] RPAREN Block
ParameterList   ::= ( (NUMBER | STRING) IDENTIFIER )
                    { COMMA (NUMBER | STRING) IDENTIFIER }

(* Print *)
PrintStatement  ::= PRINT LPAREN Expression RPAREN SEMICOLON

(* Block *)
Block           ::= LBRACE { Statement } RBRACE

(* Expressions with full BODMAS + logical operators *)
Expression      ::= LogicalOr

LogicalOr       ::= LogicalAnd { OR LogicalAnd }
LogicalAnd      ::= Equality   { AND Equality }
Equality        ::= Relational { ( EQ | NEQ ) Relational }
Relational      ::= Additive   { ( LT | GT | LEQ | GEQ ) Additive }
Additive        ::= Term       { ( PLUS | MINUS ) Term }
Term            ::= Factor     { ( MULTIPLY | DIVIDE ) Factor }
Factor          ::= [ NOT | MINUS ] Primary

Primary         ::= IDENTIFIER
                  | NUMBER_LITERAL
                  | STRING_LITERAL
                  | LPAREN Expression RPAREN
                  | CallExpression

(* Function Call *)
CallExpression  ::= IDENTIFIER LPAREN [ ArgumentList ] RPAREN
ArgumentList    ::= Expression { COMMA Expression }

(* Lexical tokens – just for clarity, these are terminals *)
NUMBER          ::= 'number'
STRING          ::= 'string'
IDENTIFIER      ::= Letter { Letter | Digit | '_' }
NUMBER_LITERAL  ::= Digit { Digit }
STRING_LITERAL  ::= '"' { Character } '"'

LPAREN          ::= '('
RPAREN          ::= ')'
LBRACE          ::= '{'
RBRACE          ::= '}'
COMMA           ::= ','
SEMICOLON       ::= ';'

ASSIGN          ::= '='
PLUS            ::= '+'
MINUS           ::= '-'
MULTIPLY        ::= '*'
DIVIDE          ::= '/'

EQ              ::= '=='
NEQ             ::= '!='
LT              ::= '<'
GT              ::= '>'
LEQ             ::= '<='
GEQ             ::= '>='

AND             ::= '&&'
OR              ::= '||'
NOT             ::= '!'

IF              ::= 'if'
ELIF            ::= 'elif'
ELSE            ::= 'else'
WHILE           ::= 'while'
FUNC            ::= 'func'
RETURN          ::= 'return'
PRINT           ::= 'print'

END_OF_FILE     ::= ↵  


*/

#include<iostream>
#include<Lexer/lexer.hpp>
#include<vector>
#include <memory>




class Parser {
    std::vector<Token> tokens;
    size_t current = 0;

    Token peek() { return tokens[current]; }
    Token advance() { return tokens[current++]; }
    bool match(TokenType type) {
        if (peek().type == type) {
            advance();
            return true;
        }
        return false;
    }

    /*
    Statement       ::= Declaration
                  | Assignment
                  | IfStatement
                  | WhileLoop
                  | FunctionDecl
                  | PrintStatement
                  | Block
                  | ReturnStatement
    
    */

    void parseStatement() {
        if (match(TokenType::NUMBER)) {
            parseDeclaration();  
        } else if (match(TokenType::IDENTIFIER)) {
            if (match(TokenType::EQUAL)) {
                parseAssignment();  
            } else {
                parseFunctionCall();  
            }
        } else if (match(TokenType::IF)) {
            parseIfStatement();  
        } else if (match(TokenType::WHILE)) {
            parseWhileLoop();  
        } else if (match(TokenType::FUNC)) {
            parseFunctionDecl();  
        } else if (match(TokenType::PRINT)) {
            parsePrintStatement(); 
        } else if (match(TokenType::RETURN)) {
            parseReturnStatement(); 
        } else if (match(TokenType::LBRACE)) {
            parseBlock();  
        } else {
            // Error handling if the token does not match any expected statements
            std::cerr << "Error in line " << peek().line << " and col " << peek().column << "in datatype: " << peek().type << "\n";
        }
    }


    /* ReturnStatement ::= RETURN Expression SEMICOLON */
    void parseReturnStatement() {
        if(!match(TokenType::RETURN)) {
            std::cerr << "Error in line " << peek().line << " and col " << peek().column << "in datatype: " << peek().type << "\n";
        }

        parseExpression();

        if(!match(TokenType::SEMICOLON)) {
            std::cerr << "Error in line " << peek().line << " and col " << peek().column << "in datatype: " << peek().type << "\n";
        }
    }


    /*
    (* Variable Declaration *)
    Declaration     ::= ( NUMBER | STRING ) VarList SEMICOLON
    VarList         ::= ( IDENTIFIER [ ASSIGN Expression ] )
                      { COMMA IDENTIFIER [ ASSIGN Expression ] }
    */

    void parseDeclaration() {

        if(match(TokenType::NUMBER)) {

        } else if(match(TokenType::STRING)) {

        } else {
            std::cerr << "Error in line " << peek().line << " and col " << peek().column << "in datatype: " << peek().type << "\n";
        }

        parseVarList();

        if(!match(TokenType::SEMICOLON)) {
            std::cerr << "Error in line " << peek().line << " and col " << peek().column << "in datatype: " << peek().type << "\n";
        }

    }

    void parseVarList() {

    }


    

public:
    Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

    // Program ::= { Statement } END_OF_FILE
    int parseProgram() {
        while (peek().type != TokenType::END_OF_FILE) {
            parseStatement();  // Parse each statement
        }

        if (peek().type == TokenType::END_OF_FILE) {
            return 1; // Success
        } else {
            // Error handling: unexpected token
            std::cerr << "Error: Expected END_OF_FILE, got " << peek() << "\n";
            return 0; // Failure
        }
    }
};
