#include <bits/stdc++.h>
#include "utils.hpp"
#include "sym_tab.hpp"
#include "semantic.hpp"

extern void yyerror(const char *s);

/* ---------------------------------------------------------- 
 * Declaration 
------------------------------------------------------------ */

void declare_img(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int color, int scope) {
    if (h <= 0 || w <= 0) {
        yyerror("Image dimensions should be positive");
        exit(1);
    }
    if (color < 0x000000 || color > 0xFFFFFF) {
        yyerror("Invalid color");
        exit(1);
    }
    ti->dim_list = new std::vector<int>(3);
    (*ti->dim_list)[0] = h; (*ti->dim_list)[1] = w; (*ti->dim_list)[2] = 3;
    stv->add_variable(name, ti->type, ti->eleType, scope);
}

void declare_gray_img(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int color, int scope) {
    if (h <= 0 || w <= 0) {
        yyerror("Image dimensions should be positive");
        exit(1);
    }
    if (color < 0 || color > 255) {
        yyerror("Invalid color");
        exit(1);
    }
    ti->dim_list = new std::vector<int>(3);
    (*ti->dim_list)[0] = h; (*ti->dim_list)[1] = w; (*ti->dim_list)[2] = 1;
    stv->add_variable(name, ti->type, ti->eleType, scope);
}

void declare_vid(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int frame_rate, int scope) {
    if (h <= 0 || w <= 0) {
        yyerror("Video dimensions should be positive");
        exit(1);
    }
    if (frame_rate <= 0) {
        yyerror("Video frame rate should be positive");
        exit(1);
    }
    ti->dim_list = new std::vector<int>(4);
    (*ti->dim_list)[0] = h; (*ti->dim_list)[1] = w; (*ti->dim_list)[2] = 3; (*ti->dim_list)[3] = frame_rate;
    stv->add_variable(name, ti->type, ti->eleType, scope);
}

void declare_gray_vid(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int frame_rate, int scope) {
    if (h <= 0 || w <= 0) {
        yyerror("Video dimensions should be positive");
        exit(1);
    }
    if (frame_rate <= 0) {
        yyerror("Video frame rate should be positive");
        exit(1);
    }
    ti->dim_list = new std::vector<int>(4);
    (*ti->dim_list)[0] = h; (*ti->dim_list)[1] = w; (*ti->dim_list)[2] = 1; (*ti->dim_list)[3] = frame_rate;
    stv->add_variable(name, ti->type, ti->eleType, scope);
}
