#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <tuple>
#include "sym_tab.hpp"
#include "semantic.hpp"

extern void yyerror(const char *s);
extern int lineno;

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
    std::cout << "Name: " << this->name;
    std::cout << " | Type: " << (this->type == TYPE::SIMPLE ? "SIMPLE" : "ARRAY");
    std::cout << " | Element Type: " << (this->ele_type == ELETYPE::ELE_NUM ? "NUM" : (this->ele_type == ELETYPE::ELE_REAL ? "REAL" : (this->ele_type == ELETYPE::ELE_BOOL ? "BOOL" : (this->ele_type == ELETYPE::ELE_IMG ? "IMG" : (this->ele_type == ELETYPE::ELE_GRAY_IMG ? "GRAY_IMG" : (this->ele_type == ELETYPE::ELE_VID ? "VID" : "GRAY_VID"))))));
    std::cout << " | dim_list: ";

    for (auto i : this->dim_list) {
        std::cout << i << " ";
    }

    std::cout << std::endl;
}

/* ---------------------------------------------------------- 
 * Symbol Table for variables
------------------------------------------------------------ */

void symbol_table_variable::add_variable(std::string name, TYPE type, ELETYPE ele_type, std::vector<int> &dim_list, int scope) {
    for (auto x : this->variable_list) {
        if (x->get_name() == name && (x->get_scope() == scope || x->get_scope() == 1)) {
            lineno--; //sus
            std::string err = "Variable " + name + " already declared";
            yyerror(err.c_str());
            lineno++;
            exit(1);
        }
    }
    
    data_record* new_record = new data_record(name, type, ele_type, dim_list, scope);
    this->variable_list.push_back(new_record);
}

void symbol_table_variable::add_variable(std::string name, TYPE type, ELETYPE ele_type, int scope) {
    for (auto x : this->variable_list) {
        if (x->get_name() == name && (x->get_scope() == scope || x->get_scope() == 1)) {
            lineno--;
            std::string err = "Variable " + name + " already declared";
            yyerror(err.c_str());
            lineno++;
            exit(1);
        }
    }

    data_record* new_record = new data_record(name, type, ele_type, scope);
    this->variable_list.push_back(new_record);
}

void symbol_table_variable::add_variable(std::vector<std::string> &names, TYPE type, ELETYPE ele_type, std::vector<int> &dim_lists, int scope) {
    for (auto i : names) {
        for (auto x : this->variable_list) {
            if (x->get_name() == i && (x->get_scope() == scope || x->get_scope() == 1)) {
                lineno--;
                std::string err = "Variable " + i + " already declared";
                yyerror(err.c_str());
                lineno++;
                exit(1);
            }
        }

        data_record* new_record = new data_record(i, type, ele_type, dim_lists, scope);
        this->variable_list.push_back(new_record);
    }
}

void symbol_table_variable::add_variable(std::vector<std::string> &names, TYPE type, ELETYPE ele_type, int scope) {
    for (auto i : names) {
        for (auto x : this->variable_list) {
            if (x->get_name() == i && (x->get_scope() == scope || x->get_scope() == 1)) {
                lineno--; //sus2
                std::string err = "Variable " + i + " already declared";
                yyerror(err.c_str());
                lineno++;
                exit(1);
            }
        }

        data_record* new_record = new data_record(i, type, ele_type, scope);
        this->variable_list.push_back(new_record);
    }
}

void symbol_table_variable::add_variable(std::vector<std::pair<std::string, type_info*> > &var_list, int scope){
    for (auto i : var_list) {
        if (i.second-> type == TYPE::ARRAY || is_img(i.second->eleType) || is_vid(i.second->eleType))
            this->add_variable(i.first, i.second->type, i.second->eleType, *(i.second->dim_list), scope);
        else 
            this->add_variable(i.first, i.second->type, i.second->eleType, scope);
    }
}


data_record* symbol_table_variable::get_variable(std::string name, int scope) {
    data_record* res = nullptr;
    bool flag = false;
    // std::cout<<"get "<<scope<<" "<<name<<std::endl;
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
    return nullptr;
}

