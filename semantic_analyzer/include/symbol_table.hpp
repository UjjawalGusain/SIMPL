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
    std::string name;
    Type type;
    std::vector<std::pair<Type, std::string>> params;
    Type returns;
    std::string parentScope;
    std::vector<std::string> childScopes;
    bool isFunction;
    bool isInitialized;

    SymbolInfo(
        const std::string &name = "",
        Type type = Type::UNKNOWN,
        const std::vector<std::pair<Type, std::string>> &params = {},
        Type returns = Type::UNKNOWN,
        const std::string &parentScope = "",
        const std::vector<std::string> &childScopes = {},
        bool isFunction = false,
        bool isInitialized = false)
        : name(name),
          type(type),
          params(params),
          returns(returns),
          parentScope(parentScope),
          childScopes(childScopes),
          isFunction(isFunction),
          isInitialized(isInitialized) {}
};

class SymbolTable {
public:
    SymbolTable(const std::string &scopeName = "global") : currentScopeName(scopeName) {
        scopes.emplace_back();
    }
    bool isDeclaredInCurrentScope(const std::string &name) const;
    void print(int) const;
    void printScope(const std::unordered_map<std::string, SymbolInfo> &) const;
    void enterScope(const std::string &scopeName);
    void exitScope();
    bool declare(const std::string &name, Type type, bool isInitialized = false);
    bool declareFunction(const std::string &name, Type returnType, const std::vector<std::pair<Type, std::string>> &params);
    // --- ADD THIS LINE ---
    void updateFunctionReturnType(const std::string& name, Type newType);
    // ---------------------
    bool assign(const std::string &name);
    bool isDeclared(const std::string &name) const;
    bool isInitialized(const std::string &name) const;
    Type getType(const std::string &name) const;
    SymbolInfo getSymbolInfo(const std::string &name) const;

    const std::string &getCurrentScopeName() const { return currentScopeName; }
    std::unordered_map<std::string, SymbolInfo> getScope() const;


private:
    std::vector<std::unordered_map<std::string, SymbolInfo>> scopes;

    std::string currentScopeName;
};

std::string typeToString(Type t);

#endif