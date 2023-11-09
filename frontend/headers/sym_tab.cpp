#include <bits/stdc++.h>
#include <iostream>
#include "sym_tab.hpp"
#include "semantic.hpp"

extern void yyerror(const char *s);

/* ---------------------------------------------------------- 
 * Symbol Table Entry for variables
------------------------------------------------------------ */

data_record::data_record(std::string name, TYPE type, ELETYPE ele_type, std::vector<int>& dim_list, int scope)
    : name(name), type(type), ele_type(ele_type), dim_list(dim_list), scope(scope)
{}

data_record::data_record(std::string name, TYPE type, ELETYPE ele_type, int scope)
    : name(name), type(type), ele_type(ele_type), scope(scope)
{}

std::string data_record::get_name() {
    return this->name;
}

TYPE data_record::get_type() {
    return this->type;
}

ELETYPE data_record::get_ele_type() {
    return this->ele_type;
}

std::vector<int> data_record::get_dim_list() {
    return this->dim_list;
}

int data_record::get_scope() {
    return this->scope;
}

void data_record::print() {
    std::cout << "Name: " << this->name << std::endl;
    std::cout << "Type: " << (this->type == TYPE::SIMPLE ? "SIMPLE" : "ARRAY") << std::endl;
    std::cout << "Element Type: " << (this->ele_type == ELETYPE::ELE_NUM ? "NUM" : (this->ele_type == ELETYPE::ELE_REAL ? "REAL" : (this->ele_type == ELETYPE::ELE_BOOL ? "BOOL" : (this->ele_type == ELETYPE::ELE_IMG ? "IMG" : (this->ele_type == ELETYPE::ELE_GRAY_IMG ? "GRAY_IMG" : (this->ele_type == ELETYPE::ELE_VID ? "VID" : "GRAY_VID")))))) << std::endl;
    std::cout << "dim_list: ";

    for (auto i : this->dim_list) {
        std::cout << i << " ";
    }

    std::cout << std::endl;
}

/* ---------------------------------------------------------- 
 * Symbol Table for variables
------------------------------------------------------------ */

void symbol_table_variable::add_variable(std::string name, TYPE type, ELETYPE ele_type, std::vector<int> &dim_list, int scope) {
    data_record* new_record = new data_record(name, type, ele_type, dim_list, scope);
    this->variable_list.push_back(new_record);
    std::cout<<"add "<<scope<<" "<<name<<std::endl;
}

void symbol_table_variable::add_variable(std::string name, TYPE type, ELETYPE ele_type, int scope) {
    data_record* new_record = new data_record(name, type, ele_type, scope);
    this->variable_list.push_back(new_record);
    std::cout<<"add "<<scope<<" "<<name<<std::endl;
}

void symbol_table_variable::add_variable(std::vector<std::string> &names, TYPE type, ELETYPE ele_type, std::vector<int> &dim_lists, int scope) {
    for (auto i : names) {
        data_record* new_record = new data_record(i, type, ele_type, dim_lists, scope);
        this->variable_list.push_back(new_record);
    }

}

void symbol_table_variable::add_variable(std::vector<std::string> &names, TYPE type, ELETYPE ele_type, int scope) {
    for (auto i : names) {
        data_record* new_record = new data_record(i, type, ele_type, scope);
        this->variable_list.push_back(new_record);
    }
}

void symbol_table_variable::add_variable(std::vector<std::pair<std::string, type_info*> > &var_list, int scope){
    for (auto i : var_list) {
        if (i.second-> type == TYPE::ARRAY) 
            this->add_variable(i.first, i.second->type, i.second->eleType, *(i.second->dim_list), scope);
        else 
            this->add_variable(i.first, i.second->type, i.second->eleType, scope);
    }
}


data_record* symbol_table_variable::get_variable(std::string name, int scope) {
    data_record* res = nullptr;
    bool flag = false;
    std::cout<<"get "<<scope<<" "<<name<<std::endl;
    for (auto i : this->variable_list) {
        if (i->get_name() == name && i->get_scope() <= scope) {
            if (flag && res->get_scope() < i->get_scope()) {
                res = i;
            } else if (!flag) {
                res = i;
                flag = true;
            }
        }
    }

    if (res != nullptr) {
        return res;
    }

    std::string err = "Variable " + name + " not declared";
    yyerror(err.c_str());
    exit(1);
}

void symbol_table_variable::print() {
    std::cout << "Variables: " << std::endl;
    for (auto i : this->variable_list) {
        i->print();
    }
}

void symbol_table_variable::delete_variable(int scope) {
    for (auto i = this->variable_list.begin(); i != this->variable_list.end(); i++) {
        if ((*i)->get_scope() >= scope) {
            delete *i;
            this->variable_list.erase(i);
            i--;
        }
    }
}

