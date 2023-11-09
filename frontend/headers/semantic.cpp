#include <bits/stdc++.h>
#include <vector>
#include "semantic.hpp"
#include "sym_tab.hpp"

extern void yyerror(const char *s);

/* ---------------------------------------------------------- 
 * Operator Compatibility Checks
------------------------------------------------------------ */

// + -
// * /
// ^ 
/**
*    ELE_NUM -> can be binoped to everything except video
*    ELE_REAL -> can be binoped to everything except video
*    ELE_BOOL -> can be binoped to everything except video
*
*    ELE_IMG -> can be binoped to everything (can only be added to videos)
*    ELE_GRAY_IMG -> can be binoped to everything (can only be added to videos)
*    
*    ELE_VID -> vid + grayvid/vid, vid + gray/img
*    ELE_GRAY_VID -> grayvid + vid/grayvid, (grayvid + gray/img) -> greyvid
*
*    ELE_VID > ELE_GRAY_VID > ELE_IMG > ELE_GRAY_IMG > ELE_REAL > ELE_NUM > ELE_BOOL
*
*/

/// @brief returns the type of the result of binary operation based on the order of precedence
ELETYPE get_type(ELETYPE t1, ELETYPE t2) {
    if (t1 == ELETYPE::ELE_ERROR || t2 == ELETYPE::ELE_ERROR) 
    {
        yyerror("Operands are not compatible");
        exit(1);
    } 
    else if (t1 == ELETYPE::ELE_VOID || t2 == ELETYPE::ELE_VOID) 
    {
        yyerror("void type cannot be used in binary operation");
        exit(1);
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

/// @brief check if the two operands are compatible with an binary operator (+, -, /, *, ^)
struct type_info* binary_compatible(struct type_info* t1, struct type_info* t2, OPERATOR op) {
    struct type_info* t_return = new struct type_info;
    if (t1->type == t2->type && t1->type == TYPE::SIMPLE) {

        t_return->type = TYPE::SIMPLE;
        t_return->eleType = get_type(t1->eleType, t2->eleType);

        if (is_primitive(t1->eleType) && is_vid(t2->eleType) || is_primitive(t2->eleType) && is_vid(t1->eleType)) 
        {
            yyerror("Cannot perform binary operation on primitive and video");
            exit(1);
        }

        if ((op != OPERATOR::ADD) && (is_vid(t1->eleType) || is_vid(t2->eleType))) 
        {
            yyerror("Only + is defined for videos");
            exit(1);
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
            for (auto i = 0; i < 2; i++) 
            {
                if (t1->dim_list->at(i) != t2->dim_list->at(i)) 
                {
                    yyerror("Cannot perform binary operation on images of different dimensions (hxw)");
                    exit(1);
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
            for (auto i = 0; i < 2; i++) 
            {
                if (t1->dim_list->at(i) != t2->dim_list->at(i)) 
                {
                    yyerror("Cannot perform binary operation on images and videos of different dimensions (hxw)");
                    exit(1);
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
            if(!(t1->dim_list->at(0) == t2->dim_list->at(0) &&
                 t1->dim_list->at(1) == t2->dim_list->at(1) &&
                 t1->dim_list->at(3) == t2->dim_list->at(3)
                )) 
            {
                yyerror("Videos are not compatible. They must have same h, w and frame rate");
                exit(1);
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
                exit(1);
            }
        } 
        else 
        {
            yyerror("Cannot perform binary operation on arrays of different dimensions");
            exit(1);
        }
    } 
    else if (t2->type == TYPE::ARRAY && t1->type == TYPE::SIMPLE || t2->type == TYPE::SIMPLE && t1->type == TYPE::ARRAY) 
    { // arr with simple, simple has to be only num/real/bool
        if (!is_primitive(t1->eleType) || !is_primitive(t2->eleType)) {
            yyerror("Cannot perform binary operation on arrays and non-primitive");
            exit(1);
        }

        t_return->type = TYPE::ARRAY;
        t_return->eleType = get_type(t1->eleType, t2->eleType);
        std::vector<int>* temp_dim_list;
        t1->type == TYPE::ARRAY ? (temp_dim_list = t1->dim_list) : (temp_dim_list = t2->dim_list);
        t_return->dim_list = new std::vector<int>(temp_dim_list->size() - 1);
    }

    return t_return;
}