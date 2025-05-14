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
    void print() const;
    void printScope(const std::unordered_map<std::string, SymbolInfo>&) const;
    void enterScope();
    void exitScope();
    bool declare(const std::string& name, Type type);
    bool assign(const std::string& name);
    bool isDeclared(const std::string& name) const;
    bool isInitialized(const std::string& name) const;
    Type getType(const std::string& name) const;
    bool declareGlobal(const std::string&, Type);

private:
    std::vector<std::unordered_map<std::string, SymbolInfo>> scopes;
    std::unordered_map<std::string, SymbolInfo> globalScope;
};

#endif // SYMBOL_TABLE_HPP
