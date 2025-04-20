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
#include<../Lexer/lexer.hpp>
#include<../Parser/ast.hpp>
#include<vector>
#include <memory>




class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;

    Token peek() { return tokens[current]; }
    Token advance() { return tokens[current++]; }
    Token previous() {
        return current!=0 ? tokens[current-1] : Token(TokenType::UNKNOWN, "", -1, -1);
    }

    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseReturnStatement();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::vector<std::unique_ptr<VarDeclareNode>> parseVarList(TokenType variableType);
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseWhileLoop();
    std::vector<std::pair<std::string, std::string>> parseParameterList();
    std::unique_ptr<ASTNode> parseFunctionDecl();
    std::unique_ptr<ASTNode> parsePrintStatement();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseRelational();
    std::unique_ptr<ASTNode> parseAdditive();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseCallExpression(Token &functionToken);
    std::vector<std::unique_ptr<ASTNode>> parseArgumentList();
    std::unique_ptr<VarDeclareNode> parseNumber();
    std::unique_ptr<VarDeclareNode> parseString();
    std::unique_ptr<ASTNode> parseIdentifier();
    std::unique_ptr<ASTNode> parseNumberLiteral();
    std::unique_ptr<ASTNode> parseStringLiteral();

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
            initExpr = parseExpression();
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
        (* Assignment *)
    Assignment      ::= IDENTIFIER ASSIGN Expression SEMICOLON

    */

    std::unique_ptr<ASTNode> parseAssignment() {
        Token idTok = previous();  
    
        if (!match(TokenType::EQ)) {
            reportError("= symbol not found");
            return nullptr;
        }
    
        std::unique_ptr<ASTNode> rightExpression = parseExpression();
    
        if (!match(TokenType::SEMICOLON)) {
            reportError("; symbol not found after assignment");
            return nullptr;
        }
    
        return std::make_unique<AssignmentNode>(
            std::make_unique<VariableNode>(idTok.value, idTok.line, idTok.column),
            std::move(rightExpression),
            idTok.line,
            idTok.column
        );
    }

    /*

    (* If / Elif / Else *)
    IfStatement     ::= IF LPAREN Expression RPAREN Statement
                        { ELIF LPAREN Expression RPAREN Statement }
                        [ ELSE Statement ]
 
    */

    std::unique_ptr<ASTNode> parseIfStatement() {
        Token ifToken = previous();  
    
        std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> conditionBlocks;
    
        std::unique_ptr<ASTNode> condition = parseExpression();
        if (!match(TokenType::LBRACE)) {
            reportError("Expected '{' after if condition");
            return NULL;
        }
        std::unique_ptr<ASTNode> ifBlock = parseBlock();
        conditionBlocks.push_back({std::move(condition), std::move(ifBlock)});
    
        // For elif blocks
        while (match(TokenType::ELIF)) {
            std::unique_ptr<ASTNode> elifCondition = parseExpression();
            if (!match(TokenType::LBRACE)) {
                reportError("Expected '{' after elif condition");
                return NULL;
            }
            std::unique_ptr<ASTNode> elifBlock = parseBlock();
            conditionBlocks.push_back({std::move(elifCondition), std::move(elifBlock)});
        }
    
        // For else block
        std::unique_ptr<ASTNode> elseBranch = nullptr;
        if (match(TokenType::ELSE)) {
            if (!match(TokenType::LBRACE)) {
                reportError("Expected '{' after else");
                return NULL;
            }
            elseBranch = parseBlock();
        }
    
        return std::make_unique<IfStatementNode>(
            std::move(conditionBlocks),
            std::move(elseBranch),
            ifToken.line,
            ifToken.column
        );
    }


    /*
    WhileLoop       ::= WHILE LPAREN Expression RPAREN Statement
    */

    std::unique_ptr<ASTNode> parseWhileLoop() {
        Token whileTok = previous();  
    
        if (!match(TokenType::LPAREN)) {
            reportError("Expected '(' after 'while'");
            return nullptr;
        }
    
        auto conditionExpr = parseExpression();
    
        if (!match(TokenType::RPAREN)) {
            reportError("Expected ')' after while condition");
            return nullptr;
        }
    
        auto bodyStmt = parseStatement();
    
        return std::make_unique<WhileNode>(
            std::move(conditionExpr),
            std::move(bodyStmt),
            whileTok.line,
            whileTok.column
        );
    }

    
    /*
    (* Function Declaration *)
    FunctionDecl    ::= FUNC IDENTIFIER LPAREN [ ParameterList ] RPAREN Block
    ParameterList   ::= ( (NUMBER | STRING) IDENTIFIER )
                        { COMMA (NUMBER | STRING) IDENTIFIER }
    
    */

    std::vector<std::pair<std::string, std::string>> parseParameterList() {
        std::vector<std::pair<std::string, std::string>> params;
    
        TokenType ty;
        if      (match(TokenType::NUMBER)) ty = TokenType::NUMBER;
        else if (match(TokenType::STRING)) ty = TokenType::STRING;
        else {
            reportError("Expected parameter type 'number' or 'string'");
            return params;
        }
        Token typeTok = previous();
    
        if (!match(TokenType::IDENTIFIER)) {
            reportError("Expected parameter name after type");
            return params;
        }
        Token nameTok = previous();
    
        params.emplace_back(typeTok.value, nameTok.value);
    
        while (match(TokenType::COMMA)) {
            if      (match(TokenType::NUMBER)) ty = TokenType::NUMBER;
            else if (match(TokenType::STRING)) ty = TokenType::STRING;
            else {
                reportError("Expected parameter type after ','");
                break;
            }
            typeTok = previous();
    
            if (!match(TokenType::IDENTIFIER)) {
                reportError("Expected parameter name after type");
                break;
            }
            nameTok = previous();
    
            params.emplace_back(typeTok.value, nameTok.value);
        }
    
        return params;
    }
    
    std::unique_ptr<ASTNode> parseFunctionDecl() {
        Token funcTok = previous();  
    
        if (!match(TokenType::IDENTIFIER)) {
            reportError("Expected function name after 'func'");
            return nullptr;
        }
        Token nameTok = previous();
    
        if (!match(TokenType::LPAREN)) {
            reportError("Expected '(' after function name");
            return nullptr;
        }
    
        std::vector<std::pair<std::string, std::string>> params;
        if (peek().type != TokenType::RPAREN) {
            params = parseParameterList();
        }
    
        if (!match(TokenType::RPAREN)) {
            reportError("Expected ')' after parameters");
            return nullptr;
        }
    
        if (!match(TokenType::LBRACE)) {
            reportError("Expected '{' to start function body");
            return nullptr;
        }
        auto body = parseBlock();
    
        return std::make_unique<FunctionNode>(
            nameTok.value,
            std::move(params),
            std::move(body),
            funcTok.line,
            funcTok.column
        );
    }
    

    /*
    (* Print *)
    PrintStatement  ::= PRINT LPAREN Expression RPAREN SEMICOLON
    */

    std::unique_ptr<ASTNode> Parser::parsePrintStatement() {
        Token printTok = previous(); 
    
        if (!match(TokenType::LPAREN)) {
            reportError("Expected '(' after 'print'");
            return nullptr;
        }
    
        auto expr = parseExpression();
    
        if (!match(TokenType::RPAREN)) {
            reportError("Expected ')' after print expression");
            return nullptr;
        }
    
        if (!match(TokenType::SEMICOLON)) {
            reportError("Expected ';' after print statement");
            return nullptr;
        }
    
        return std::make_unique<PrintNode>(
            std::move(expr),
            printTok.line,
            printTok.column
        );
    }

    /*
    Block           ::= LBRACE { Statement } RBRACE
    */

    std::unique_ptr<ASTNode> parseBlock() {

        Token tok = previous();
        std::vector<std::unique_ptr<ASTNode>> statements;

        while(!match(TokenType::RBRACE)) {
            statements.push_back(parseStatement());
        }

        return std::make_unique<BlockNode> (
            std::move(statements),
            tok.line,
            tok.column
        );
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

    std::unique_ptr<ASTNode> parseExpression() {
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
            left = std::make_unique<BinaryExprNode>(
                std::move(left),
                std::move(right),
                opToken.type,
                opToken.line,
                opToken.column
            );
        }

        return left;
    }

    std::unique_ptr<ASTNode> parseTerm() {

        auto left = parseFactor();

        while(match(TokenType::MULTIPLY) || match(TokenType::DIVIDE)) {
            Token opToken = previous();
            auto right = parseTerm();            
            left = std::make_unique<BinaryExprNode>(
                std::move(left),
                std::move(right),
                opToken.type,
                opToken.line,
                opToken.column
            );
        }

        return left;
    }


    std::unique_ptr<ASTNode> parseFactor() {
        if (match(TokenType::NOT) || match(TokenType::MINUS)) {
            Token opToken = previous();
            auto operand = parseFactor();
            return std::make_unique<UnaryExprNode>(
                opToken.type,
                std::move(operand),
                opToken.line,
                opToken.column
            );
        }
    
        return parsePrimary();
    }

    /*
    Primary         ::= IDENTIFIER
                  | NUMBER_LITERAL
                  | STRING_LITERAL
                  | LPAREN Expression RPAREN
                  | CallExpression

    CallExpression  ::= IDENTIFIER LPAREN [ ArgumentList ] RPAREN

    ArgumentList    ::= Expression { COMMA Expression }
    */

    std::unique_ptr<ASTNode> parsePrimary() {
        if(match(TokenType::IDENTIFIER)) {
            
            if(peek().type == TokenType::LPAREN) {
                Token functionToken = previous();
                return parseCallExpression(functionToken);
            }
            return parseIdentifier();
        } 

        if(match(TokenType::NUMBER_LITERAL)) {
            return parseNumberLiteral();
        }

        if(match(TokenType::STRING_LITERAL)) {
            return parseStringLiteral();
        }

        if(match(TokenType::LPAREN)) {
            auto expression = parseExpression();
            if(!match(TokenType::RPAREN)) {
                reportError(") symbol not found");
                return NULL;
            }
            return expression;
        }

        reportError("Unexpected expression found: (, ), number_literal, string_literal allowed");
        return NULL;
    }    

    std::unique_ptr<ASTNode> parseCallExpression(Token &functionToken) {

        if(functionToken.type != TokenType::IDENTIFIER) {
            reportError("Missing identifier from function call");
            return NULL;
        }

        if(!match(TokenType::LPAREN)) {
            reportError("( missing");
            return NULL;
        }

        std::vector<std::unique_ptr<ASTNode>> argumentList;

        if (peek().type != TokenType::RPAREN) {
            argumentList = parseArgumentList();
        }

        if (!match(TokenType::RPAREN)) {
            reportError("Expected ) after function arguments");
            return NULL;
        }

        return std::make_unique<CallExprNode>(
            functionToken.value,     
            std::move(argumentList),      
            functionToken.line,
            functionToken.column
        );
    }


    std::vector<std::unique_ptr<ASTNode>> parseArgumentList() {
        std::vector<std::unique_ptr<ASTNode>> argumentList;
        auto argument1 = parseExpression();

        argumentList.push_back(argument1);

        while(match(TokenType::COMMA)) {
            auto nextArgument = parseExpression();
            argumentList.push_back(nextArgument);
        }

        return argumentList;
    }

    /*

    (* Lexical tokens - just for clarity, these are terminals *)
    NUMBER          ::= 'number'
    STRING          ::= 'string'
    */

    std::unique_ptr<VarDeclareNode> parseNumber() {
        Token typeToken = previous(); 
    
        if (!match(TokenType::IDENTIFIER)) {
            reportError("Expected identifier after 'number'");
            return NULL;
        }
    
        Token nameToken = previous();  
    
        std::unique_ptr<ASTNode> initializer = NULL;
        if (match(TokenType::EQ)) {
            initializer = parseExpression();
        }
    
        return std::make_unique<VarDeclareNode>(
            typeToken.type,
            nameToken,
            std::move(initializer)
        );
    }

    std::unique_ptr<VarDeclareNode> parseString() {
        Token typeToken = previous(); 
    
        if (!match(TokenType::IDENTIFIER)) {
            reportError("Expected identifier after 'string'");
            return NULL;
        }
    
        Token nameToken = previous();  
    
        std::unique_ptr<ASTNode> initializer = NULL;
        if (match(TokenType::EQ)) {
            initializer = parseExpression();
        }
    
        return std::make_unique<VarDeclareNode>(
            typeToken.type,
            nameToken,
            std::move(initializer)
        );
    }
    
    /*
    
    IDENTIFIER      ::= Letter { Letter | Digit | '_' }
    NUMBER_LITERAL  ::= Digit { Digit }
    STRING_LITERAL  ::= '"' { Character } '"'
    
    */

    std::unique_ptr<ASTNode> parseIdentifier() {
        Token idTok = previous();
        return std::make_unique<VariableNode>(
            idTok.value,     
            idTok.line,
            idTok.column
        );
    }

    std::unique_ptr<ASTNode> parseNumberLiteral() {
        Token idTok = previous();
        return std::make_unique<NumberLiteralNode>(
            idTok.value,     
            idTok.line,
            idTok.column
        );
    }

    std::unique_ptr<ASTNode> parseStringLiteral() {
        Token idTok = previous();
        return std::make_unique<StringLiteralNode>(
            idTok.value,     
            idTok.line,
            idTok.column
        );
    }

    /*
    => These are already handled in lexer, we dont have it in parser
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
    */
    
    


    


public:
    std::unique_ptr<BlockNode> parseProgram();

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

