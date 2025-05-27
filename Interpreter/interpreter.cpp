#include "interpreter.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cctype>

bool isNumericLiteral(const std::string& s) {
    if (s.empty()) return false;
    size_t start = 0;
    if (s[0] == '-') {
        if (s.length() == 1) return false;
        start = 1;
    }
    for (size_t i = start; i < s.length(); ++i) {
        if (!std::isdigit(s[i])) return false;
    }
    return true;
}

bool isStringLiteral(const std::string& s) {
    return s.length() >= 2 && s[0] == '"' && s.back() == '"';
}

TACInterpreter::TACInterpreter(const IR& intermediate_representation)
    : ir(intermediate_representation), last_return_value(0LL) {
    pre_scan_for_labels_and_functions();
}

VMValue TACInterpreter::get_operand_value(const std::string& operand_str) {
    if (isNumericLiteral(operand_str)) {
        try {
            return std::stoll(operand_str);
        } catch (const std::out_of_range& oor) {
            std::cerr << "Runtime Error: Numeric literal out of range: " << operand_str << std::endl;
            return 0LL;
        }
    } else if (isStringLiteral(operand_str)) {
        return operand_str.substr(1, operand_str.length() - 2);
    } else {
        if (!call_stack.empty()) {
            CallFrame& current_frame = call_stack.top();
            if (current_frame.local_variables.count(operand_str)) {
                return current_frame.local_variables.at(operand_str);
            }
        }
        std::cerr << "Runtime Error: Variable or temporary '" << operand_str << "' not found in current scope." << std::endl;
        return 0LL;
    }
}

void TACInterpreter::set_variable_value(const std::string& var_name, VMValue val) {
    if (!call_stack.empty()) {
        CallFrame& current_frame = call_stack.top();
        current_frame.local_variables[var_name] = val;
    } else {
        std::cerr << "Runtime Error: Attempt to set variable '" << var_name << "' with no active call frame. This should not happen (e.g., for global variables in main)." << std::endl;
    }
}

void TACInterpreter::pre_scan_for_labels_and_functions() {
    const auto& instructions = ir.instructions;
    for (int i = 0; i < instructions.size(); ++i) {
        const auto& instr = instructions[i];
        if (instr.opcode == "label") {
            labels[instr.arg1] = i;
        } else if (instr.opcode == "func_start") {
            function_entry_points[instr.arg1] = i;
        }
    }
}

