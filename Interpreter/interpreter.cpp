#include "interpreter.hpp"
#include <iostream>

std::string Interpreter::evaluate(const std::string& operand) {
    if (variables.find(operand) != variables.end()) {
        return variables[operand];
    }
    return operand;
}

bool Interpreter::toBool(const std::string& val) {
    return val != "0" && val != "" && val != "false";
}

void Interpreter::execute(const std::vector<IRInstruction>& instructions) {
    size_t ip = 0;
    std::stack<size_t> callStack;

    while (ip < instructions.size()) {
        const auto& instr = instructions[ip];
        const std::string& op = instr.opcode;

        if (op == "assign") {
            variables[instr.result] = evaluate(instr.arg1);
        }
        else if (op == "add" || op == "sub" || op == "mul" || op == "div") {
            int left = std::stoi(evaluate(instr.arg1));
            int right = std::stoi(evaluate(instr.arg2));
            int result = 0;

            if (op == "add") result = left + right;
            else if (op == "sub") result = left - right;
            else if (op == "mul") result = left * right;
            else if (op == "div") result = right != 0 ? left / right : 0;

            variables[instr.result] = std::to_string(result);
        }
        else if (op == "print") {
            std::cout << evaluate(instr.arg1) << std::endl;
        }
        else if (op == "goto") {
            ip = std::stoi(instr.result);
            continue;
        }
        else if (op == "if_false") {
            if (!toBool(evaluate(instr.arg1))) {
                ip = std::stoi(instr.result);
                continue;
            }
        }
        else if (op == "call") {
            callStack.push(ip + 1);
            ip = std::stoi(instr.result);
            continue;
        }
        else if (op == "ret") {
            if (callStack.empty()) break;
            ip = callStack.top();
            callStack.pop();
            continue;
        }

        ip++;
    }
}

