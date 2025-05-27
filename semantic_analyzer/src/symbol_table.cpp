#include "../include/symbol_table.hpp"
#include <stdexcept>
#include <iostream>

std::string typeToString(Type type) {
    switch (type) {
        case Type::NUMBER: return "number";
        case Type::STRING: return "string";
        case Type::VOID: return "void";
        case Type::UNKNOWN: return "unknown";
        default: return "invalid";
    }
}

bool SymbolTable::isDeclaredInCurrentScope(const std::string& name) const {
    if (scopes.empty()) {
        return false;
    }
    return scopes.back().count(name);
}
void SymbolTable::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "Scope Name: " << currentScopeName << "\n";
    std::cout << indentStr << "Number of Scope Levels: " << scopes.size() << "\n\n";

    int level = 0;
    for (const auto& scopeMap : scopes) {
        std::cout << indentStr << "Scope Level " << level;
        if (level == 0) {
            std::cout << " (Function Scope)";
        } else {
            std::cout << " (Nested Block Scope)";
        }
        std::cout << ":\n";

        if (scopeMap.empty()) {
            std::cout << indentStr << "  (No symbols in this scope)\n";
        } else {
            for (const auto& [name, info] : scopeMap) {
                std::cout << indentStr << "  Symbol: " << name << "\n";
                std::cout << indentStr << "    Type: " << typeToString(info.type) << "\n";
                if (info.isFunction) {
                    std::cout << indentStr << "    Kind: Function\n";
                    std::cout << indentStr << "    Returns: " << typeToString(info.returns) << "\n";
                    std::cout << indentStr << "    Parameters: ";
                    if (info.params.empty()) {
                        std::cout << "None\n";
                    } else {
                        std::cout << "\n";
                        for (const auto& [paramType, paramName] : info.params) {
                            std::cout << indentStr << "      - " << typeToString(paramType) << " " << paramName << "\n";
                        }
                    }
                } else {
                    std::cout << indentStr << "    Kind: Variable\n";
                    std::cout << indentStr << "    Initialized: " << (info.isInitialized ? "Yes" : "No") << "\n";
                }
            }
        }
        std::cout << "\n--------------------------------------------\n\n";
        level++;
    }

    std::cout << indentStr << "--- End of " << currentScopeName << " ---\n";
}




void SymbolTable::printScope(const std::unordered_map<std::string, SymbolInfo>& scope) const {
    for (const auto& [name, info] : scope) {
        std::cout << "  " << info.name << " : "
                  << typeToString(info.type)
                  << ", initialized: " << (info.isInitialized ? "yes" : "no")
                  << std::endl;
    }
}

void SymbolTable::enterScope(const std::string& scopeName) {
    scopes.emplace_back();
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}
bool SymbolTable::declare(const std::string& name, Type type, bool isInitialized) {
    if (scopes.empty()) {
        scopes.emplace_back(); 
    }

    if (scopes.back().count(name)) { 
        return false; 
    }
    scopes.back()[name] = SymbolInfo(name, type, {}, Type::UNKNOWN, currentScopeName /* parent can be table name */, {}, false, isInitialized);
    return true;
}

bool SymbolTable::declareFunction(const std::string& name, Type returnType, const std::vector<std::pair<Type, std::string>>& params) {
    if (scopes.empty()) {
        scopes.emplace_back();
    }
    auto& currentScopeMap = scopes.back();
    if (currentScopeMap.count(name)) {
        return false; 
    }
    currentScopeMap[name] = SymbolInfo(name, Type::VOID , params, returnType, currentScopeName, {}, true, true );
    return true;
}


bool SymbolTable::assign(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto& scope = *it;
        auto found = scope.find(name);
        if (found != scope.end()) {
            found->second.isInitialized = true;
            return true;
        }
    }
    return false;
}

bool SymbolTable::isDeclared(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->count(name)) {
            return true;
        }
    }
    return false;
}

bool SymbolTable::isInitialized(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second.isInitialized;
        }
    }
    return false;
}

Type SymbolTable::getType(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second.type;
        }
    }
    return Type::UNKNOWN;
}

SymbolInfo SymbolTable::getSymbolInfo(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second;
        }
    }
    return SymbolInfo();
}

std::unordered_map<std::string, SymbolInfo> SymbolTable::getScope() const {
    if (!scopes.empty()) {
        return scopes.back();
    }
    return {};
}