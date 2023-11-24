#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <bits/stdc++.h>
#include <string>
#include "sym_tab.hpp"
#include "semantic.hpp"

std::string codegen_headers();

std::string basic_type_to_string(ELETYPE type);
std::string type_to_string(struct type_info* t);
std::string codegen_operator(OPERATOR op);
std::string codegen_operator( OPERATOR op, std::string op1, std::string op2);
std::vector<std::string> join(std::vector<std::string>* vec, std::string delim);
std::string codegen_decl_img(struct type_info* t, std::string name, int h, int w, int c);
std::string codegen_decl_img(struct type_info* t, std::string name, struct type_info* p);
std::string codegen_decl_vid(struct type_info* t, std::string name, int h, int w, int fps);

std::string codegen_function_definition(
    ELETYPE type, 
    std::string name, 
    std::vector<std::pair<std::string, struct type_info*>> *par_vec
);

std::string codegen_sketch_definition(
    std::string name, 
    std::vector<std::pair<std::string, struct type_info*>> *par_vec
);

std::string codegen_decl_numeric(
    struct type_info* t,
    std::string name, 
    std::string expr,
    std::vector<std::string> * id_list,
    int mode=0
);
std::string codegen_decl_numeric_partial(struct type_info* t, std::string name);

#endif