#include <bits/stdc++.h>
#include <vector>
#include "semantic.hpp"
#include "sym_tab.hpp"
#include "utils.hpp"


extern void yyerror(const char *s);
extern int lineno;


/// @brief vector of inbuilt functions
std::vector<std::string> inbuilt_functions = {"blur", "sharpen", "sobel", "T", "vflip", 
                                "hflip", "pixelate", "invert", "noise", "bnw",
                                "get", "set", "convolve", "paint", "frame",
                                "play", "len", "append", "height", "width", "draw"};

/* ---------------------------------------------------------- 
 * Helper functions
------------------------------------------------------------ */

/// @brief returns the type of the result of binary operation based on the order of precedence
ELETYPE get_type(ELETYPE t1, ELETYPE t2) {
    if (t1 == ELETYPE::ELE_ERROR || t2 == ELETYPE::ELE_ERROR) 
    {
        yyerror("Operands are not compatible");
        
    } 
    else if (t1 == ELETYPE::ELE_VOID || t2 == ELETYPE::ELE_VOID) 
    {
        yyerror("void type cannot be used in binary operation");
        
    } 
    else if (t1 == ELETYPE::ELE_VID || t2 == ELETYPE::ELE_VID) 
    {
        return ELETYPE::ELE_VID;
    } 
    else if (t1 == ELETYPE::ELE_GRAY_VID || t2 == ELETYPE::ELE_GRAY_VID) 
    {
        if (t1 == ELETYPE::ELE_IMG || t2 == ELETYPE::ELE_IMG) 
        {
            return ELETYPE::ELE_VID;
        } 
        else 
        {
            return ELETYPE::ELE_GRAY_VID;
        }
    } 
    else if (t1 == ELETYPE::ELE_IMG || t2 == ELETYPE::ELE_IMG) 
    {
        return ELETYPE::ELE_IMG;
    } 
    else if (t1 == ELETYPE::ELE_GRAY_IMG || t2 == ELETYPE::ELE_GRAY_IMG) 
    {
        return ELETYPE::ELE_GRAY_IMG;
    } 
    else if (t1 == ELETYPE::ELE_REAL || t2 == ELETYPE::ELE_REAL) 
    {
        return ELETYPE::ELE_REAL;
    } 
    else if (t1 == ELETYPE::ELE_NUM || t2 == ELETYPE::ELE_NUM) 
    {
        return ELETYPE::ELE_NUM;
    } 
    else if (t1 == ELETYPE::ELE_BOOL || t2 == ELETYPE::ELE_BOOL) 
    {
        return ELETYPE::ELE_BOOL;
    } 
    else 
    {
        return ELETYPE::ELE_ERROR;
    }
}

/// @brief returns true if eletype is a num/real/bool
bool is_primitive(ELETYPE t) {
    if (t == ELETYPE::ELE_NUM || t == ELETYPE::ELE_REAL || t == ELETYPE::ELE_BOOL) {
        return true;
    } else {
        return false;
    }
}

/// @brief returns true if eletype is a img/grayimg
bool is_img(ELETYPE t) {
    if (t == ELETYPE::ELE_IMG || t == ELETYPE::ELE_GRAY_IMG) {
        return true;
    } else {
        return false;
    }
}

/// @brief returns true if eletype is a vid/grayvid
bool is_vid(ELETYPE t) {
    if (t == ELETYPE::ELE_VID || t == ELETYPE::ELE_GRAY_VID) {
        return true;
    } else {
        return false;
    }
}

/// Checks if any dimension is undefined 
bool is_dim_undefined(std::vector<int> &v, int len) {
    assert (len <= v.size());
    for(auto i = 0; i < len; i++) {
        if (v[i] == -1) {
            return true;
        }
    }
    return false;
}

// Credit: https://stackoverflow.com/questions/70236962/how-do-i-find-the-closest-match-to-a-string-key-within-a-c-map
size_t Levenstein(std::string_view const & a, std::string_view const & b) {
    // https://en.wikipedia.org/wiki/Levenshtein_distance
    if (b.size() == 0)
        return a.size();
    if (a.size() == 0)
        return b.size();
    if (a[0] == b[0])
        return Levenstein(a.substr(1), b.substr(1));
    return 1 + std::min(
        std::min(
            Levenstein(a          , b.substr(1)),
            Levenstein(a.substr(1), b          )
        ),  Levenstein(a.substr(1), b.substr(1))
    );
}

