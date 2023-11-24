#include <bits/stdc++.h>
#include <vector>
#include "semantic.hpp"
#include "sym_tab.hpp"
#include "utils.hpp"

#define THRESHOLD 3

extern void yyerror(const char *s);
extern int lineno;


/// @brief vector of inbuilt functions
std::vector<std::string> inbuilt_functions = {"blur", "sharpen", "sobel", "T", "vflip", 
                                "hflip", "pixelate", "invert", "noise", "bnw",
                                "get_pixel", "set_pixel", "convolve", "paint", "frame",
                                "play", "get_num_frames", "concat_frame", "get_height", "get_width",
                                "grayscale", "clip", "crystallize", "to_image", "draw", "get_fps", "get_frame",
                                 "set_fps", "set_frame"};

std::vector<std::string> inbuilt_sketches = {"penup", "pendown", "set_pen_color", "circle", "arc", "rotate",
                                        "forward", "go_to"};

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

    return ELETYPE::ELE_ERROR;
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

bool array_compatibility(std::vector<int> &v1, std::vector<int> &v2) {
    if (v1.size() != v2.size()) {
        return false;
    }
    for(auto i = 0; i < v1.size(); i++) {
        if (v1[i] != -1 && v2[i] != -1) {
            if (v1[i] != v2[i]) {
                yyerror("Array dimensions mismatch");
                return false;
            }
        }
    }
    return true;
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
                    lineno--; // why?
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
// only num real bool allowed in array
struct type_info* sketch_compatible(struct type_info* t1, struct type_info* t2, flag_type flag){
    struct type_info* t_return = new struct type_info;
    if (t1->type == t2->type && t1->type == TYPE::SIMPLE) {
        if (is_primitive(t1->eleType)) {
            if (is_img(t2->eleType) || is_vid(t2->eleType)) {
                if (flag == flag_type::assignment)
                    yyerror("Cannot assign img/video to primitive");
                else if (flag == flag_type::call_stmt)
                    yyerror("Cannot pass img/video to primitive");
            }
            t_return->type = TYPE::SIMPLE;
            t_return->eleType = t1->eleType;
            return t_return;
        }
    }
    yyerror("Cannot pass incompatible types, only primitive types allowed");
    exit(1);
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
        std::string err = "Function"+ func_name + "doesn't match any declaration";
        yyerror(err.c_str());
        exit(1);
    }
    
    if (!compare_par_list_arg_list(func_list, *arg_vec))
    {
        
        std::string err = "Function Call Arguments for " + func_name  +" doesn't match any function declaration";
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
        std::string err = "Function"+ func_name +"doesn't match any declaration";
        yyerror(err.c_str());
        exit(1);
    }

    std::vector<struct type_info*> *arg_vec = new std::vector<struct type_info*>;

    if (!compare_par_list_arg_list((SymbolTableFunction->get_function(func_name)), *arg_vec))
    {
        std::string err = "Function Call Arguments for " + func_name  +" doesn't match any function declaration";
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

struct type_info* check_sketch_call(symbol_table_sketch* SymbolTableSketch, std::string func_name, std::vector<struct type_info*> *arg_vec){
    std::vector<sketch_record*> sketch_list = SymbolTableSketch->get_sketch(func_name);
    if (sketch_list.empty())
    {
        std::string err = "Sketch"+ func_name +"doesn't match any declaration";
        yyerror(err.c_str());
        exit(1);
    }
    
    if (!compare_par_list_arg_list(sketch_list, *arg_vec))
    {
        
        std::string err = "Sketch Call Arguments for"+func_name +"doesn't match any sketch declaration";
        yyerror(err.c_str());
        
    }
    struct type_info* t_return = new struct type_info;
    t_return->type = TYPE::SIMPLE;
    t_return->eleType = ELETYPE::ELE_IMG;
    t_return->dim_list = new std::vector<int>(3);
    t_return->dim_list->at(0) = -1; 
    t_return->dim_list->at(1) = -1;
    t_return->dim_list->at(2) = 3;
    return t_return;
}

struct type_info* check_sketch_call(symbol_table_sketch* SymbolTableSketch, std::string func_name) { 
    if ((SymbolTableSketch->get_sketch(func_name)).empty())
    {
        std::string err = "Sketch"+func_name +"doesn't match any declaration";
        yyerror(err.c_str());
        exit(1);
    }

    struct type_info* t_return = new struct type_info;
    t_return->type = TYPE::SIMPLE;
    t_return->eleType = ELETYPE::ELE_IMG;
    t_return->dim_list = new std::vector<int>(3);
    t_return->dim_list->at(0) = -1;
    t_return->dim_list->at(1) = -1;
    t_return->dim_list->at(2) = 3;
    return t_return;
}
/* ---------------------------------------------------------- 
 * In-built function call
------------------------------------------------------------ */

struct type_info* check_inbuilt_func_call(struct type_info* ti, std::string func_name, std::vector<struct type_info*> *arg_list) {
    auto const [idx, dist] = FindClosest(inbuilt_functions, func_name);
    struct type_info* t_error = new struct type_info;
    t_error->type = TYPE::SIMPLE;
    t_error->eleType = ELETYPE::ELE_ERROR;
    if (inbuilt_functions[idx] != func_name) {
        // arbitrary threshold of 7
        std::string err;
        if (dist < THRESHOLD) {
            err = func_name + " is not an inbuilt function; Did you mean " + inbuilt_functions[idx] + "?";
        } else {
            err = func_name + " is not an inbuilt function";
        }
        yyerror(err.c_str());
        // exit(1);
        return t_error;
    }

    // Takes 1 argument
    if (func_name == "pixelate" || func_name == "noise"){
            if (arg_list == NULL || arg_list->size() != 1) {
                std::string err = "in-built function " + func_name + " takes exactly 1 argument";
                yyerror(err.c_str());
                return t_error;
            }
            if (arg_list != NULL && (arg_list->at(0)->type != TYPE::SIMPLE && !is_primitive(arg_list->at(0)->eleType))) {
                std::string err = "in-built function " + func_name + " takes only primitive arguments (type will be casted to num)";
                yyerror(err.c_str());
                return t_error;
            }
            if (!is_img(ti->eleType)) {
                std::string err = "in-built function " + func_name + " can only be applied to images";
                yyerror(err.c_str());
                return t_error;
            }
            struct type_info* t_return = new struct type_info;
            t_return->type = TYPE::SIMPLE;
            t_return->eleType = ti->eleType;
            t_return->dim_list = new std::vector<int>(3);
            for (int i = 0; i < 3; i++) {
                t_return->dim_list->at(i) = ti->dim_list->at(i);
            }
            return t_return;
    }

    // Can have 0 or 1 argument
    else if (func_name == "blur" || func_name == "sharpen" || func_name == "bnw" || func_name == "crystallize") {
        if (arg_list != NULL && arg_list->size() != 1) {
            std::string err = "in-built function " + func_name + " takes 0 or 1 argument";
            yyerror(err.c_str());
            return t_error;
        }
        if (arg_list != NULL && arg_list->at(0)->type != TYPE::SIMPLE && !is_primitive(arg_list->at(0)->eleType)) {
            std :: string err = "in-built function " + func_name + " takes only primitive arguments (type will be casted to num)";
            yyerror(err.c_str());
            return t_error;
        }
        if (!is_img(ti->eleType)) {
            std::string err = "in-built function " + func_name + " can only be applied to images";
            yyerror(err.c_str());
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ti->eleType;
        t_return->dim_list = new std::vector<int>(3);

        for (int i = 0; i < 3; i++) {
            t_return->dim_list->at(i) = ti->dim_list->at(i);
        }
        return t_return;
    }
    else if (func_name == "T" || func_name == "invert" || func_name == "paint" || func_name == "sobel" || func_name == "vflip" || func_name == "hflip") {
        if (arg_list != NULL && arg_list->size() != 0) {
            std::string err = "in-built function " + func_name + " takes no arguments";
            yyerror(err.c_str());
            return t_error;
        }
        if (!is_img(ti->eleType)) {
            std::string err = "in-built function " + func_name + " can only be applied to images";
            yyerror(err.c_str());
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ti->eleType;
        t_return->dim_list = new std::vector<int>(3);
        for (int i = 0; i < 3; i++) {
                t_return->dim_list->at(i) = ti->dim_list->at(i);
        }
        if (func_name == "T") {
            int temp = t_return->dim_list->at(0);
            t_return->dim_list->at(0) = t_return->dim_list->at(1);
            t_return->dim_list->at(1) = temp;
        }

        // return void if paint
        if (func_name == "paint") {
            t_return->eleType = ELETYPE::ELE_VOID;
        }

        return t_return;
    }
    else if (func_name == "play") {
        if (arg_list != NULL && arg_list->size() != 0) {
            std::string err = "in-built function " + func_name + " takes no arguments";
            yyerror(err.c_str());
            return t_error;
        }
        if (!is_vid(ti->eleType)) {
            yyerror("in-built function play can only be applied to videos");
            // exit(1);
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_VOID;
        return t_return;
    }
    else if (func_name == "frame"){
        if (arg_list != NULL && arg_list->size() != 1) {
            yyerror("in-built function frame takes exactly 1 argument");
            return t_error;
        }
        if (!is_img(ti->eleType)) {
            yyerror("in-built function frame can only be applied to images");
            return t_error;
        }
        if (arg_list != NULL && (arg_list->at(0)->type != TYPE::SIMPLE || arg_list->at(0)->eleType != ELETYPE::ELE_STR)){
            yyerror("in-built function frame takes only string argument");
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_VOID;
        return t_return;
    }
    else if (func_name == "draw"){
        // if (arg_list == NULL || arg_list->size() != 2) {
        //     yyerror("in-built function draw takes exactly 2 arguments");
        //     return t_error;
        // }
        if (!is_img(ti->eleType)) {
            yyerror("in-built function draw can only be applied to images");
            return t_error;
        }
        if (arg_list!= NULL && (arg_list->at(0)->type != TYPE::SIMPLE || arg_list->at(0)->eleType != ELETYPE::ELE_STR)){
            yyerror("in-built function draw takes only string as first argument");
            return t_error;
        }
        // if "circle" -> 6 params
        // if "arc" -> 7 params
        // if "line" -> 5 params
        if (arg_list->at(0)->name == "\"circle\"" && arg_list->size() != 6){
                yyerror("in-built function draw takes 6 arguments for circle");
                return t_error;
        }
        if (arg_list->at(0)->name == "\"arc\"" && arg_list->size() != 7){
                yyerror("in-built function draw takes 7 arguments for arc");
                return t_error;
        }
        if (arg_list->at(0)->name == "\"line\"" && arg_list->size() != 5){
                yyerror("in-built function draw takes 5 arguments for line");
                return t_error;
        }
        if (arg_list->at(0)->name != "\"circle\"" && arg_list->at(0)->name != "\"arc\"" && arg_list->at(0)->name != "\"line\""){
                yyerror("in-built function draw takes only \"circle\", \"arc\" or \"line\" as first argument");
                return t_error;
        }

        // All params should be simple and primitive
        for (int i = 1; i < arg_list->size(); i++) {
            if (arg_list->at(i)->type != TYPE::SIMPLE || !is_primitive(arg_list->at(i)->eleType)) {
                yyerror("in-built function draw takes only primitive args (type will be casted to real)");
                return t_error;
            }
        }

        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ti->eleType;
        t_return->dim_list = new std::vector<int>(3);
        for (int i = 0; i < 3; i++) {
                t_return->dim_list->at(i) = ti->dim_list->at(i);
        }
        return t_return;
    }
    else if (func_name == "grayscale"){
        if (arg_list != NULL && arg_list->size() != 0) {
            yyerror("in-built function grayscale takes no arguments");
            return t_error;
        }
        if (ti->eleType != ELETYPE::ELE_IMG || ti->type != TYPE::SIMPLE) {
            yyerror("in-built function grayscale can only be applied to images");
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_GRAY_IMG;
        t_return->dim_list = new std::vector<int>(3);
        for (int i = 0; i < 3; i++) {
                t_return->dim_list->at(i) = ti->dim_list->at(i);
        }
        return t_return;
    }
    else if (func_name == "clip"){
        if (arg_list != NULL && arg_list->size() != 0) {
            yyerror("in-built function clip takes no arguments");
            return t_error;
        }
        if (ti->eleType != ELETYPE::ELE_IMG || ti->type != TYPE::SIMPLE) {
            yyerror("in-built function clip can only be applied to images");
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_IMG;
        t_return->dim_list = new std::vector<int>(3);
        for (int i = 0; i < 3; i++) {
                t_return->dim_list->at(i) = ti->dim_list->at(i);
        }
        return t_return;
    }
    else if (func_name == "get_height" || func_name == "get_width") {
        if (arg_list != NULL && arg_list->size() != 0) {
            std::string err = "in-built function " + func_name + " takes no arguments";
            yyerror(err.c_str());
            return t_error;
        }
        if (!is_img(ti->eleType) && !is_vid(ti->eleType)) {
            std::string err = "in-built function " + func_name + " can only be applied to images or videos";
            yyerror(err.c_str());
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_NUM;
        return t_return;
    }
    else if (func_name == "get_pixel") {
        if (arg_list == NULL || (arg_list->size() != 3 && arg_list->size() !=2 ) ) {
            yyerror("in-built function get_pixel takes 2 or 3 arguments");
            return t_error;
        }
        if (!is_img(ti->eleType)) {
            yyerror("in-built function get_pixel can only be applied to images");
            return t_error;
        }
        if (ti->eleType == ELETYPE::ELE_IMG && arg_list != NULL && arg_list->size() != 3) {
            yyerror("in-built function get_pixel takes 3 arguments for images");
            return t_error;
        }
        if (ti->eleType == ELETYPE::ELE_GRAY_IMG && arg_list != NULL && arg_list->size() != 2) {
            yyerror("in-built function get_pixel takes 2 arguments for gray images");
            return t_error;
        }
        if (arg_list != NULL && (arg_list->at(0)->type != TYPE::SIMPLE || arg_list->at(0)->eleType != ELETYPE::ELE_NUM)){
            yyerror("in-built function get_pixel takes only num as first argument");
            return t_error;
        }
        if (arg_list != NULL && (arg_list->at(1)->type != TYPE::SIMPLE || arg_list->at(1)->eleType != ELETYPE::ELE_NUM)){
            yyerror("in-built function get_pixel takes only num as second argument");
            return t_error;
        }
        if (arg_list != NULL && ti->eleType == ELETYPE::ELE_IMG && (arg_list->at(2)->type != TYPE::SIMPLE || arg_list->at(2)->eleType != ELETYPE::ELE_NUM)){
            yyerror("in-built function get_pixel takes only num as third argument");
            return t_error;
        }

        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_NUM;
        return t_return;
    }
    else if (func_name == "set_pixel") {
        if (arg_list == NULL || (arg_list->size() != 3 && arg_list->size() !=4 )) {
            yyerror("in-built function set_pixel takes 3 or 4 arguments");
            return t_error;
        }
        if (!is_img(ti->eleType)) {
            yyerror("in-built function get_pixel can only be applied to images");
        }
        if (ti->eleType == ELETYPE::ELE_IMG && arg_list != NULL && arg_list->size() != 4) {
            yyerror("in-built function get_pixel takes 4 arguments for images");
            return t_error;
        }
        if (ti->eleType == ELETYPE::ELE_GRAY_IMG && arg_list != NULL && arg_list->size() != 3) {
            yyerror("in-built function get_pixel takes 3 arguments for gray images");
            return t_error;
        }
        if (arg_list != NULL && (arg_list->at(0)->type != TYPE::SIMPLE || arg_list->at(0)->eleType != ELETYPE::ELE_NUM)){
            yyerror("in-built function get_pixel takes only num as first argument");
            return t_error;
        }
        if (arg_list != NULL && (arg_list->at(1)->type != TYPE::SIMPLE || arg_list->at(1)->eleType != ELETYPE::ELE_NUM)){
            yyerror("in-built function get_pixel takes only num as second argument");
            return t_error;
        }
        if (arg_list != NULL && (arg_list->at(2)->type != TYPE::SIMPLE || arg_list->at(2)->eleType != ELETYPE::ELE_NUM)){
            yyerror("in-built function get_pixel takes only num as third argument");
            return t_error;
        }
        if (arg_list != NULL && ti->eleType == ELETYPE::ELE_IMG && (arg_list->at(3)->type != TYPE::SIMPLE || arg_list->at(3)->eleType != ELETYPE::ELE_NUM)){
            yyerror("in-built function get_pixel takes only num as fourth argument");
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_VOID;
        return t_return;
    }
    else if (func_name == "to_image") {
        if (arg_list != NULL && arg_list->size() != 0) {
            std::string err = "in-built function " + func_name + " takes no arguments";
            yyerror(err.c_str());
            return t_error;
        }
        if (ti->eleType != ELETYPE::ELE_IMG) {
            yyerror("in-built function to_image can only be applied to images");
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_IMG;
        t_return->dim_list = new std::vector<int>(3);
        for (int i = 0; i < 3; i++) {
                t_return->dim_list->at(i) = ti->dim_list->at(i);
        }
        return t_return;
    }
    else if (func_name == "get_fps" || func_name == "get_num_frames"){
        if (arg_list != NULL && arg_list->size() != 0) {
            std::string err = "in-built function " + func_name + " takes no arguments";
            yyerror(err.c_str());
            return t_error;
        }
        if (!is_vid(ti->eleType)) {
            std::string err = "in-built function " + func_name + " can only be applied to videos";
            yyerror(err.c_str());
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_NUM;
        return t_return;
    }
    else if (func_name == "get_frame"){
        if (arg_list == NULL || arg_list->size() != 1) {
            yyerror("in-built function get_frame takes exactly 1 argument");
            return t_error;
        }
        if (!is_vid(ti->eleType)) {
            yyerror("in-built function get_frame can only be applied to videos");
            return t_error;
        }
        if (arg_list != NULL && (arg_list->at(0)->type != TYPE::SIMPLE || arg_list->at(0)->eleType != ELETYPE::ELE_NUM)){
            yyerror("in-built function get_frame takes only num as argument");
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->dim_list = new std::vector<int>(3);
        for (int i = 0; i < 3; i++) {
                t_return->dim_list->at(i) = ti->dim_list->at(i);
        }
        if (ti->eleType == ELETYPE::ELE_VID)
            t_return->eleType = ELETYPE::ELE_IMG;
            
        else t_return->eleType = ELETYPE::ELE_GRAY_IMG;
        return t_return;
    }
    else if (func_name == "set_fps") {
        if (arg_list == NULL || arg_list->size() != 1) {
            yyerror("in-built function set_fps takes exactly 1 argument");
            return t_error;
        }
        if (!is_vid(ti->eleType)) {
            yyerror("in-built function set_fps can only be applied to videos");
            return t_error;
        }
        if (arg_list != NULL && (arg_list->at(0)->type != TYPE::SIMPLE || arg_list->at(0)->eleType != ELETYPE::ELE_NUM)){
            yyerror("in-built function set_fps takes only num as argument");
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_VOID;
        return t_return;
    }
    else if (func_name == "set_frame") {
        if (arg_list == NULL || arg_list->size() != 2) {
            yyerror("in-built function set_frame takes exactly 2 arguments");
            return t_error;
        }
        if (!is_vid(ti->eleType)) {
            yyerror("in-built function set_frame can only be applied to videos");
            return t_error;
        }
        if (arg_list != NULL && (arg_list->at(0)->type != TYPE::SIMPLE || arg_list->at(0)->eleType != ELETYPE::ELE_NUM)){
            yyerror("in-built function set_frame takes only num as first argument");
            return t_error;
        }
        if (ti->eleType == ELETYPE::ELE_GRAY_VID && arg_list != NULL && (arg_list->at(1)->type != TYPE::SIMPLE || arg_list->at(1)->eleType != ELETYPE::ELE_GRAY_IMG)){
            yyerror("in-built function set_frame should be applied to gray images");
            return t_error;
        }
        if (ti->eleType == ELETYPE::ELE_VID && arg_list != NULL && (arg_list->at(1)->type != TYPE::SIMPLE || arg_list->at(1)->eleType != ELETYPE::ELE_IMG)){
            yyerror("in-built function set_frame should be applied to images");
            return t_error;
        }
        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_VOID;
        return t_return;
    }
    
    // Only 1 argument (an image). Returns void
    else if (func_name == "concat_frame"){
        if (arg_list == NULL || arg_list->size() != 1) {
            yyerror("in-built function concat_frame takes exactly 1 argument");
            return t_error;
        }
        // ti should be a video
        if (!is_vid(ti->eleType)) {
            yyerror("in-built function concat_frame can only be applied to videos");
            return t_error;
        }
        // If gray video -> arg should be gray image; if video -> arg should be image
        if (ti->eleType == ELETYPE::ELE_GRAY_VID && arg_list != NULL && arg_list->at(0)->eleType != ELETYPE::ELE_GRAY_IMG) {
            yyerror("in-built function concat_frame should be applied to gray images");
            return t_error;
        }
        if (ti->eleType == ELETYPE::ELE_VID && arg_list != NULL && arg_list->at(0)->eleType != ELETYPE::ELE_IMG) {
            yyerror("in-built function concat_frame should be applied to images");
            return t_error;
        }

        struct type_info* t_return = new struct type_info;
        t_return->type = TYPE::SIMPLE;
        t_return->eleType = ELETYPE::ELE_VOID;
        return t_return;
    }

    
    return nullptr; //temp
}


void check_inbuilt_sketch_call(std::string func_name, std::vector<struct type_info*> *arg_list) {

    auto const [idx, dist] = FindClosest(inbuilt_sketches, func_name);
    if (inbuilt_sketches[idx] != func_name) {
        // arbitrary threshold of 7
        std::string err;
        if (dist < THRESHOLD) {
            err = func_name + " is not an inbuilt sketch; Did you mean " + inbuilt_sketches[idx] + "?";
        } else {
            err = func_name + " is not an inbuilt sketch";
        }
        yyerror(err.c_str());
        return;
    }

    // Takes 1 argument
    if (func_name == "forward" || func_name == "rotate" || func_name == "set_pen_color" || func_name == "circle" ){
        if (arg_list == NULL || arg_list->size() != 1) {
            std::string err = "in-built sketch function " + func_name + "takes exactly 1 argument";
            yyerror(err.c_str());
            return;
        }
        if (arg_list != NULL && (arg_list->at(0)->type != TYPE::SIMPLE || !is_primitive(arg_list->at(0)->eleType))) {
            std::string err = "in-built sketch function " + func_name + "takes only primitive arguments (type will be casted to num)";
            yyerror(err.c_str());
            return;
        }
    }
    else if (func_name == "arc"){
        if (arg_list == NULL || arg_list->size() != 2) {
            std::string err = "in-built sketch function " + func_name + "takes exactly 2 arguments";
            yyerror(err.c_str());
            return;
        }
        if (arg_list != NULL && (arg_list->at(0)->type != TYPE::SIMPLE || !is_primitive(arg_list->at(0)->eleType))) {
            std::string err = "in-built sketch function " + func_name + "takes only primitive arguments (type will be casted to num)";
            yyerror(err.c_str());
            return;
        }
    }
    // Only 2 arguments
    else if (func_name == "go_to"){
        if (arg_list == NULL || arg_list->size() != 2) {
            std::string err = "in-built sketch function " + func_name + "takes exactly 2 arguments";
            yyerror(err.c_str());
            return;
        }
        if (arg_list != NULL && (arg_list->at(0)->type != TYPE::SIMPLE || !is_primitive(arg_list->at(0)->eleType))) {
            std::string err = "in-built sketch function " + func_name + "takes only primitive arguments (type will be casted to num)";
            yyerror(err.c_str());
            return;
        }
        if (arg_list != NULL && (arg_list->at(1)->type != TYPE::SIMPLE || !is_primitive(arg_list->at(1)->eleType))) {
            std::string err = "in-built sketch function " + func_name + "takes only primitive arguments (type will be casted to num)";
            yyerror(err.c_str());
            return;
        }
    }
}