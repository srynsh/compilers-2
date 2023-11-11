#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include "sym_tab.hpp"
#include <bits/stdc++.h>

struct type_info{
    TYPE type;
    ELETYPE eleType;
    std::vector<int>* dim_list; // For arrays -> -1 or (-1, -1) or (-1, -1, -1) | for images -> (h,w,1) or (h,w,3) | for videos -> (h, w, 1, f) | (h, w, 3, f)
};

enum class OPERATOR {
    ADD,
    SUB,
    MUL,
    DIV,
    XOR,
    POST_INCR,
    POST_DECR,
    INV,
};

ELETYPE get_type(ELETYPE t1, ELETYPE t2);

bool is_primitive(ELETYPE t);
bool is_img(ELETYPE t);
bool is_vid(ELETYPE t);
bool is_dim_undefined(std::vector<int> &v, int len);

struct type_info* binary_compatible(struct type_info* t1, struct type_info* t2, OPERATOR op);
struct type_info* unary_compatible(struct type_info* t1, OPERATOR op);
struct type_info* dim_list_compatible(struct type_info* t1, struct type_info* t2);
struct type_info* assignment_compatible(struct type_info* t1, struct type_info* t2);

struct type_info* check_func_call(symbol_table_function* SymbolTableFunction, std::string func_name, std::vector<struct type_info*> *arg_vec);
struct type_info* check_func_call(symbol_table_function* SymbolTableFunction, std::string func_name);

struct type_info* check_inbuilt_func_call(struct type_info* ti, std::string func_name, std::vector<struct type_info*> *arg_list);

#endif // SEMANTIC_HPP