// Credit: https://stackoverflow.com/questions/70236962/how-do-i-find-the-closest-match-to-a-string-key-within-a-c-map
std::tuple<size_t, size_t> FindClosest(std::vector<std::string> const & strs, std::string const & query) {
    size_t minv = size_t(-1), mini = size_t(-1);
    for (size_t i = 0; i < strs.size(); ++i) {
        size_t const dist = Levenstein(strs[i], query);
        if (dist < minv) {
            minv = dist;
            mini = i;
        }
    }
    
    return std::make_tuple(mini, minv);
}

/* ---------------------------------------------------------- 
 * Binary Compatibility
------------------------------------------------------------ */

// + -
// * /
// ^ 
/**
*    ELE_NUM -> can be binoped to everything except video
*    ELE_REAL -> can be binoped to everything except video (casted to int for img/grayimg)
*    ELE_BOOL -> can be binoped to everything except video (casted to int for img/grayimg)
*
*    ELE_IMG -> can be binoped to everything (can only be added to videos)
*    ELE_GRAY_IMG -> can be binoped to everything (can only be added to videos)
*    
*    ELE_VID -> vid + grayvid/vid, vid + grayimg/img
*    ELE_GRAY_VID -> grayvid + vid/grayvid, (grayvid + gray/img) -> greyvid
*
*    ELE_VID > ELE_GRAY_VID > ELE_IMG > ELE_GRAY_IMG > ELE_REAL > ELE_NUM > ELE_BOOL
*
*/

