#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include "sym_tab.hpp"
#include <bits/stdc++.h>

struct type_info{
    TYPE type;
    ELETYPE eleType;
    std::vector<int>* dim_list;
};

#endif // SEMANTIC_HPP