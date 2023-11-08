#include <bits/stdc++.h>
#include "sym_tab.hpp"

/* ---------------------------------------------------------- 
 * Symbol Table Entry for variables
------------------------------------------------------------ */

data_record::data_record(std::string name, TYPE type, ELETYPE ele_type, std::vector<int>& dimlist, int scope)
    : name(name), type(type), ele_type(ele_type), dimlist(dimlist), scope(scope)
{}

data_record::~data_record() {
    this->dimlist.clear();
    
}

std::string data_record::get_name() {
    return this->name;
}

TYPE data_record::get_type() {
    return this->type;
}

ELETYPE data_record::get_ele_type() {
    return this->ele_type;
}

std::vector<int> data_record::get_dimlist() {
    return this->dimlist;
}

int data_record::get_scope() {
    return this->scope;
}

void data_record::print() {
    std::cout << "Name: " << this->name << std::endl;
    std::cout << "Type: " << (this->type == TYPE::SIMPLE ? "SIMPLE" : "ARRAY") << std::endl;
    std::cout << "Element Type: " << (this->ele_type == ELETYPE::ELE_NUM ? "NUM" : (this->ele_type == ELETYPE::ELE_REAL ? "REAL" : (this->ele_type == ELETYPE::ELE_BOOL ? "BOOL" : (this->ele_type == ELETYPE::ELE_IMG ? "IMG" : (this->ele_type == ELETYPE::ELE_GRAY_IMG ? "GRAY_IMG" : (this->ele_type == ELETYPE::ELE_VID ? "VID" : "GRAY_VID")))))) << std::endl;
    std::cout << "Dimlist: ";

    for (auto i : this->dimlist) {
        std::cout << i << " ";
    }

    std::cout << std::endl;
}

/* ---------------------------------------------------------- 
 * Symbol Table for variables
------------------------------------------------------------ */

void symbol_table_variable::add_variable(std::string name, TYPE type, ELETYPE ele_type, std::vector<int> &dimlist, int scope) {
    data_record* new_record = new data_record(name, type, ele_type, dimlist, scope);
    this->variable_list[name + " " + std::to_string(scope)] = new_record;
}

void symbol_table_variable::add_variable(std::vector<std::string> &names, std::vector<TYPE> &types, std::vector<ELETYPE> &ele_types, std::vector<std::vector<int>> &dimlists, int scope) {
    assert (names.size() == types.size() && types.size() == ele_types.size() && ele_types.size() == dimlists.size());

    for(int i = 0; i < names.size(); i++) {
        data_record* new_record = new data_record(names[i], types[i], ele_types[i], dimlists[i], scope);
        this->variable_list[names[i] + " " + std::to_string(scope)] = new_record;
    }

}

data_record* symbol_table_variable::get_variable(std::string name, int scope) {
    if (this->variable_list.find(name + " " + std::to_string(scope)) != this->variable_list.end()) {
        return this->variable_list[name + " " + std::to_string(scope)];
    } else {
        return nullptr;
    }
}

void symbol_table_variable::delete_variable(int scope) {
    for (auto i = this->variable_list.begin(); i != this->variable_list.end(); i++) {
        if (i->second->get_scope() >= scope) {
            delete i->second;
            this->variable_list.erase(i);
        }
    }
}

symbol_table_variable::~symbol_table_variable() {
    for (auto i : this->variable_list) {
        delete i.second;
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
    if (this->parameter_list.find(name) != this->parameter_list.end()) {
        return this->parameter_list[name];
    } else {
        return nullptr;
    }
}

void function_record::add_parameter(std::string name, TYPE type, ELETYPE ele_type, std::vector<int> &dimlist) {
    data_record* new_record = new data_record(name, type, ele_type, dimlist, 1);
    this->parameter_list[name] = new_record;
}

void function_record::add_parameter(std::vector<std::string> &names, std::vector<TYPE> &types, std::vector<ELETYPE> &ele_types, std::vector<std::vector<int> > &dimlists){
    assert (names.size() == types.size() && types.size() == ele_types.size() && ele_types.size() == dimlists.size());

    for (int i = 0; i < names.size(); i++) {
        data_record* new_record = new data_record(names[i], types[i], ele_types[i], dimlists[i], 1);
        this->parameter_list[names[i]] = new_record;
    }

}

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

void symbol_table_function::add_function(std::string name, ELETYPE return_type) {
    function_record* new_record = new function_record(name, return_type);
    this->function_list[name] = new_record;
    this->current_func_name = name;
    this->current_return_type = return_type;
}

function_record* symbol_table_function::get_function(std::string name) {
    if (this->function_list.find(name) != this->function_list.end()) {
        return this->function_list[name];
    } else {
        return nullptr;
    }
}

void symbol_table_function::print(){
    std::cout << "Function Name: " << current_func_name << std::endl;

    std::cout << "Prev Return Type: " << (current_return_type == ELETYPE::ELE_NUM ? "NUM" : (current_return_type == ELETYPE::ELE_REAL ? "REAL" : (current_return_type == ELETYPE::ELE_BOOL ? "BOOL" : (current_return_type == ELETYPE::ELE_IMG ? "IMG" : (current_return_type == ELETYPE::ELE_GRAY_IMG ? "GRAY_IMG" : (current_return_type == ELETYPE::ELE_VID ? "VID" : "GRAY_VID")))))) << std::endl;

    std::cout << "Functions: " << std::endl;
    for (auto i : this->function_list) {
        i.second->print();
    }

}

symbol_table_function::~symbol_table_function() {
    for (auto i : this->function_list) {
        delete i.second;
    }
}