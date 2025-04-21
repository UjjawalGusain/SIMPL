#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <vector>

enum class Type {
    NUMBER,
    STRING,
    VOID,
    UNKNOWN
};

struct SymbolInfo {
    Type type;
    bool isInitialized;
};

class SymbolTable {
public:
    void enterScope();
    void exitScope();
    bool declare(const std::string& name, Type type);
    bool assign(const std::string& name);
    bool isDeclared(const std::string& name) const;
    bool isInitialized(const std::string& name) const;
    Type getType(const std::string& name) const;

private:
    std::vector<std::unordered_map<std::string, SymbolInfo>> scopes;
};

#endif // SYMBOL_TABLE_HPP
