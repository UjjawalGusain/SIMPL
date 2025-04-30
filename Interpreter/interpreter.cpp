#include "interpreter.hpp"
#include <iostream>

void Interpreter::execute(const IR& ir) {
    for (const auto& instr : ir) {
        if (instr.op == "load_const") {
            variables[instr.result] = std::stoi(instr.arg1);
        } else if (instr.op == "add") {
            variables[instr.result] = variables[instr.arg1] + variables[instr.arg2];
        } else if (instr.op == "sub") {
            variables[instr.result] = variables[instr.arg1] - variables[instr.arg2];
        } else if (instr.op == "mul") {
            variables[instr.result] = variables[instr.arg1] * variables[instr.arg2];
        } else if (instr.op == "div") {
            variables[instr.result] = variables[instr.arg1] / variables[instr.arg2];
        } else if (instr.op == "store") {
            variables[instr.result] = variables[instr.arg1];
        }
    }

    std::cout << "\n--- Final Variable Values ---\n";
    for (const auto& [name, value] : variables) {
        std::cout << name << " = " << value << "\n";
    }
}
