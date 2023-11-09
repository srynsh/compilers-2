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
    XOR
};

#endif // SEMANTIC_HPP