void symbol_table_variable::print() {
    // std::cout << "Variables: " << std::endl;
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

std::vector<std::pair<std::string, data_record*> > function_record::get_parameter_list() {
    return this->parameter_list;
}

void function_record::add_parameter(std::string* name, TYPE type, ELETYPE ele_type, std::vector<int> *dim_list) {
    data_record* new_record = new data_record(*(name), type, ele_type, *(dim_list), 1);
    this->parameter_list.push_back(std::make_pair(*(name), new_record));
    // std::cout << "add parameter " << *(name) << std::endl;
}

void function_record::add_parameter(std::string* name, TYPE type, ELETYPE ele_type) {
    data_record* new_record = new data_record(*(name), type, ele_type, 1);
    this->parameter_list.push_back(std::make_pair(*(name), new_record));
    // std::cout << "add parameter " << *(name) << std::endl;
}

void function_record::print() {
    std::cout << "Name: " << this->name;

    std::cout << " | Return Type: " << (this->return_type == ELETYPE::ELE_NUM ? "NUM" : (this->return_type == ELETYPE::ELE_REAL ? "REAL" : (this->return_type == ELETYPE::ELE_BOOL ? "BOOL" : (this->return_type == ELETYPE::ELE_IMG ? "IMG" : (this->return_type == ELETYPE::ELE_GRAY_IMG ? "GRAY_IMG" : (this->return_type == ELETYPE::ELE_VID ? "VID" : (this->return_type == ELETYPE::ELE_GRAY_VID? "GRAY_VID" : "VOID"))))))) << std::endl;
    
    std::cout << "Parameters: " << std::endl;

    for (auto i : this->parameter_list) {
        i.second->print();
    }

    std::cout << std::endl << std::endl;
}

/* ---------------------------------------------------------- 
 * Symbol Table for functions
------------------------------------------------------------ */

/*
Utility function to compare 2 parameter lists
*/

bool compare_parameter_list(std::vector<std::pair<std::string, data_record*> > &parameter_list_1, std::vector<std::pair<std::string, type_info*> > *parameter_list_2) {
    if (parameter_list_1.size() != parameter_list_2->size()) {
        return false;
    }

    for (auto i = 0; i < parameter_list_1.size(); i++) {
        if ((parameter_list_1[i].second->get_ele_type() != parameter_list_2->at(i).second->eleType) || (parameter_list_1[i].second->get_type() != parameter_list_2->at(i).second->type)) {
            return false;
        }
    }

    return true;
}

// Archit
bool compare_par_list_arg_list(std::vector<struct function_record*> func_list, std::vector<struct type_info*> &arg_list) {
    for (auto i : func_list)
    {
        std::vector<std::pair<std::string, struct data_record*>> par_list = i->get_parameter_list();
        if (par_list.size() == arg_list.size()) {
            int j;
            for (j = 0; j < par_list.size(); j++) {
                if ((par_list[j].second->get_ele_type() != arg_list[j]->eleType) || (par_list[j].second->get_type() != arg_list[j]->type)) {
                    struct type_info* t = new struct type_info;
                    t->type = par_list[j].second->get_type();
                    t->eleType = par_list[j].second->get_ele_type();
                    std::vector <int> temp_dim_list = par_list[j].second->get_dim_list();
                    t->dim_list = new std::vector<int>(temp_dim_list.size());
                    for (int k = 0; k < temp_dim_list.size(); k++) {
                        t->dim_list->at(k) = temp_dim_list[k];
                    }
                    assignment_compatible(t, arg_list[j], flag_type::call_stmt);
                }
            }
            if (j == par_list.size())
                return true;
        }
    }
    
    return false;
}

/*-----------------------------------*/

void symbol_table_function::add_function_record(std::string name, ELETYPE return_type) { 
    //functions that have no input parameters

    if (name == "main") {
        for (auto i : this->function_list) {
            if (i->get_name() == name) {
                std::string err = "Main function already declared";
                yyerror(err.c_str());
                exit(1);
            }
        }
    }
    else {
        // functions that have no input parameters
        for (auto i : this->function_list) {
            if (i->get_name() == name && i->get_parameter_list().size() == 0) {
                std::string err = "Function " + name + " already declared";
                yyerror(err.c_str());
                exit(1);
            }
        }
    }


    function_record* new_record = new function_record(name, return_type);
    this->function_list.push_back(new_record);
    this->current_func_name = name;
    this->current_return_type = return_type;
}

void symbol_table_function::add_function_record(std::string name, ELETYPE return_type, std::vector<std::pair<std::string, type_info*> > *par_vec) {
    // functions that have input parameters

    if (name == "main") {
        for (auto i : this->function_list) {
            if (i->get_name() == name) {
                std::string err = "Main function already declared";
                yyerror(err.c_str());
                exit(1);
            }
        }
        std::string err = "Main function cannot have parameters";
        yyerror(err.c_str());
        exit(1);
    }
    else {
        for (auto i : this->function_list) {
            std::vector<std::pair<std::string, data_record*> > temp = i->get_parameter_list();
            if (i->get_name() == name && compare_parameter_list(temp, par_vec)) {
                std::string err = "Function " + name + " already declared";
                yyerror(err.c_str());
                exit(1);
            }
        }
    }


    for (auto i : this->function_list) {
        std::vector<std::pair<std::string, data_record*> > temp = i->get_parameter_list();
        if (i->get_name() == name && compare_parameter_list(temp, par_vec)) {
            std::string err = "Function " + name + " already declared";
            yyerror(err.c_str());
            exit(1);
        }
    }

    function_record* new_record = new function_record(name, return_type);
    this->function_list.push_back(new_record);
    this->current_func_name = name;
    this->current_return_type = return_type;
    for (auto i : *(par_vec)) {
        if (i.second-> type == TYPE::ARRAY || is_img(i.second->eleType) || is_vid(i.second->eleType))
            new_record->add_parameter(&(i.first), i.second->type, i.second->eleType, i.second->dim_list);
        else 
            new_record->add_parameter(&(i.first), i.second->type, i.second->eleType);
    }
}


std::vector<function_record*> symbol_table_function::get_function(std::string name) {
    std::vector<function_record*> res;
    for (auto i : this->function_list) {
        if (i->get_name() == name) {
            res.push_back(i);
        }
    }

    return res;
}

void symbol_table_function::print(){

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


/* ---------------------------------------------------------* 
 * Symbol Table Entry for sketches                          *
 *----------------------------------------------------------*/

sketch_record::sketch_record(std::string name)
    : name(name)
{}

std::string sketch_record::get_name() {
    return this->name;
}

data_record* sketch_record::get_parameter(std::string name) {
    for (auto i : this->parameter_list) {
        if (i.first == name) {
            return i.second;
        }
    }
    return nullptr;
}

std::vector<std::pair<std::string, data_record*>> sketch_record::get_parameter_list() {
    return this->parameter_list;
}

void sketch_record::add_parameter(std::string* name, TYPE type, ELETYPE ele_type) {
    data_record* new_record = new data_record(*(name), type, ele_type, 1);
    this->parameter_list.push_back(std::make_pair(*(name), new_record));
}

void sketch_record::print() {
    std::cout << "Name: " << this->name << std::endl;
    std::cout << "Parameters: " << std::endl;

    for (auto i : this->parameter_list) {
        i.second->print();
    }

    std::cout << std::endl << std::endl;
}

/* ---------------------------------------------------------* 
 * Symbol Table for sketches                                *
 *----------------------------------------------------------*/


// no parameters record
void symbol_table_sketch::add_sketch_record(std::string name) {
    for (auto i : this->sketch_list) {
        if (i->get_name() == name && i->get_parameter_list().size() == 0) {
            std::string err = "Sketch " + name + " already declared";
            yyerror(err.c_str());
            exit(1);
        }
    }

    sketch_record* new_record = new sketch_record(name);
    this->sketch_list.push_back(new_record);
}

// with parameters record
void symbol_table_sketch::add_sketch_record(std::string name, std::vector<std::pair<std::string, type_info*> > *par_vec) {
    for (auto i : this->sketch_list) {
        std::vector<std::pair<std::string, data_record*> > temp = i->get_parameter_list();
        if (i->get_name() == name && compare_parameter_list(temp, par_vec)) {
            std::string err = "Sketch " + name + " already declared";
            yyerror(err.c_str());
            exit(1);
        }
    }

    sketch_record* new_record = new sketch_record(name);
    this->sketch_list.push_back(new_record);
    for (auto i : *(par_vec)) {
        new_record->add_parameter(&(i.first), i.second->type, i.second->eleType);
    }
}

std::vector<sketch_record*> symbol_table_sketch::get_sketch(std::string name) {
    std::vector<sketch_record*> res;
    for (auto i : this->sketch_list) {
        if (i->get_name() == name) {
            res.push_back(i);
        }
    }

    return res;
}

void symbol_table_sketch::print() {
    std::cout << "Sketches: " << std::endl;
    for (auto i : this->sketch_list) {
        i->print();
    }
}

symbol_table_sketch::~symbol_table_sketch() {
    for (auto i : this->sketch_list) {
        delete i;
    }
}

bool compare_par_list_arg_list(std::vector<struct sketch_record*> sketch_list, std::vector<struct type_info*> &arg_list) {
    for (auto i : sketch_list)
    {
        std::vector<std::pair<std::string, struct data_record*>> par_list = i->get_parameter_list();
        if (par_list.size() == arg_list.size()) {
            int j;
            for (j = 0; j < par_list.size(); j++) {
                if ((par_list[j].second->get_ele_type() != arg_list[j]->eleType) || (par_list[j].second->get_type() != arg_list[j]->type)) {
                    struct type_info* t = new struct type_info;
                    t->type = par_list[j].second->get_type();
                    t->eleType = par_list[j].second->get_ele_type();
                    sketch_compatible(t, arg_list[j], flag_type::call_stmt);
                }
            }
            if (j == par_list.size())
                return true;
        }
    }
    return false;
}