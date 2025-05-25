#include "../include/symbol_table.hpp"
#include <stdexcept>
#include <iostream>

std::unordered_map<std::string, SymbolInfo> symbolTable;
void SymbolTable::enterScope() {
    scopes.emplace_back();
}

void SymbolTable::printScope(const std::unordered_map<std::string, SymbolInfo>& scope) const {
    for (const auto& [name, info] : scope) {
        std::cout << "  " << name << " : " 
                  << (info.type == Type::NUMBER ? "number" : "string")
                  << ", initialized: " << (info.isInitialized ? "yes" : "no") 
                  << std::endl;
    }
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        printScope(scopes.back());
        scopes.pop_back();
        std::cout << "We exited scope here" << std::endl;
    }
}

// bool SymbolTable::declare(const std::string& name, Type type) {
//     if (scopes.empty()) {
//         enterScope();
//     }

//     auto& currentScope = scopes.back();
//     if (currentScope.find(name) != currentScope.end()) {
//         return false; // Already declared in current scope
//     }
//     std::cout << "We declared scope at: " << name << std::endl;
//     currentScope[name] = {type, false};
//     return true;
// }

// bool SymbolTable::declare(const std::string& name, Type type) {
//     if (scopes.empty()) {
//         // Declare in global scope
//         if (globalScope.find(name) != globalScope.end()) {
//             return false; // already declared globally
//         }

//         globalScope[name] = {type, false};
//         return true;
//     }

//     auto& currentScope = scopes.back();
//     if (currentScope.find(name) != currentScope.end()) {
//         return false; // already declared in current scope
//     }
//     currentScope[name] = {type, false};
//     return true;
// }

bool SymbolTable::declare(const std::string& name, Type type) {
    if (scopes.empty()) {
        // Declare in global scope
        if (globalScope.find(name) != globalScope.end()) {
            return false; // already declared globally
        }
        globalScope[name] = {type, false};
        symbolTable[name] = {type, false};  // Also add here
        return true;
    }

    auto& currentScope = scopes.back();
    if (currentScope.find(name) != currentScope.end()) {
        return false; // already declared in current scope
    }
    currentScope[name] = {type, false};
    symbolTable[name] = {type, false};  // Also add here
    return true;
}


bool SymbolTable::declareGlobal(const std::string& name, Type type) {
    if (globalScope.find(name) != globalScope.end()) {
        return false;
    }
    globalScope[name] = {type, false};
    std::cout << "Declared global variable: " << name << std::endl;
    return true;
}


// bool SymbolTable::assign(const std::string& name) {

//     // auto globalFound = globalScope.find(name);
//     // if (globalFound != globalScope.end()) {
//     //     globalFound->second.isInitialized = true;
//     //     return true;
//     // }

//     auto it = globalScope.find(name);
//     if (it != globalScope.end()) {
//         SymbolInfo modified = it->second;
//         globalScope.erase(it);
//         globalScope.insert({name, modified});
//         return true;
//     }

//     for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
//         auto& scope = *it;
//         auto found = scope.find(name);
//         // if(found != scope.end()) std::cout << "We found scope" << std::endl;
//         if (found != scope.end()) {
//             found->second.isInitialized = true;
//             return true;
//         }
//     }
//     return false;
// }

bool SymbolTable::assign(const std::string& name) {
    auto it = globalScope.find(name);
    if (it != globalScope.end()) {
        it->second.isInitialized = true; 
        return true;
    }

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
    if (globalScope.find(name) != globalScope.end()) {
        return true;
    }

    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }

    return false;
}


bool SymbolTable::isInitialized(const std::string& name) const {

    if (globalScope.find(name) != globalScope.end()) {
        return globalScope.at(name).isInitialized;
    }

    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second.isInitialized;
        }
    }
    return false;
}

Type SymbolTable::getType(const std::string& name) const {

    if (globalScope.find(name) != globalScope.end()) {
        return globalScope.at(name).type;
    }

    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second.type;
        }
    }
    return Type::UNKNOWN;
}

std::unordered_map<std::string, SymbolInfo> SymbolTable::getScope() const {
    return symbolTable;
}

