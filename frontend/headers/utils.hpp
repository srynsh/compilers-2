#ifndef UTILS_HPP
#define UTILS_HPP

#include <bits/stdc++.h>
#include "sym_tab.hpp"
#include "semantic.hpp"

void declare_img(symbol_table_variable* stv, struct type_info* ti, std::string name, int scope);
void declare_img(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int color, int scope);
void declare_gray_img(symbol_table_variable* stv, struct type_info* ti, std::string name, int scope);
void declare_gray_img(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int color, int scope);
void declare_vid(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int frame_rate, int scope);
void declare_gray_vid(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int frame_rate, int scope);
void print_eleType (ELETYPE eleType);
void print_type(TYPE type);
# endif