/// @brief check if the two operands are compatible with an binary operator (+, -, /, *, ^)
struct type_info* binary_compatible(struct type_info* t1, struct type_info* t2, OPERATOR op) {
    struct type_info* t_return = new struct type_info;
    if (t1->type == t2->type && t1->type == TYPE::SIMPLE) {
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = get_type(t1->eleType, t2->eleType);
        if (is_primitive(t1->eleType) && is_vid(t2->eleType) || is_primitive(t2->eleType) && is_vid(t1->eleType)) 
        {
            yyerror("Cannot perform binary operation on primitive and video");
            
        }

        if ((op != OPERATOR::ADD) && (is_vid(t1->eleType) || is_vid(t2->eleType))) 
        {
            yyerror("Only + is defined for videos");
            
        }

        if (is_primitive(t1->eleType) && is_primitive(t2->eleType)) 
        {
            return t_return;
        } 
        else if ((is_primitive(t1->eleType) && is_img(t2->eleType)) || (is_primitive(t2->eleType) && is_img(t1->eleType))) 
        {
            t_return->eleType = get_type(t1->eleType, t2->eleType);

            std::vector<int>* dim_list_temp;
            is_img(t1->eleType) ? (dim_list_temp = t1->dim_list) : (dim_list_temp = t2->dim_list);
            t_return->dim_list = new std::vector<int>(dim_list_temp->size());

            for(auto i = 0; i < dim_list_temp->size(); i++) {
                t_return->dim_list->at(i) = dim_list_temp->at(i);
            }
            
            return t_return;
        }
        else if (is_img(t1->eleType) && is_img(t2->eleType))
        {
            if (!(is_dim_undefined(*(t1->dim_list), 2) || is_dim_undefined(*(t2->dim_list), 2))) {
                for (auto i = 0; i < 2; i++) 
                {
                    if (t1->dim_list->at(i) != t2->dim_list->at(i)) 
                    {
                        yyerror("Cannot perform binary operation on images of different dimensions (hxw)");
                        
                    }
                }
            }
            t_return->eleType = get_type(t1->eleType, t2->eleType);
            t_return->dim_list = new std::vector<int>(3);
            t_return->dim_list->at(0) = t1->dim_list->at(0); t_return->dim_list->at(1) = t1->dim_list->at(1);
            if (t_return->eleType == ELETYPE::ELE_IMG)
                t_return->dim_list->at(2) = 3;
            else
                t_return->dim_list->at(2) = 1;
            
            return t_return;
        }
        else if ((is_img(t1->eleType) && is_vid(t2->eleType)) || (is_img(t2->eleType) && is_vid(t1->eleType))) 
        {
            if (!(is_dim_undefined(*(t1->dim_list), 2) || is_dim_undefined(*(t2->dim_list), 2))) {
                for (auto i = 0; i < 2; i++) 
                {
                    if (t1->dim_list->at(i) != t2->dim_list->at(i)) 
                    {
                        yyerror("Cannot perform binary operation on images and videos of different dimensions (hxw)");
                        
                    }
                }
            }

            std::vector<int>* dim_list_temp_vid;
            is_vid(t1->eleType) ? (dim_list_temp_vid = t1->dim_list) : (dim_list_temp_vid = t2->dim_list);

            t_return->eleType = get_type(t1->eleType, t2->eleType);
            t_return->dim_list = new std::vector<int>(4);
            t_return->dim_list->at(0) = t1->dim_list->at(0); t_return->dim_list->at(1) = t1->dim_list->at(1); t_return->dim_list->at(3) = dim_list_temp_vid->at(3);
            if (t_return->eleType == ELETYPE::ELE_VID) 
                t_return->dim_list->at(2) = 3;
            else
                t_return->dim_list->at(2) = 1;
            
            return t_return;
        }
        else if (is_vid(t1->eleType) && is_vid(t2->eleType)) 
        {
            
            if (!(is_dim_undefined(*(t1->dim_list), 2) || is_dim_undefined(*(t2->dim_list), 2)))    {
                if(!((t1->dim_list->at(0) == t2->dim_list->at(0)) && (t1->dim_list->at(1) == t2->dim_list->at(1)))) 
                {
                    yyerror("Videos are not compatible. They must have same h and w");
                    
                }
            } 
            if(t1->dim_list->at(3) != -1 && t2->dim_list->at(3) != -1) {
                if (t1->dim_list->at(3) != t2->dim_list->at(3)) {
                    yyerror("Videos are not compatible. They must have same frame rate");
                    
                }
            }

            t_return->dim_list = new std::vector<int>(4);
            t_return->dim_list->at(0) = t1->dim_list->at(0); t_return->dim_list->at(1) = t1->dim_list->at(1); t_return->dim_list->at(3) = t1->dim_list->at(3);
            if (t_return->eleType == ELETYPE::ELE_VID) 
                t_return->dim_list->at(2) = 3;
            else
                t_return->dim_list->at(2) = 1;
            
            return t_return;
        }

    } 
    else if (t1->type == t2->type && t1->type == TYPE::ARRAY) 
    { // arrays always need to have same dimlist size
        if (t1->dim_list->size() == t2->dim_list->size()) 
        {
            t_return->type = TYPE::ARRAY;
            t_return->eleType = get_type(t1->eleType, t2->eleType);
            t_return->dim_list = new std::vector<int>(t1->dim_list->size() - 1);

            if (!is_primitive(t1->eleType) || !is_primitive(t2->eleType)) 
            {
                yyerror("Cannot perform binary operation on non-primitive arrays");
                
            }
        } 
        else 
        {
            yyerror("Cannot perform binary operation on arrays of different dimensions");
            
        }
    } 
    else if (t2->type == TYPE::ARRAY && t1->type == TYPE::SIMPLE || t2->type == TYPE::SIMPLE && t1->type == TYPE::ARRAY) 
    { // arr with simple, simple has to be only num/real/bool
        if (!is_primitive(t1->eleType) || !is_primitive(t2->eleType)) {
            yyerror("Cannot perform binary operation on arrays and non-primitive");
            
        }

        t_return->type = TYPE::ARRAY;
        t_return->eleType = get_type(t1->eleType, t2->eleType);
        std::vector<int>* temp_dim_list;
        t1->type == TYPE::ARRAY ? (temp_dim_list = t1->dim_list) : (temp_dim_list = t2->dim_list);
        t_return->dim_list = new std::vector<int>(temp_dim_list->size() - 1);
    }

    return t_return;
}

/* ----------------------------------------------------------*
 * Relational Compatibility                                  *
 *---------------------------------------------------------- */
struct type_info* relational_compatible(struct type_info* t1, struct type_info* t2, OPERATOR op) {
    /*
        - since this is just for relational operators, we can just check if the types are primitive
        - since only primitives, no need to transfer dim_list
    */
    // if (op == OPERATOR::EQUAL) {
    //     if (!t1->type)
    // } 
    // else
    if (t1->type == TYPE::ARRAY || t2->type == TYPE::ARRAY || !is_primitive(t1->eleType) || !is_primitive(t2->eleType)) {
        yyerror("Can only perform logical/relational operations on numeric types");
        
    }
    struct type_info* t_return = new struct type_info;
    t_return->type = TYPE::SIMPLE;
    t_return->eleType = ELETYPE::ELE_BOOL;