symbol_table_variable::~symbol_table_variable() {
    for (auto i : this->variable_list) {
        delete i;
    }
}

/* ---------------------------------------------------------- 
 * Symbol Table Entry for functions
------------------------------------------------------------ */

function_record::function_record(std::string name, ELETYPE return_type)
    : name(name), return_type(return_type)
{}

ELETYPE function_record::get_return_type() {
    return this->return_type;
}

std::string function_record::get_name() {
    return this->name;
}

data_record* function_record::get_parameter(std::string name) {
    for (auto i : this->parameter_list) {
        if (i.first == name) {
            return i.second;
        }
    }
    return nullptr;
}

void function_record::add_parameter(std::string* name, TYPE type, ELETYPE ele_type, std::vector<int> *dim_list) {
    data_record* new_record = new data_record(*(name), type, ele_type, *(dim_list), 1);
    this->parameter_list.push_back(std::make_pair(*(name), new_record));
    std::cout << "add parameter " << *(name) << std::endl;
}

void function_record::add_parameter(std::string* name, TYPE type, ELETYPE ele_type) {
    data_record* new_record = new data_record(*(name), type, ele_type, 1);
    this->parameter_list.push_back(std::make_pair(*(name), new_record));
    std::cout << "add parameter " << *(name) << std::endl;
}

// void function_record::add_parameter(std::vector<std::string> &names, std::vector<TYPE> &types, std::vector<ELETYPE> &ele_types, std::vector<std::vector<int> > &dim_lists){
//     assert (names.size() == types.size() && types.size() == ele_types.size() && ele_types.size() == dim_lists.size());

//     for (int i = 0; i < names.size(); i++) {
//         data_record* new_record = new data_record(names[i], types[i], ele_types[i], dim_lists[i], 1);
//         this->parameter_list[names[i]] = new_record;
//     }

// }

void function_record::print() {
    std::cout << "Name: " << this->name << std::endl;

    std::cout << "Return Type: " << (this->return_type == ELETYPE::ELE_NUM ? "NUM" : (this->return_type == ELETYPE::ELE_REAL ? "REAL" : (this->return_type == ELETYPE::ELE_BOOL ? "BOOL" : (this->return_type == ELETYPE::ELE_IMG ? "IMG" : (this->return_type == ELETYPE::ELE_GRAY_IMG ? "GRAY_IMG" : (this->return_type == ELETYPE::ELE_VID ? "VID" : "GRAY_VID")))))) << std::endl;
    
    std::cout << "Parameters: " << std::endl;

    for (auto i : this->parameter_list) {
        i.second->print();
    }

}

/* ---------------------------------------------------------- 
 * Symbol Table for functions
------------------------------------------------------------ */

void symbol_table_function::add_function_record(std::string name, ELETYPE return_type) {

    // Check if main function is already defined
    // if (name == "main") {
    //     if (this->function_list != this->function_list.end()) {
    //         yyerror("main function is already defined");
    //     }
    // }

    function_record* new_record = new function_record(name, return_type);
    this->function_list.push_back(new_record);
    this->current_func_name = name;
    this->current_return_type = return_type;
}

void symbol_table_function::add_function_record(std::string name, ELETYPE return_type, std::vector<std::pair<std::string, type_info*> > *par_vec) {
    function_record* new_record = new function_record(name, return_type);
    this->function_list.push_back(new_record);
    this->current_func_name = name;
    this->current_return_type = return_type;
    for (auto i : *(par_vec)) {
        if (i.second-> type == TYPE::ARRAY) 
            new_record->add_parameter(&(i.first), i.second->type, i.second->eleType, i.second->dim_list);
        else 
            new_record->add_parameter(&(i.first), i.second->type, i.second->eleType);
    }
}


function_record* symbol_table_function::get_function(std::string name) {
    for (auto i : this->function_list) {
        if (i->get_name() == name) {
            return i;
        }
    }

    return nullptr;
}

void symbol_table_function::print(){
    std::cout << "Function Name: " << current_func_name << std::endl;

    std::cout << "Prev Return Type: " << (current_return_type == ELETYPE::ELE_NUM ? "NUM" : (current_return_type == ELETYPE::ELE_REAL ? "REAL" : (current_return_type == ELETYPE::ELE_BOOL ? "BOOL" : (current_return_type == ELETYPE::ELE_IMG ? "IMG" : (current_return_type == ELETYPE::ELE_GRAY_IMG ? "GRAY_IMG" : (current_return_type == ELETYPE::ELE_VID ? "VID" : "GRAY_VID")))))) << std::endl;

    std::cout << "Functions: " << std::endl;
    for (auto i : this->function_list) {
        i->print();
    }

}

symbol_table_function::~symbol_table_function() {
    for (auto i : this->function_list) {
        delete i;
    }
}