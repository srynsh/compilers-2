#ifndef SYM_TAB_HPP
#define SYM_TAB_HPP

#include <bits/stdc++.h>

/// @brief  Class used for identifier type in identifier table
enum class TYPE{
    SIMPLE,
    ARRAY
};

/// @brief Class used for element type in identifier table
enum class ELETYPE{
    ELE_NUM,
    ELE_REAL,
    ELE_BOOL,
    ELE_IMG,
    ELE_GRAY_IMG,
    ELE_VID,
    ELE_GRAY_VID,
    ELE_VOID
};

/*------------------------------------------------------------------------*
 * Symbol Table                                                           *
 *------------------------------------------------------------------------*/

/// @brief Entry in the symbol table for identifier
class data_record {
    
    private:
        std::string name;
        TYPE type; // simple or array
        ELETYPE ele_type;
        std::vector<int> dimlist; 
        int scope; // scope of the identifier 0 for globals, 1 for parameters etc.
    
    public:
        data_record(){}
        data_record(std::string name, TYPE type, ELETYPE ele_type, std::vector<int>& dimlist, int scope);
        std::string get_name();
        TYPE get_type();
        ELETYPE get_ele_type();
        std::vector<int> get_dimlist();
        int get_scope();
        void print();
        ~data_record();
};

class symbol_table_variable {

    private:
        std::unordered_map<std::string, data_record*> variable_list; // map from name and scope to data_record
    
    public:
        symbol_table_variable(){}

        void add_variable(std::string name, TYPE type, ELETYPE ele_type, std::vector<int>& dimlist, int scope);
        data_record* get_variable(std::string name, int scope);  

        ~symbol_table_variable();
};

/// @brief Symbol table for functions
class function_record {

    private:
        std::string name; // function name
        ELETYPE return_type;
        std::unordered_map<std::string, data_record*> parameter_list;

    public:
        function_record(){}
        function_record(std::string name, ELETYPE return_type);

        std::string get_name();
        ELETYPE get_return_type();
        data_record* get_parameter(std::string name);

        void add_parameter(std::string name, TYPE type, ELETYPE ele_type, std::vector<int>& dimlist, int scope);
        void print();
};


class symbol_table_function {

    private:
        std::string prev_name;
        ELETYPE prev_return_type;
        std::unordered_map<std::string, function_record*> function_list; // map from name to function_record

    public:
        symbol_table_function(){}

        void add_function(std::string name, ELETYPE return_type);
        function_record* get_function(std::string name);

        void print();

        ~symbol_table_function();
};


#endif // SYM_TAB_HPP