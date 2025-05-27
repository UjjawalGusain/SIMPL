#pragma once

#include <string>
#include <vector>
#include <variant> 
#include <unordered_map> 
#include <stack>   

#include "../IR/ir.hpp" 

using VMValue = std::variant<long long, std::string>;

struct CallFrame {
    int return_address;
    std::unordered_map<std::string, VMValue> local_variables;
};

struct VMVariable {
    VMValue value;
};

class TACInterpreter {
private:
    const IR& ir;

    VMValue last_return_value; 

    std::unordered_map<std::string, int> labels;
    std::unordered_map<std::string, int> function_entry_points;

    std::stack<VMValue> arg_passing_stack;

    std::stack<CallFrame> call_stack;

    VMValue get_operand_value(const std::string& operand_str);

    void set_variable_value(const std::string& var_name, VMValue val);

    void pre_scan_for_labels_and_functions();

public:
    TACInterpreter(const IR& intermediate_representation);

    void execute();
};

bool isNumericLiteral(const std::string& s);
bool isStringLiteral(const std::string& s);