    return t_return;
}



/* ---------------------------------------------------------- 
 * Unary Compatibility
------------------------------------------------------------ */

/**
*    ELE_NUM -> ++, -- defined
*    ELE_REAL ->  ++, -- defined
*    ELE_BOOL -> Nothing defined
*
*    ELE_IMG -> Only ~ defined
*    ELE_GRAY_IMG -> Only ~ defined
*    
*    ELE_VID ->  Only ~ defined
*    ELE_GRAY_VID ->  Only ~ defined
*
*    Type stays the same
*/


/// @brief check if the two operands are compatible with an unary operator (++, --, ~)
struct type_info* unary_compatible(struct type_info* t1, OPERATOR op, flag_type flag) {
    struct type_info* t_return = new struct type_info;
    t_return->eleType = t1->eleType;
    t_return->type = t1->type;
    if (t1->type == TYPE::SIMPLE) 
    {
        if (is_img(t1->eleType) || is_vid(t1->eleType))
        {
            if (op != OPERATOR::INV )
            {
                if (flag == flag_type::stmt)
                    lineno--;
                yyerror("Cannot perform postinc/postdec unary operation on img/video");
                lineno++;
                
            }
            t_return->dim_list = new std::vector<int>(t1->dim_list->size());
            for (int i = 0; i < t1->dim_list->size(); i++) {
                t_return->dim_list->at(i) = t1->dim_list->at(i);
            }
        }
        else if (t1->eleType == ELETYPE::ELE_BOOL) {
            if (flag == flag_type::stmt)
                lineno--;
            yyerror("Cannot perform unary operation on bool");
            lineno++;
            
        }
        else if (t1->eleType == ELETYPE::ELE_NUM || t1->eleType == ELETYPE::ELE_REAL) 
        {
            if (op == OPERATOR::INV) 
            {
                if (flag == flag_type::stmt)
                    lineno--;
                yyerror("Cannot perform invert unary operation on num/real");
                lineno++;
                
            }
            // t_return->dim_list = new std::vector<int>(0);
        }
    }
    else if (t1->type == TYPE::ARRAY)
    {
        if (flag == flag_type::stmt)
            lineno--;
        yyerror("Cannot perform unary operation on array");
        lineno++;
        
    }
    return t_return;
}


/* ---------------------------------------------------------*
 * Assignment Compatibility                      *
 *----------------------------------------------------------*/

/**
* ELE_NUM -> can be assigned to ELE_NUM, ELE_REAL, ELE_BOOL
* ELE_REAL -> can be assigned to ELE_REAL, ELE_NUM, ELE_BOOL
* ELE_BOOL -> can be assigned to ELE_BOOL, ELE_NUM, ELE_REAL

* ELE_IMG -> can be assigned to ELE_IMG, ELE_GRAY_IMG
* ELE_GRAY_IMG -> can be assigned to ELE_GRAY_IMG, ELE_IMG
* ELE_VID -> can be assigned to ELE_VID, ELE_GRAY_VID
* ELE_GRAY_VID -> can be assigned to ELE_GRAY_VID, ELE_VID
*/

/// @brief check if the two operands are compatible with an assignment operator (=)
struct type_info* assignment_compatible(struct type_info* t1, struct type_info* t2, flag_type flag) {
    struct type_info* t_return = new struct type_info;
    
