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
#include<Parser/ast.hpp>
#include<vector>
#include <memory>




class Parser {
    std::vector<Token> tokens;
    size_t current = 0;

    Token peek() { return tokens[current]; }
    Token advance() { return tokens[current++]; }
    Token previous() {
        return current!=0 ? tokens[current-1] : Token(TokenType::UNKNOWN, "", -1, -1);
    }

    void reportError(const std::string& message) {
        const Token& token = peek();
        std::cerr << "Parse Error [line " << token.line 
                  << ", col " << token.column << "]: " 
                  << message << "\n" << " (Token: " << token.type << ")\n";
    }

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

    std::unique_ptr<ASTNode> parseStatement() {
        if (match(TokenType::NUMBER)) {
            return parseDeclaration();  
        } else if (match(TokenType::STRING)) {
            return parseDeclaration();  
        } else if (match(TokenType::IDENTIFIER)) {
            return parseAssignment();
        } else if (match(TokenType::IF)) {
            return parseIfStatement();  
        } else if (match(TokenType::WHILE)) {
            return parseWhileLoop();  
        } else if (match(TokenType::FUNC)) {
            return parseFunctionDecl();  
        } else if (match(TokenType::PRINT)) {
            return parsePrintStatement(); 
        } else if (match(TokenType::RETURN)) {
            return parseReturnStatement(); 
        } else if (match(TokenType::LBRACE)) {
            return parseBlock();  
        } else {
            // Error handling if the token does not match any expected statements
            reportError("Unexpeced token in statement");
            return NULL;
        }
    }


    /* ReturnStatement ::= RETURN Expression SEMICOLON */
    std::unique_ptr<ASTNode> parseReturnStatement() {
        if(!match(TokenType::RETURN)) {
            reportError("return keyword not found");
            return NULL;
        }

        std::unique_ptr<ASTNode> parsedExpression = parseExpression();

        if(!match(TokenType::SEMICOLON)) {
            reportError("; not found");
            return NULL;
        }
        return parsedExpression;
    }


    /*
    (* Variable Declaration *)
    Declaration     ::= ( NUMBER | STRING ) VarList SEMICOLON
    VarList         ::= ( IDENTIFIER [ ASSIGN Expression ] )
                      { COMMA IDENTIFIER [ ASSIGN Expression ] }
    */

    std::unique_ptr<ASTNode> parseDeclaration() {

        TokenType variableType;

        if(match(TokenType::NUMBER)) {
            variableType = TokenType::NUMBER;
        } else if(match(TokenType::STRING)) {
            variableType = TokenType::STRING;
        } else {
            reportError("Unknown data type found. Expected 'number' or 'string'");
            return NULL;
        }

        std::vector<std::unique_ptr<VarDeclareNode>> declarations = parseVarList(variableType);

        if(!match(TokenType::SEMICOLON)) {
            reportError("; not found");
            return NULL;
        }

        return std::make_unique<DeclarationNode>(variableType, std::move(declarations));
    }

    std::vector<std::unique_ptr<VarDeclareNode>> parseVarList(TokenType variableType) {
        
        std::vector<std::unique_ptr<VarDeclareNode>> varDeclareList;
        if(!match(TokenType::IDENTIFIER)) {
            reportError("Invalid identifier");
            return std::vector<std::unique_ptr<VarDeclareNode>>();
        }

        std::string varName = previous().value;
        std::unique_ptr<ASTNode> initExpr = NULL;

        if(match(TokenType::ASSIGN)) {
            initExpr = parseExpession();
        }
        varDeclareList.push_back(std::make_unique<VarDeclareNode>(variableType, varName, std::move(initExpr)));

        while(match(TokenType::COMMA)) {
            if(!match(TokenType::IDENTIFIER)) {
                reportError("Invalid identifier");
                break;
            }


            std::unique_ptr<ASTNode> additionalExprs = NULL;
            if(match(TokenType::ASSIGN)) {
                additionalExprs = parseExpression();
            }

            std::string varName = previous().value;

            varDeclareList.push_back(std::make_unique<VarDeclareNode>(variableType, varName, std::move(additionalExprs)));
        }
        return varDeclareList;

    }

    /*
    (* Expressions with full BODMAS + logical operators *)
    Expression      ::= LogicalOr

    LogicalOr       ::= LogicalAnd { OR LogicalAnd }
    LogicalAnd      ::= Equality   { AND Equality }
    Equality        ::= Relational { ( EQ | NEQ ) Relational }
    Relational      ::= Additive   { ( LT | GT | LEQ | GEQ ) Additive }
    Additive        ::= Term       { ( PLUS | MINUS ) Term }
    Term            ::= Factor     { ( MULTIPLY | DIVIDE ) Factor }
    Factor          ::= [ NOT | MINUS ] Primary
    */

    std::unique_ptr<ASTNode> parseExpession() {
        return parseLogicalOr();
    }

    std::unique_ptr<ASTNode> parseLogicalOr() {
        auto left = parseLogicalAnd();
    
        while (match(TokenType::OR)) {
            Token opToken = previous();
            auto right = parseLogicalAnd();
    
            left = std::make_unique<LogicalExprNode>(
                std::move(left),
                std::move(right),
                TokenType::OR,
                opToken.line,
                opToken.column
            );
        }
    
        return left;
    }

    std::unique_ptr<ASTNode> parseLogicalAnd() {
        auto left = parseEquality();

        while (match(TokenType::AND)) {
            Token opToken = previous();
            auto right = parseEquality();
    
            left = std::make_unique<LogicalExprNode>(
                std::move(left),
                std::move(right),
                TokenType::AND,
                opToken.line,
                opToken.column
            );
        }
    
        return left;
    }
    
    std::unique_ptr<ASTNode> parseEquality() {
        auto left = parseRelational();

        while(match(TokenType::EQ) || match(TokenType::NEQ)) {
            Token opToken = previous();

            auto right = parseRelational();            
            left = std::make_unique<ComparisonNode>(
                std::move(left),
                std::move(right),
                opToken.type,
                opToken.line,
                opToken.column
            );
        }

        return left;
    }

    std::unique_ptr<ASTNode> parseRelational() {
        auto left = parseAdditive();

        while(match(TokenType::LT) || match(TokenType::GT) || match(TokenType::LEQ) || match(TokenType::GEQ)) {
            Token opToken = previous();

            auto right = parseAdditive();            
            left = std::make_unique<ComparisonNode>(
                std::move(left),
                std::move(right),
                opToken.type,
                opToken.line,
                opToken.column
            );

        }

        return left;
    }

    std::unique_ptr<ASTNode> parseAdditive() {
        auto left = parseTerm();

        while(match(TokenType::PLUS) || match(TokenType::MINUS)) {
            Token opToken = previous();

            auto right = parseTerm();            
            left = std::make_unique<ComparisonNode>(
                std::move(left),
                std::move(right),
                opToken.type,
                opToken.line,
                opToken.column
            );
        }

        return left;
    }

    

public:
    Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

    // Program ::= { Statement } END_OF_FILE
    std::unique_ptr<BlockNode> parseProgram() {
        std::vector<std::unique_ptr<ASTNode>> statements;
        while (!match(TokenType::END_OF_FILE)) {
            statements.push_back(parseStatement());
        }
        return std::make_unique<BlockNode>(std::move(statements), 0, 0); // because the program simply starts from first line and first column
    }
};

