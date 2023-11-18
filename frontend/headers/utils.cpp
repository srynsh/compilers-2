#include <bits/stdc++.h>
#include "utils.hpp"
#include "sym_tab.hpp"
#include "semantic.hpp"

extern void yyerror(const char *s);

/* ---------------------------------------------------------- 
 * Declaration 
------------------------------------------------------------ */

void declare_img(symbol_table_variable* stv, struct type_info* ti, std::string name, int scope) {
    ti->dim_list = new std::vector<int>(3);
    (*ti->dim_list)[0] = -1; (*ti->dim_list)[1] = -1; (*ti->dim_list)[2] = 3;
    stv->add_variable(name, ti->type, ti->eleType, (*ti->dim_list), scope);
}

void declare_img(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int color, int scope) {
    if (h <= 0 || w <= 0) {
        yyerror("Image dimensions should be positive");
        
    }
    if (color < 0x000000 || color > 0xFFFFFF) {
        yyerror("Invalid color");
        
    }
    ti->dim_list = new std::vector<int>(3);
    (*ti->dim_list)[0] = h; (*ti->dim_list)[1] = w; (*ti->dim_list)[2] = 3;
    stv->add_variable(name, ti->type, ti->eleType, (*ti->dim_list), scope);
}

void declare_gray_img(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int color, int scope) {
    if (h <= 0 || w <= 0) {
        yyerror("Image dimensions should be positive");
        
    }
    if (color < 0 || color > 255) {
        yyerror("Invalid color");
        
    }
    ti->dim_list = new std::vector<int>(3);
    (*ti->dim_list)[0] = h; (*ti->dim_list)[1] = w; (*ti->dim_list)[2] = 1;
    stv->add_variable(name, ti->type, ti->eleType, (*ti->dim_list), scope);
}

void declare_gray_img(symbol_table_variable* stv, struct type_info* ti, std::string name, int scope) {
    ti->dim_list = new std::vector<int>(3);
    (*ti->dim_list)[0] = -1; (*ti->dim_list)[1] = -1; (*ti->dim_list)[2] = 1;
    stv->add_variable(name, ti->type, ti->eleType, (*ti->dim_list), scope);
}

void declare_vid(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int frame_rate, int scope) {
    if (h <= 0 || w <= 0) {
        yyerror("Video dimensions should be positive");
        
    }
    if (frame_rate <= 0) {
        yyerror("Video frame rate should be positive");
        
    }
    ti->dim_list = new std::vector<int>(4);
    (*ti->dim_list)[0] = h; (*ti->dim_list)[1] = w; (*ti->dim_list)[2] = 3; (*ti->dim_list)[3] = frame_rate;
    stv->add_variable(name, ti->type, ti->eleType, (*ti->dim_list), scope);
}

void declare_gray_vid(symbol_table_variable* stv, struct type_info* ti, std::string name, int h, int w, int frame_rate, int scope) {
    if (h <= 0 || w <= 0) {
        yyerror("Video dimensions should be positive");
        
    }
    if (frame_rate <= 0) {
        yyerror("Video frame rate should be positive");
        
    }
    ti->dim_list = new std::vector<int>(4);
    (*ti->dim_list)[0] = h; (*ti->dim_list)[1] = w; (*ti->dim_list)[2] = 1; (*ti->dim_list)[3] = frame_rate;
    stv->add_variable(name, ti->type, ti->eleType, (*ti->dim_list), scope);
}

void print_eleType (ELETYPE eleType) {
    switch (eleType) {
        case ELETYPE::ELE_NUM:
            std::cout << "num";
            break;
        case ELETYPE::ELE_REAL:
            std::cout << "real";
            break;
        case ELETYPE::ELE_BOOL:
            std::cout << "bool";
            break;
        case ELETYPE::ELE_IMG:
            std::cout << "img";
            break;
        case ELETYPE::ELE_GRAY_IMG:
            std::cout << "gray_img";
            break;
        case ELETYPE::ELE_VID:
            std::cout << "vid";
            break;
        case ELETYPE::ELE_GRAY_VID:
            std::cout << "gray_vid";
            break;
        case ELETYPE::ELE_VOID:
            std::cout << "void";
            break;
        case ELETYPE::ELE_STR:
            std::cout << "str";
            break;
        case ELETYPE::ELE_ERROR:
            std::cout << "error";
            break;
        default:
            std::cout << "unknown";
            break;
    }
}


void print_type(TYPE type) {
    switch (type) {
        case TYPE::SIMPLE:
            std::cout << "simple";
            break;
        case TYPE::ARRAY:
            std::cout << "array";
            break;
        default:
            std::cout << "unknown";
            break;
    }
}

void print_operator(OPERATOR op) {
    switch (op) {
        case OPERATOR::ADD:
            std::cout << "ADD";
            break;
        case OPERATOR::SUB:
            std::cout << "SUB";
            break;
        case OPERATOR::MUL:
            std::cout << "MUL";
            break;
        case OPERATOR::DIV:
            std::cout << "DIV";
            break;
        case OPERATOR::XOR:
            std::cout << "XOR";
            break;
        case OPERATOR::POST_INCR:
            std::cout << "POST_INCR";
            break;
        case OPERATOR::POST_DECR:
            std::cout << "POST_DECR";
            break;
        case OPERATOR::INV:
            std::cout << "INV";
            break;
        default:
            std::cout << "unknown";
            break;
    }
}