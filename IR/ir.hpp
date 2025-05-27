
#pragma once
#include <string>
#include <vector>

struct IRInstruction {
    std::string opcode;
    std::string arg1;
    std::string arg2;
    std::string result;

    IRInstruction() = default;

    IRInstruction(std::string opcode, std::string arg1 = "", std::string arg2 = "", std::string result = "")
        : opcode(std::move(opcode)), arg1(std::move(arg1)), arg2(std::move(arg2)), result(std::move(result)) {}
};

class IR {
public:
    std::vector<IRInstruction> instructions;

    void add(const IRInstruction& instr) {
        instructions.push_back(instr);
    }

    void clear() {
        instructions.clear();
    }

    void print() const {
        for (const auto& instr : instructions) {
            printf("%s %s %s %s\n",
                instr.opcode.c_str(),
                instr.arg1.c_str(),
                instr.arg2.c_str(),
                instr.result.c_str());
        }
    }
};