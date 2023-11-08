#ifndef SYM_TAB_HPP
#define SYM_TAB_HPP

#include <bits/stdc++.h>

/*------------------------------------------------------------------------*
 * Type Information                                                       *
 *------------------------------------------------------------------------*/

 /* Forward declaration */
struct type_info;

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
 * Symbol Table for Variables                                             *
 *------------------------------------------------------------------------*/

/// @brief Entry in the symbol table for identifier
class data_record {
    
    private:
        std::string name;
        TYPE type; // simple or array
        ELETYPE ele_type;
        std::vector<int> dim_list; 
        int scope; // scope of the identifier 0 for globals, 1 for parameters etc.
    
    public:
        data_record(){}
        data_record(std::string name, TYPE type, ELETYPE ele_type, std::vector<int>& dim_list, int scope);
        data_record(std::string name, TYPE type, ELETYPE ele_type, int scope);
        std::string get_name();
        TYPE get_type();
        ELETYPE get_ele_type();
        std::vector<int> get_dim_list();
        int get_scope();
        void print();
};

class symbol_table_variable {

    private:
        std::unordered_map<std::string, data_record*> variable_list; // map from name and scope (space-seperated) to data_record
    
    public:
        symbol_table_variable(){}

        void add_variable(std::string name, TYPE type, ELETYPE ele_type, std::vector<int>& dim_list, int scope);
        void add_variable(std::string name, TYPE type, ELETYPE ele_type, int scope);
        void add_variable(std::vector<std::string>& names, std::vector<TYPE>& types, std::vector<ELETYPE>& ele_types, std::vector<std::vector<int> >& dim_lists, int scope);
        data_record* get_variable(std::string name, int scope);  
        void delete_variable(int scope);

        ~symbol_table_variable();
};

/*------------------------------------------------------------------------*
 * Symbol Table for Functions                                             *
 *------------------------------------------------------------------------*/

/// @brief Entry in the symbol table for function
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

        void add_parameter(std::string* name, TYPE type, ELETYPE ele_type, std::vector<int>*dim_list);
        void add_parameter(std::string* name, TYPE type, ELETYPE ele_type);
        // void add_parameter(std::vector<std::string>& names, std::vector<TYPE>& types, std::vector<ELETYPE>& ele_types, std::vector<std::vector<int>>& dim_lists);
        void print();
};


class symbol_table_function {

    private:
        std::string current_func_name;
        ELETYPE current_return_type;
        std::unordered_map<std::string, function_record*> function_list; // map from name to function_record

    public:
        symbol_table_function(){}

        void add_function_record(std::string name, ELETYPE return_type); 
        void add_parameter(std::string name, ELETYPE return_type, std::vector<std::pair<std::string, type_info*> > *par_vec);
        function_record* get_function(std::string name);

        std::string get_current_func_name(){ return current_func_name; };
        ELETYPE get_current_return_type(){ return current_return_type; };

        void print();

        ~symbol_table_function();
};


#endif // SYM_TAB_HPP