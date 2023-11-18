#include <bits/stdc++.h>
#include "codegen.hpp"
#include "semantic.hpp"
#include "sym_tab.hpp"


/// @brief Generates the headers for the output C++ code
std::string codegen_headers() {
    std::string code = "";
    code += "#include <bits/stdc++.h>\n";
    code += "#include \"image.hpp\"\n";
    code += "#include \"turtle.hpp\"\n";
    code += "#include \"./frontend/headers/kernel.hpp\"\n";
    code += "#include \"./frontend/headers/load_bmp.hpp\"\n";
    code += '\n';
    code += "using namespace std;\n\n";

    return code;
}


/// @brief Converts simple types to corresponding type in C++
std::string basic_type_to_string(ELETYPE type) {
    if (type == ELETYPE::ELE_NUM)
        return "int";
    else if (type == ELETYPE::ELE_REAL)
        return "float";
    else if (type == ELETYPE::ELE_BOOL)
        return "bool";
    else if (type == ELETYPE::ELE_IMG)
        return "image";
    else if (type == ELETYPE::ELE_GRAY_IMG)
        return "gray_image";
    else if (type == ELETYPE::ELE_VID)
        return "video";
    else if (type == ELETYPE::ELE_GRAY_VID)
        return "gray_video";
    else if (type == ELETYPE::ELE_VOID)
        return "void";
    else if (type == ELETYPE::ELE_STR)
        return "string";
    else
        return "unknown_eletype";
}

/// @brief Converts any type (arrays included) to corresponding type in C++
std::string type_to_string(struct type_info* t) {
    if (t->type == TYPE::SIMPLE) {
        return basic_type_to_string(t->eleType);
    } else {
        if (t->eleType == ELETYPE::ELE_NUM) {
            if (t->dim_list == NULL)
                return "error";
            else if (t->dim_list->size() == 1)
                return "vector<int>";
            else if (t->dim_list->size() == 2)
                return "vector<vector<int>>";
            else if (t->dim_list->size() == 3)
                return "vector<vector<vector<int>>>";
            else
                return "unknown_type";
        }
        else if (t->eleType == ELETYPE::ELE_REAL) {
            if (t->dim_list->size() == 1)
                return "vector<float>";
            else if (t->dim_list->size() == 2)
                return "vector<vector<float>>";
            else if (t->dim_list->size() == 3)
                return "vector<vector<vector<float>>>";
            else
                return "unknown_type";
        }
        else {
            return "unknown_type";
        }
    }
}

/// @brief Generates the code in C++ for operators
std::string codegen_operator(OPERATOR op) {
    std::string code = "";
    if (op == OPERATOR::ADD)
        code += "+";
    else if (op == OPERATOR::SUB)
        code += "-";
    else if (op == OPERATOR::MUL)
        code += "*";
    else if (op == OPERATOR::DIV)
        code += "/";
    else if (op == OPERATOR::XOR)
        code += "^";
    else if (op == OPERATOR::POST_INCR)
        code += "++";
    else if (op == OPERATOR::POST_DECR)
        code += "--";
    else if (op == OPERATOR::INV)
        code += "~";
    else if (op == OPERATOR::NOT)
        code += "!";
    else if (op == OPERATOR::NEQ)
        code += "!=";
    else if (op == OPERATOR::GTEQ)
        code += ">=";
    else if (op == OPERATOR::LTEQ)
        code += "<=";
    else if (op == OPERATOR::GREATER)
        code += ">";
    else if (op == OPERATOR::LESSER)
        code += "<";
    else if (op == OPERATOR::EQUAL)
        code += "==";
    else
        code += "unknown_operator";
    return code;
}

/// @brief combines an operator and two operands
std::vector<std::string> codegen_operator(OPERATOR op, std::vector<std::string>* ops) {
    std::vector<std::string> code = {};
    if (op == OPERATOR::ADD)
        code.push_back(ops->at(0) + " + " + ops->at(1));
    else if (op == OPERATOR::SUB)
        code.push_back(ops->at(0) + " - " + ops->at(1));
    else if (op == OPERATOR::MUL)
        code.push_back(ops->at(0) + " * " + ops->at(1));
    else if (op == OPERATOR::DIV)
        code.push_back(ops->at(0) + " / " + ops->at(1));
    else if (op == OPERATOR::XOR)
        code.push_back(ops->at(0) + " ^ " + ops->at(1));
    else if (op == OPERATOR::POST_INCR)
        code.push_back(ops->at(0) + "++");
    else if (op == OPERATOR::POST_DECR)
        code.push_back(ops->at(0) + "--");
    else if (op == OPERATOR::INV)
        code.push_back(ops->at(0) + ".invert()");
    else if (op == OPERATOR::NOT)
        code.push_back("!" + ops->at(0));
    else if (op == OPERATOR::NEQ)
        code.push_back(ops->at(0) + " != " + ops->at(1));
    else if (op == OPERATOR::GTEQ)
        code.push_back(ops->at(0) + " >= " + ops->at(1));
    else if (op == OPERATOR::LTEQ)
        code.push_back(ops->at(0) + " <= " + ops->at(1));
    else if (op == OPERATOR::GREATER)
        code.push_back(ops->at(0) + " > " + ops->at(1));
    else if (op == OPERATOR::LESSER)
        code.push_back(ops->at(0) + " < " + ops->at(1));
    else if (op == OPERATOR::EQUAL)
        code.push_back(ops->at(0) + " == " + ops->at(1));
    else
        code.push_back("unknown_operator");

    return code;
}

/// @brief Generate the code in C++ for the function definition
std::string codegen_function_definition(
    ELETYPE type, 
    std::string name, 
    std::vector<std::pair<std::string, struct type_info*>> *par_vec
) {
    std::string code = "";
    if (name == "main")
        type = ELETYPE::ELE_NUM;
    code += basic_type_to_string(type) + " " + name + "(";

    if (par_vec != NULL) {
    for (auto par : *par_vec) {
        code += type_to_string(par.second) + " " + par.first + ", ";
    }
    // Remove the last comma and space if parameters are present
        code.pop_back(); 
        code.pop_back();
    }

    code += ")";

    return code;
}

/// @brief Generates the code in C++ for numeric declarations (not definitions)
std::string codegen_decl_numeric(
    struct type_info* t,
    std::string name, 
    std::string expr,
    std::vector<std::string> * id_list
) {
    std::string code = "";
    
    if (expr == "") {
        code += type_to_string(t) + " ";
        if (id_list != NULL) {
            for (auto id : *id_list) {
                code += id + ", ";
            }
            // Remove the last comma and space if parameters are present
            code.pop_back(); 
            code.pop_back();
        }
        else {
            code += name;
        }
    }
    else {
        code += type_to_string(t) + " " + name + " = " + expr; 
    }

    return code;
}

