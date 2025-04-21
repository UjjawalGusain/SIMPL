#include "../include/symbol_table.hpp"
#include <stdexcept>

void SymbolTable::enterScope() {
    scopes.emplace_back();
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

bool SymbolTable::declare(const std::string& name, Type type) {
    if (scopes.empty()) {
        enterScope();
    }

    auto& currentScope = scopes.back();
    if (currentScope.find(name) != currentScope.end()) {
        return false; // Already declared in current scope
    }

    currentScope[name] = {type, false};
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
        if (it->find(name) != it->end()) {
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