void TACInterpreter::execute() {
    int start_pc = function_entry_points.count("main") ? function_entry_points["main"] : 0;

    if (function_entry_points.find("main") == function_entry_points.end()) {
        std::cerr << "Runtime Error: No 'main' function found to start execution." << std::endl;
        return;
    }

    CallFrame main_frame;
    main_frame.return_address = -1; 
    call_stack.push(main_frame);

    int pc = start_pc; 

    const auto& all_instructions = ir.instructions;

    while (pc < all_instructions.size()) {
        const auto& instr = all_instructions[pc];
        const std::string& opcode = instr.opcode;

        if (opcode == "func_start") {
            std::vector<std::string> param_names_in_order;
            int temp_pc = pc + 1; 
            while (temp_pc < all_instructions.size() && all_instructions[temp_pc].opcode == "param") {
                param_names_in_order.push_back(all_instructions[temp_pc].arg1);
                temp_pc++;
            }

            std::vector<VMValue> received_arg_values;
            for (size_t i = 0; i < param_names_in_order.size(); ++i) {
                if (!arg_passing_stack.empty()) {
                    received_arg_values.push_back(arg_passing_stack.top());
                    arg_passing_stack.pop();
                } else {
                    std::cerr << "Runtime Error: Too few arguments for function '" << instr.arg1 << "'." << std::endl;
                    break; 
                }
            }

            for (int i = 0; i < param_names_in_order.size(); ++i) {
                set_variable_value(param_names_in_order[i], 
                                   received_arg_values[param_names_in_order.size() - 1 - i]);
            }
        } else if (opcode == "func_end") {
            if (!call_stack.empty()) {
                CallFrame completed_frame = call_stack.top();
                call_stack.pop();

                if (!call_stack.empty()) {
                    pc = completed_frame.return_address;
                    continue;
                } else {
                    break;
                }
            } else {
                std::cerr << "Runtime Error: 'func_end' encountered with empty call stack." << std::endl;
                break;
            }
        } else if (opcode == "var") {
            set_variable_value(instr.arg1, {0LL});
        } else if (opcode == "assign") {
            set_variable_value(instr.arg1, get_operand_value(instr.arg2));
        } else if (opcode == "print") {
            VMValue val = get_operand_value(instr.arg1);
            if (std::holds_alternative<long long>(val)) {
                std::cout << std::get<long long>(val) << std::endl;
            } else if (std::holds_alternative<std::string>(val)) {
                std::cout << std::get<std::string>(val) << std::endl;
            }
        } else if (opcode == "label") {
        } else if (opcode == "goto") {
            pc = labels.at(instr.arg1);
            continue;
        } else if (opcode == "ifz_goto") {
            VMValue cond_val = get_operand_value(instr.arg1);
            if (std::holds_alternative<long long>(cond_val) && std::get<long long>(cond_val) == 0) {
                pc = labels.at(instr.arg2);
                continue;
            }
        } else if (opcode == "ret") {
            last_return_value = get_operand_value(instr.arg1);
            if (!call_stack.empty()) {
                CallFrame completed_frame = call_stack.top();
                call_stack.pop();

                if (!call_stack.empty()) {
                    pc = completed_frame.return_address;
                    continue;
                } else {
                    break;
                }
            } else {
                std::cerr << "Runtime Error: 'ret' instruction with empty call stack." << std::endl;
                break;
            }
        } else if (opcode == "arg") {
            arg_passing_stack.push(get_operand_value(instr.arg1));
        } else if (opcode == "call") {
            std::string func_name = instr.arg1;

            CallFrame new_frame;
            new_frame.return_address = pc + 1;
            call_stack.push(new_frame);

            pc = function_entry_points.at(func_name);
            continue;
        } else if (opcode == "param") {
            // Handled by func_start
        } else if (opcode == "move") {
            if (instr.arg1 == "retval") {
                set_variable_value(instr.result, last_return_value);
            } else {
                set_variable_value(instr.result, get_operand_value(instr.arg1));
            }
        }
        else if (opcode == "add" || opcode == "sub" || opcode == "mul" || opcode == "div") {
            long long val1 = std::get<long long>(get_operand_value(instr.arg1));
            long long val2 = std::get<long long>(get_operand_value(instr.arg2));
            long long result_val;
            if (opcode == "add") result_val = val1 + val2;
            else if (opcode == "sub") result_val = val1 - val2;
            else if (opcode == "mul") result_val = val1 * val2;
            else {
                if (val2 == 0) {
                    std::cerr << "Runtime Error: Division by zero at instruction " << pc << "!" << std::endl;
                    break;
                }
                result_val = val1 / val2;
            }
            set_variable_value(instr.result, result_val);
        }
        else if (opcode == "eq" || opcode == "neq" || opcode == "lt" || opcode == "le" || opcode == "gt" || opcode == "ge") {
            VMValue val1 = get_operand_value(instr.arg1);
            VMValue val2 = get_operand_value(instr.arg2);
            bool comparison_result = false;

            if (std::holds_alternative<long long>(val1) && std::holds_alternative<long long>(val2)) {
                long long num1 = std::get<long long>(val1);
                long long num2 = std::get<long long>(val2);
                if (opcode == "eq") comparison_result = (num1 == num2);
                else if (opcode == "neq") comparison_result = (num1 != num2);
                else if (opcode == "lt") comparison_result = (num1 < num2);
                else if (opcode == "le") comparison_result = (num1 <= num2);
                else if (opcode == "gt") comparison_result = (num1 > num2);
                else if (opcode == "ge") comparison_result = (num1 >= num2);
            } else if (std::holds_alternative<std::string>(val1) && std::holds_alternative<std::string>(val2)) {
                std::string str1 = std::get<std::string>(val1);
                std::string str2 = std::get<std::string>(val2);
                if (opcode == "eq") comparison_result = (str1 == str2);
                else if (opcode == "neq") comparison_result = (str1 != str2);
                else {
                    std::cerr << "Runtime Error: String comparison for '" << opcode << "' is not supported: " << instr.arg1 << " vs " << instr.arg2 << std::endl;
                    break;
                }
            } else {
                std::cerr << "Runtime Error: Type mismatch in comparison '" << opcode << "': " << instr.arg1 << " vs " << instr.arg2 << " at instruction " << pc << std::endl;
                break;
            }
            set_variable_value(instr.result, (long long)(comparison_result ? 1 : 0));
        }
        else if (opcode == "and" || opcode == "or") {
            long long val1 = std::get<long long>(get_operand_value(instr.arg1));
            long long val2 = std::get<long long>(get_operand_value(instr.arg2));
            long long logical_result;
            if (opcode == "and") logical_result = ((val1 != 0) && (val2 != 0) ? 1 : 0);
            else logical_result = ((val1 != 0) || (val2 != 0) ? 1 : 0);
            set_variable_value(instr.result, logical_result);
        }
        else if (opcode == "neg") {
            long long val = std::get<long long>(get_operand_value(instr.arg1));
            set_variable_value(instr.result, -val);
        } else if (opcode == "not") {
            long long val = std::get<long long>(get_operand_value(instr.arg1));
            set_variable_value(instr.result, (long long)(val == 0 ? 1 : 0));
        }
        else {
            std::cerr << "Runtime Error: Unhandled IR opcode: " << opcode << " at instruction " << pc << std::endl;
            break;
        }

        pc++;
    }
}