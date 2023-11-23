#include <bits/stdc++.h>
#include <string>
#include "codegen.hpp"
#include "semantic.hpp"
#include "sym_tab.hpp"


/// @brief Generates the headers for the output C++ code
std::string codegen_headers() {
    std::string code = "";
    code += "#include <bits/stdc++.h>\n";
    code += "#include \"./frontend/headers/image.hpp\"\n";
    code += "#include \"./frontend/headers/turtle.hpp\"\n";
    code += "#include \"./frontend/headers/kernel.hpp\"\n";
    code += "#include \"./frontend/headers/load_bmp.hpp\"\n";
    code += '\n';
    code += "using namespace std;\n\n";
    code += "turtle _t_global;\n\n";

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
std::string codegen_operator(OPERATOR op, std::string op1, std::string op2) {
    std::string code;
    if (op == OPERATOR::ADD)
        code = op1 + " + " + op2;
    else if (op == OPERATOR::SUB)
        code = op1 + " - " + op2;
    else if (op == OPERATOR::MUL)
        code = op1 + " * " + op2;
    else if (op == OPERATOR::DIV)
        code = op1 + " / " + op2;
    else if (op == OPERATOR::XOR)
        code = op1 + " ^ " + op2;
    else if (op == OPERATOR::POST_INCR)
        code = op1 + "++ ";
    else if (op == OPERATOR::POST_DECR)
        code = op1 + "-- ";
    else if (op == OPERATOR::INV)
        code = "~" + op1;
    else if (op == OPERATOR::NOT)
        code = "!" + op1;
    else if (op == OPERATOR::NEQ)
        code = op1 + " != " + op2;
    else if (op == OPERATOR::GTEQ)
        code = op1 + " >= " + op2;
    else if (op == OPERATOR::LTEQ)
        code = op1 + " <= " + op2;
    else if (op == OPERATOR::GREATER)
        code = op1 + " > " + op2;
    else if (op == OPERATOR::LESSER)
        code = op1 + " < " + op2;
    else if (op == OPERATOR::EQUAL)
        code = op1 + " == " + op2;
    else
        code = "unknown_operator";

    return code;
}

/// @brief Joins a vector of strings into a single string, separated by a delimiter. Return a vector containing the joined string
std::vector<std::string> join(std::vector<std::string>* vec, std::string delim) {
    if (vec->size() == 1)
        return *vec;
    std::string code_str = "";
    for (auto s : *vec) {
        code_str += s + " " + delim + " ";
    }
    // Remove the last delimiter and space if parameters are present
    if (vec->size() > 0) {
        code_str.pop_back(); 
        code_str.pop_back();
    }
    std::vector<std::string> code = {};
    code.push_back(code_str);
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

/// @brief Generate the code in C++ for the sketch definition
std::string codegen_sketch_definition(
    std::string name, 
    std::vector<std::pair<std::string, struct type_info*>> *par_vec
) {
    std::string code = "";
    code += std::string("image") + " " + name + "(";

    if (par_vec != NULL) {
        code += " image i, ";
    for (auto par : *par_vec) {
        code += type_to_string(par.second) + " " + par.first + ", ";
    }
    code += "turtle t";
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

/// @brief Generates the code in C++ for numeric declarations partially 
std::string codegen_decl_numeric_partial(struct type_info* t, std::string name) {
    std::string code = "";
    code += type_to_string(t) + " " + name;

    return code;
}

std::string codegen_decl_img(struct type_info* t, std::string name, int h, int w, int c) {
    std::string code = "";
    code += type_to_string(t) + " " + name + "(" + std::to_string(h) + ", " + std::to_string(w) + ", " + std::to_string(c) + ")";

    return code;
}

std::string codegen_decl_img(struct type_info* t, std::string name, struct type_info* p) {
    std::string code = "";
    // change single quotes in p->name to double quotes
    p->name[0] = '\"';
    p->name[p->name.size()-1] = '\"';
    code += type_to_string(t) + " " + name + "(" + p->name + ")";

    return code;
}

std::string codegen_decl_vid(struct type_info* t, std::string name, int h, int w, int fps) {
    std::string code = "";
    code += type_to_string(t) + " " + name + "(" + std::to_string(h) + ", " + std::to_string(w) + ", " + std::to_string(fps) + ")";

    return code;
}