    if (t1->type == t2->type && t1->type == TYPE::SIMPLE) 
    {
        if (is_primitive(t1->eleType)) 
        {
            if (is_img(t2->eleType) || is_vid(t2->eleType)) 
            {
                if (flag == flag_type::assignment)
                    yyerror("Cannot assign img/video to primitive");
                else if (flag == flag_type::call_stmt)
                    yyerror("Cannot pass img/video to primitive");
                // yyerror("Cannot assign img/video to primitive");
                
            }
            t_return->type = TYPE::SIMPLE;
            t_return->eleType = t1->eleType;
            // t_return->dim_list = new std::vector<int>(0);
        } 
        else if (is_img(t1->eleType) && is_img(t2->eleType)) 
        {
            if (!(is_dim_undefined(*(t1->dim_list), 2) || is_dim_undefined(*(t2->dim_list), 2))) {
                for (auto i = 0; i < 2; i++) 
                {
                    if (t1->dim_list->at(i) != t2->dim_list->at(i)) 
                    {
                        yyerror("Incompatible dimensions. Cannot perform assignment");
                        
                    }
                }
            }
            t_return->type = TYPE::SIMPLE;
            t_return->eleType = t1->eleType;
            std::vector<int> * dim_list_temp = new std::vector<int>(t1->dim_list->size());
            for (int i = 0; i < t1->dim_list->size(); i++) {
                dim_list_temp->at(i) = t1->dim_list->at(i);
            }
            t_return->dim_list = dim_list_temp;
        } 
        else if (is_vid(t1->eleType) && is_vid(t2->eleType)) 
        {

            if (!(is_dim_undefined(*(t1->dim_list), 2) || is_dim_undefined(*(t2->dim_list), 2)))    {
                if(!((t1->dim_list->at(0) == t2->dim_list->at(0)) && (t1->dim_list->at(1) == t2->dim_list->at(1)))) 
                {
                    yyerror("Videos are not compatible. They must have same h and w");
                    
                }
            } 
            if(t1->dim_list->at(3) != -1 && t2->dim_list->at(3) != -1) {
                if (t1->dim_list->at(3) != t2->dim_list->at(3)) {
                    yyerror("Videos are not compatible. They must have same frame rate");
                    
                }
            }
            t_return->type = TYPE::SIMPLE;
            t_return->eleType = t1->eleType;
            std::vector<int> * dim_list_temp = new std::vector<int>(t1->dim_list->size());
            for (int i = 0; i < t1->dim_list->size(); i++) {
                dim_list_temp->at(i) = t1->dim_list->at(i);
            }

            t_return->dim_list = dim_list_temp;
        } 
        else 
        {
            std::string s;
            lineno--;
            if (flag == flag_type::assignment)
                s = "Cannot assign incompatible types";
            else if (flag == flag_type::call_stmt)
                s = "Passed arguments are incompatible";
            yyerror(s.c_str());
            lineno++;
            
        }
    }
    else if (t1->type == t2->type && t1->type == TYPE::ARRAY) 
    {
        t_return->type = TYPE::ARRAY;
        t_return->eleType = t1->eleType;
        
        if (t1->dim_list->size() == t2->dim_list->size()){
            for (int i = 0; i < t1->dim_list->size(); i++) {
                if (t1->dim_list->at(i) != -1 && t1->dim_list->at(i) != t2->dim_list->at(i)) {
                    yyerror("Dimension values mismatch in assignment");
                    
                }
            }
            std::vector<int> * dim_list_temp = new std::vector<int>(t1->dim_list->size());
            for (int i = 0; i < t1->dim_list->size(); i++) {
                dim_list_temp->at(i) = t1->dim_list->at(i);
            }
        }
        else {
            yyerror("Dimension mismatch in assignment");
        }
    }
    else 
    {
        std::string s;
        lineno--;
        if (flag == flag_type::assignment)
            s = "Cannot assign incompatible types";
        else if (flag == flag_type::call_stmt)
            s = "Passed arguments are incompatible";
        yyerror(s.c_str());
        lineno++;
        
    }
    
    return t_return;
}

/* ---------------------------------------------------------*
 * Dimension List Compatibility                      *
 *----------------------------------------------------------*/

/**
* only ELE_NUM, ELE_REAL, ELE_BOOL can be used as dimension list
*/
// struct type_info* dim_list_compatible(struct type_info* t1, struct type_info* t2) {
//     struct type_info* t_return = new struct type_info;
//     if (!is_primitive(t1->eleType) || !is_primitive(t2->eleType)) {
//         yyerror("Cannot use non-primitive types as dimension list");
//         
//     }
//     // TODO
// }

/* ---------------------------------------------------------- 
 * Function calls
------------------------------------------------------------ */


struct type_info* check_func_call(symbol_table_function* SymbolTableFunction, std::string func_name, std::vector<struct type_info*> *arg_vec) { 
    std::vector<function_record*> func_list = SymbolTableFunction->get_function(func_name);
    if (func_list.empty())
    {
        std::string err = "Function doesn't match any declaration";
        yyerror(err.c_str());
        
    }
    
    if (!compare_par_list_arg_list(func_list, *arg_vec))
    {
        
        std::string err = "Function Call Arguments doesn't match any function declaration";
        yyerror(err.c_str());
        
    }
    struct type_info* t_return = new struct type_info;
    t_return->type = TYPE::SIMPLE;
    t_return->eleType = func_list[0]->get_return_type();
    if (is_img(t_return->eleType)) {
        t_return->dim_list = new std::vector<int>(3);
        t_return->dim_list->at(0) = -1; 
        t_return->dim_list->at(1) = -1;
        if (t_return->eleType == ELETYPE::ELE_IMG)
            t_return->dim_list->at(2) = 3;
        else
            t_return->dim_list->at(2) = 1;
    }
    else if (is_vid(t_return->eleType)) {
        t_return->dim_list = new std::vector<int>(4);
        t_return->dim_list->at(0) = -1; t_return->dim_list->at(1) = -1; t_return->dim_list->at(3) = -1;
        if (t_return->eleType == ELETYPE::ELE_VID)
            t_return->dim_list->at(2) = 3;
        else
            t_return->dim_list->at(2) = 1;
    }
    return t_return;
}

struct type_info* check_func_call(symbol_table_function* SymbolTableFunction, std::string func_name) { //Archit
    if ((SymbolTableFunction->get_function(func_name)).empty())
    {
        std::string err = "Function doesn't match any declaration";
        yyerror(err.c_str());
        
    }

    struct type_info* t_return = new struct type_info;
    t_return->type = TYPE::SIMPLE;
    t_return->eleType = SymbolTableFunction->get_function(func_name)[0]->get_return_type();
    if (is_img(t_return->eleType)) {
        t_return->dim_list = new std::vector<int>(3);
        t_return->dim_list->at(0) = -1; t_return->dim_list->at(1) = -1;
        if (t_return->eleType == ELETYPE::ELE_IMG)
            t_return->dim_list->at(2) = 3;
        else
            t_return->dim_list->at(2) = 1;
    }
    else if (is_vid(t_return->eleType)) {
        t_return->dim_list = new std::vector<int>(4);
        t_return->dim_list->at(0) = -1; t_return->dim_list->at(1) = -1; t_return->dim_list->at(3) = -1;
        if (t_return->eleType == ELETYPE::ELE_VID)
            t_return->dim_list->at(2) = 3;
        else
            t_return->dim_list->at(2) = 1;
    }
    return t_return;
}

/* ---------------------------------------------------------- 
 * In-built function call
------------------------------------------------------------ */

struct type_info* check_inbuilt_func_call(struct type_info* ti, std::string func_name, std::vector<struct type_info*> *arg_list) {
    auto const [idx, dist] = FindClosest(inbuilt_functions, func_name);
    if (inbuilt_functions[idx] != func_name) {
        // arbitrary threshold of 7
        std::string err;
        if (dist < 7) {
            err = func_name + " is not an inbuilt function; Did you mean " + inbuilt_functions[idx] + "?";
        } else {
            err = func_name + " is not an inbuilt function";
        }
        yyerror(err.c_str());
        
    }

    if (func_name == "blur" || func_name == "sharpen" || func_name == "pixelate" || func_name == "noise"){
            if (arg_list->size() != 1) {
                yyerror("in-built function takes exactly 1 argument");
                
            }
            if (arg_list->at(0)->type != TYPE::SIMPLE && !is_primitive(arg_list->at(0)->eleType)) {
                yyerror("in-built function takes only primitive arguments (type will be casted to num)");
                
            }
            if (!is_img(ti->eleType)) {
                yyerror("in-built function can only be applied to images");
                
            }
    }
    else if (func_name == "T" || func_name == "invert" || func_name == "paint" || func_name == "sobel" || func_name == "vflip" || func_name == "hflip") {
        if (arg_list->size() != 0) {
                yyerror("in-built function takes no arguments");
                
            }
        if (!is_img(ti->eleType)) {
            yyerror("in-built function can only be applied to images");
            
        }
    }
    else if (func_name == "bnw") {
        if (arg_list->size() != 1) {
                yyerror("in-built function takes exactly 1 argument");
                
        }
        if (arg_list->at(0)->eleType != ELETYPE::ELE_GRAY_IMG) {
            yyerror("in-built function can only be applied to gray images");
            
        }
    }
    else if (func_name == "play") {
        if (arg_list->size() != 0) {
                yyerror("in-built function takes no arguments");
                
        }
        if (!is_vid(ti->eleType)) {
            yyerror("in-built function can only be applied to videos");
            
        }
    }
    return nullptr; //temp
}