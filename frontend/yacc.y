%{
    #include <bits/stdc++.h>
    #include "lex.yy.c"
    #include "headers/sym_tab.hpp"
    #include "headers/semantic.hpp"
    #include "headers/utils.hpp"
    #include "headers/codegen.hpp"

    int yylex (void);
    FILE* ftoken, *fparser, *foutput;
    int lineno = 1; 
    void yyerror (const char*);
    
    symbol_table_function* SymbolTableFunction = new symbol_table_function();
    symbol_table_variable* SymbolTableVariable = new symbol_table_variable();
    int current_scope = 0, error_counter = 0;
    bool return_flag = false;
    // std::vector<std::string> temp_str = std::vector<std::string>();
    // std::vector<std::string> temp_str_2 = std::vector<std::string>();
%}

%code requires {
    #include "headers/semantic.hpp"
    #include "headers/sym_tab.hpp"
    #include "headers/utils.hpp"
    #include "headers/codegen.hpp"
}

%union {
    int ival;
    float fval;
    bool bval;
    struct type_info* tval;
    std::string* sval;
    std::vector<int> *dim_list;
    std::vector<std::string> *id_vec;

    std::vector<std::pair<std::string, struct type_info*>> *par_vec;
    std::vector<struct type_info*> *arg_vec;

    struct {
    int val;
    std::string *str;
    std::vector<int> *vector_int;
    struct type_info* ti;
} lang_element;
    // struct 

    OPERATOR opval;
}

%type <lang_element> brak_pred brak_pred_list expr_pred_list expr_pred

%type <dim_list> array_element brak
%type <tval> datatype call_stmt RET_TYPE in_built_call_stmt
%type <par_vec> par_list par 
%type <arg_vec> arg_list arg
%type <id_vec> id_list
%type <ival> array_ele

%token <sval> ID /* Identifiers */

/* Literals */
%token <fval> REAL_CONST
%token <ival> NUM_CONST
%token <bval> BOOL_CONST

/* Operators */
%token <opval> BINARY_OP
%token <opval> UNARY_OP
%token <opval> INV_OP
%token <opval> GT
%token <opval> LT
%token <opval> LOG_OP
%token <opval> REL_OP
%token <opval> NEG_OP

%token <tval> IMG GRAY_IMG VID GRAY_VID NUM REAL VOID BOOL /* Datatypes */
%token IF ELSE_IF RETURN CONTINUE BREAK LOOP INK /* Control flow keywords */
%token ARROW DOT_OP /* Operators */
%token NEWLINE

%token PATH 
%start S /* Start symbol */

%left DOT_OP
%left UNARY_OP
%right INV_OP NEG_OP
%left BINARY_OP
%left LOG_OP 
%left REL_OP GT LT

%%

/*------------------------------------------------------------------------
 * Main Program Structure
 *------------------------------------------------------------------------*/

S : 
    optional_new_lines program
    {
        std::string last_func = SymbolTableFunction->get_current_func_name();
        ELETYPE last_ret_type = SymbolTableFunction->get_current_return_type();
        if (last_func != "main"){
            yyerror("last function is not main");
        }
        if (last_ret_type != ELETYPE::ELE_VOID){
            yyerror("last function does not return void");
        }
    }
    ;

program : function new_lines program
        | function optional_new_lines
        ;/* empty */ 


new_lines : new_lines NEWLINE
        | NEWLINE
        ;  

optional_new_lines : /* empty */
        | new_lines
        ; 

/*------------------------------------------------------------------------
 * Functions
 *------------------------------------------------------------------------*/

function : function_definition optional_new_lines func_body decrement_scope 
            {
                if (return_flag == false) {
                    yyerror("function does not return");
                } else {
                    return_flag = false;
                }
                if (error_counter == 0) fprintf(foutput, "}\n");
            }
        ;

function_definition 
    : INK ID  '(' increment_scope par_list ')' ARROW RET_TYPE 
        {
            std::string func_name = *$2;
            SymbolTableFunction->add_function_record(func_name, $8->eleType, $5); 
            assert(current_scope == 1);
            SymbolTableVariable->add_variable(*($5), current_scope);
            if (error_counter == 0) fprintf(foutput, "%s {\n", codegen_function_definition($8->eleType, func_name, $5).c_str());
        }
    | INK ID  '(' increment_scope ')' ARROW RET_TYPE 
        {
            std::string func_name = *$2;
            SymbolTableFunction->add_function_record(func_name, $7->eleType);
            if (error_counter == 0) fprintf(foutput, "%s {\n", codegen_function_definition($7->eleType, func_name, NULL).c_str());
        }
    ;

par_list : 
    par_list ',' par 
        {
            std::vector<std::pair<std::string, type_info*>> *p = $1;
            std::vector<std::pair<std::string, type_info*>> *q = $3;
            p->insert(p->end(), q->begin(), q->end());
            $$ = p;
        }
    | par
    ;

func_body : '{' increment_scope new_lines stmt_list '}' decrement_scope 
        ;

/* Only simple types are returned */
RET_TYPE : datatype 
        | VOID 
        ;

par : 
    datatype ID 
        {
        //     function_record* func = SymbolTableFunction->get_function(SymbolTableFunction->get_current_func_name());
        //     func->add_parameter($2,$1->type, $1->eleType);
            struct type_info* t = new struct type_info;
            t->type = $1->type;
            t->eleType = $1->eleType;
            
            if (is_vid(t->eleType) || is_img(t->eleType)) {
                t->dim_list = new std::vector<int>;
                for (int i = 0; i < $1->dim_list->size(); i++){
                    t->dim_list->push_back($1->dim_list->at(i));
                }
            }
            std::pair<std::string, type_info*> *p = new std::pair<std::string, type_info*>(*$2, t);
            $$ = new std::vector<std::pair<std::string, type_info*>>(1, *p);
        }
    | NUM brak ID
        { 
        //     function_record* func = SymbolTableFunction->get_function(SymbolTableFunction->get_current_func_name());
        //     func->add_parameter($3, TYPE::ARRAY, $1->eleType, $2);
            struct type_info* t = $1;
            t->type = TYPE::ARRAY;
            t->dim_list = $2;
            std::pair<std::string, type_info*> *p = new std::pair<std::string, type_info*>(*$3, t);
            $$ = new std::vector<std::pair<std::string, type_info*>>(1, *p);
        
        }
    | REAL brak ID   
        { 
        //     function_record* func = SymbolTableFunction->get_function(SymbolTableFunction->get_current_func_name());
        //     func->add_parameter($3, TYPE::ARRAY, $1->eleType, $2);
            struct type_info* t = $1;
            t->type = TYPE::ARRAY;
            t->dim_list = $2;
            std::pair<std::string, type_info*> *p = new std::pair<std::string, type_info*>(*$3, t);
            $$ = new std::vector<std::pair<std::string, type_info*>>(1, *p);
        }
    ;

datatype : IMG 
        | GRAY_IMG 
        | VID 
        | GRAY_VID  
        | NUM 
        | REAL
        | BOOL
        ;

brak : '[' ']'                  {$$ = new std::vector<int>(1, -1);}
        | '[' ',' ']'           {$$ = new std::vector<int>(2, -1);}
        | '[' ',' ',' ']'       {$$ = new std::vector<int>(3, -1);}
        ;

///////////////////////////////////////////////////////////////////
//                          Statements                         ////
///////////////////////////////////////////////////////////////////

stmt_list : stmt 
        | stmt_list stmt 
        ;

stmt : decl_stmt /* new_lines is included in expr_stmt */
        | conditional_stmt /* new_lines is included in conditional_stmt */
        | call_stmt new_lines
        | in_built_call_stmt new_lines
        | expr_stmt new_lines 
        | return_stmt /* new_lines is included in expr_stmt */
        | loop_block new_lines
        | '{'increment_scope new_lines stmt_list '}' decrement_scope new_lines /* This allows nested scopes */
        | '{' increment_scope new_lines '}' decrement_scope new_lines
        | '{' '}' new_lines
        | unary_stmt  /* new_lines is included in expr_stmt */
        ; 

/*------------------------------------------------------------------------*
 * Declaration Statements                                                 *
 *------------------------------------------------------------------------*/

decl_stmt : img_decl  new_lines
        | gray_img_decl new_lines
        | vid_decl new_lines
        | gray_vid_decl  new_lines
        | num_decl new_lines {if (error_counter == 0) fprintf(foutput, ";\n");}
        | bool_decl new_lines {if (error_counter == 0) fprintf(foutput, ";\n");}
        | real_decl new_lines  {if (error_counter == 0) fprintf(foutput, ";\n");}
        | num_array_decl new_lines {if (error_counter == 0) fprintf(foutput, ";\n");}
        | real_array_decl new_lines {if (error_counter == 0) fprintf(foutput, ";\n");}
        ;

img_decl : IMG ID LT NUM_CONST ',' NUM_CONST  GT                { declare_img(SymbolTableVariable, $1, *$2, $4, $6, 0, current_scope);}
        | IMG ID LT NUM_CONST ',' NUM_CONST ',' NUM_CONST GT    { declare_img(SymbolTableVariable, $1, *$2, $4, $6, $8, current_scope);}
        | IMG ID LT PATH GT                                     { declare_img(SymbolTableVariable, $1, *$2, current_scope);}
        | IMG ID '=' expr_pred                                  { struct type_info* t = assignment_compatible($1, $4.ti); declare_img(SymbolTableVariable, t, *$2, current_scope);}
        ; 

gray_img_decl : GRAY_IMG ID LT NUM_CONST ',' NUM_CONST GT           { declare_gray_img(SymbolTableVariable, $1, *$2, $4, $6, 0, current_scope);}
        | GRAY_IMG ID LT NUM_CONST ',' NUM_CONST ',' NUM_CONST GT   { declare_gray_img(SymbolTableVariable, $1, *$2, $4, $6, $8, current_scope);}
        | GRAY_IMG ID LT PATH GT                                    { declare_gray_img(SymbolTableVariable, $1, *$2, current_scope);}
        | GRAY_IMG ID '=' expr_pred                                 { struct type_info* t = assignment_compatible($1, $4.ti); declare_gray_img(SymbolTableVariable, t, *$2, current_scope);}
        ;

vid_decl : VID ID LT NUM_CONST ',' NUM_CONST GT                     { declare_vid(SymbolTableVariable, $1, *$2, $4, $6, 30, current_scope);}
        | VID ID LT NUM_CONST ',' NUM_CONST ',' NUM_CONST GT        { declare_vid(SymbolTableVariable, $1, *$2, $4, $6, $8, current_scope);}
        ; // we can have an assignment here

gray_vid_decl : GRAY_VID ID LT NUM_CONST ',' NUM_CONST GT           { declare_gray_vid(SymbolTableVariable, $1, *$2, $4, $6, 30, current_scope);}
        | GRAY_VID ID LT NUM_CONST ',' NUM_CONST ',' NUM_CONST GT   { declare_gray_vid(SymbolTableVariable, $1, *$2, $4, $6, $8, current_scope);}
        ; // we can have an assignment here

num_decl : 
    NUM id_list
        {
            struct type_info* t = $1;
            SymbolTableVariable->add_variable(*$2, t->type, t->eleType, current_scope);
            if (error_counter == 0) fprintf(foutput, "%s", codegen_decl_numeric(t, "", "", $2).c_str());
        }
    | NUM ID '=' expr_pred
        {
            struct type_info* t = $1, *t_res = new struct type_info;
            t_res = assignment_compatible(t, $4.ti);
            SymbolTableVariable->add_variable(*$2, t_res->type, t_res->eleType, current_scope);
            if (error_counter == 0) {
                // fprintf(foutput, "%s", codegen_decl_numeric(t, *$2, temp_str[0], NULL).c_str());
                fprintf(foutput, "%s", codegen_decl_numeric(t, *$2, *($4.str), NULL).c_str());
                // temp_str.clear();
            }
        }

        ;

bool_decl : 
    BOOL id_list
        {
            struct type_info* t = $1;
            SymbolTableVariable->add_variable(*$2, t->type, t->eleType, current_scope);
            if (error_counter == 0) fprintf(foutput, "%s", codegen_decl_numeric(t, "", "", $2).c_str());
        }
    | BOOL ID '=' expr_pred
        {
            struct type_info* t = $1, *t_res = new struct type_info;
            t_res = assignment_compatible(t, $4.ti);
            SymbolTableVariable->add_variable(*$2, t_res->type, t_res->eleType, current_scope);
            if (error_counter == 0) {
                // fprintf(foutput, "%s", codegen_decl_numeric(t, *$2, temp_str[0], NULL).c_str());
                fprintf(foutput, "%s", codegen_decl_numeric(t, *$2, *($4.str), NULL).c_str());
                // temp_str.clear();
            }
        }
    ;

real_decl : 
    REAL id_list
        {
            std::vector<std::string> *p = $2;
            struct type_info* t = $1;
            SymbolTableVariable->add_variable(*$2, t->type, t->eleType, current_scope);
            if (error_counter == 0) fprintf(foutput, "%s", codegen_decl_numeric(t, "", "", $2).c_str());
        }
        
    | REAL ID '=' expr_pred
        {
            struct type_info* t = $1, *t_res = new struct type_info;
            t_res = assignment_compatible(t, $4.ti);
            SymbolTableVariable->add_variable(*$2, t_res->type, t_res->eleType, current_scope);
            if (error_counter == 0) {
                // fprintf(foutput, "%s", codegen_decl_numeric(t, *$2, temp_str[0], NULL).c_str());
                fprintf(foutput, "%s", codegen_decl_numeric(t, *$2, *($4.str), NULL).c_str());
                // temp_str.clear();
            }
        }
    ;

num_array_decl : 
    NUM array_element id_list
        {
            struct type_info* t = $1;
            t->type = TYPE::ARRAY;
            t->dim_list = new std::vector<int>;
            std::vector<int> temp_dim_list = *$2;
            for (int i = 0; i < temp_dim_list.size(); i++) {
                t->dim_list->push_back(temp_dim_list[i]);
            }

            SymbolTableVariable->add_variable(*$3, t->type, t->eleType, *$2, current_scope);
            if (error_counter == 0) fprintf(foutput, "%s", codegen_decl_numeric(t, "", "", $3).c_str());
        }
    | NUM array_element ID '=' ID 
        {
            data_record* dr = SymbolTableVariable->get_variable(*$5, current_scope);
            struct type_info* t1 = $1, *t_res = new struct type_info, *t2 = new struct type_info;
            t2->type = dr->get_type();
            t2->eleType = dr->get_ele_type();
            std::vector<int> temp_dim_list = dr->get_dim_list();

            // t2->dim_list = &temp_dim_list;
            t2->dim_list = new std::vector<int>;
            for (int i = 0; i < temp_dim_list.size(); i++){
                t2->dim_list->push_back(temp_dim_list[i]);
            }

            t1->type = TYPE::ARRAY;
            t1->dim_list = $2;
            
            t_res = assignment_compatible(t1, t2);
            SymbolTableVariable->add_variable(*$3, t_res->type, t_res->eleType, *(t_res->dim_list), current_scope);
            if (error_counter == 0) fprintf(foutput, "%s", codegen_decl_numeric(t_res, *$3, *$5, NULL).c_str());
        }
    | NUM array_element ID '=' brak_pred 
        {
            struct type_info *t_res = new struct type_info;
            t_res->type = TYPE::ARRAY;
            t_res->eleType = ELETYPE::ELE_NUM;
            std::vector<int> *temp_dim_list = $2;
            t_res->dim_list = new std::vector<int>;
            for (int i = 0; i < temp_dim_list->size(); i++){
                t_res->dim_list->push_back(temp_dim_list->at(i));
            }
            array_compatibility(*(t_res->dim_list), *($5.vector_int));
            SymbolTableVariable->add_variable(*$3, t_res->type, t_res->eleType, *(t_res->dim_list), current_scope);
            if (error_counter == 0) fprintf(foutput, "%s", codegen_decl_numeric(t_res, *$3, *($5.str), NULL).c_str());
        }
    ;

real_array_decl : 
    REAL array_element id_list
        {
            struct type_info* t = $1;
            t->type = TYPE::ARRAY;
            t->dim_list = new std::vector<int>;
            std::vector<int> temp_dim_list = *$2;
            for (int i = 0; i < temp_dim_list.size(); i++) {
                t->dim_list->push_back(temp_dim_list[i]);
            }

            SymbolTableVariable->add_variable(*$3, t->type, t->eleType, *$2, current_scope);  
            if (error_counter == 0) fprintf(foutput, "%s", codegen_decl_numeric(t, "", "", $3).c_str());  
        }
    | REAL array_element ID '=' ID 
        {
            data_record* dr = SymbolTableVariable->get_variable(*$5, current_scope);
            struct type_info* t1 = $1, *t_res = new struct type_info, *t2 = new struct type_info;
            t2->type = dr->get_type();
            t2->eleType = dr->get_ele_type();
            std::vector<int> temp_dim_list = dr->get_dim_list();
            // t2->dim_list = &temp_dim_list;
            t2->dim_list = new std::vector<int>;
            for (int i = 0; i < temp_dim_list.size(); i++){
                t2->dim_list->push_back(temp_dim_list[i]);
            }

            t1->type = TYPE::ARRAY;
            std::vector<int> *temp_dim_list2 = $2;
            t1->dim_list = temp_dim_list2;
            
            t_res = assignment_compatible(t1, t2);
            SymbolTableVariable->add_variable(*$3, t_res->type, t_res->eleType, *(t_res->dim_list), current_scope);
            if (error_counter == 0) fprintf(foutput, "%s", codegen_decl_numeric(t_res, *$3, *$5, NULL).c_str());
        }
    | REAL array_element ID '=' brak_pred
        {
            struct type_info *t_res = new struct type_info;
            t_res->type = TYPE::ARRAY;
            t_res->eleType = ELETYPE::ELE_REAL;
            std::vector<int> *temp_dim_list = $2;
            t_res->dim_list = new std::vector<int>;
            for (int i = 0; i < temp_dim_list->size(); i++){
                t_res->dim_list->push_back(temp_dim_list->at(i));
            }
            array_compatibility(*(t_res->dim_list), *($5.vector_int));
            SymbolTableVariable->add_variable(*$3, t_res->type, t_res->eleType, *(t_res->dim_list), current_scope);
            if (error_counter == 0) fprintf(foutput, "%s", codegen_decl_numeric(t_res, *$3, *($5.str), NULL).c_str());
        }  
    ;

array_element : '[' array_ele ']' { $$ = new std::vector<int>(1); $$->at(0) = $2;}
        |  '[' array_ele ',' array_ele ']' { $$ = new std::vector<int>(2); $$->at(0) = $2; $$->at(1) = $4;}
        |  '[' array_ele ',' array_ele ',' array_ele ']' { $$ = new std::vector<int>(3); $$->at(0) = $2; $$->at(1) = $4; $$->at(2) = $6;}
        ;

array_ele : ID  {$$ = -1;}
        | NUM_CONST {$$ = $1;}

 /* 
{{{1, 1}, {1, 1}, {1, 1}}, {{1, 1}, {1, 1}, {1, 1}}}
2 -> expr_pred_list
{2} -> brak_pred
new_brak_pred -> check last n-1 dimensions 2 matches!
{3, 2} -> brak_pred_list
brak_pred -> {3,2}
brak_pred_list -> {1,3,2}
new_brak_pred -> check last n-1 dimensions, 3,2 matches!
{2,3,2} 
*/

expr_pred_list : expr_pred_list ',' expr_pred 
                    { 
                        $$.val = $1.val + 1; 
                        // temp_str_2.push_back(temp_str[0]); temp_str.clear(); 
                        // temp_str_2 = join(&temp_str_2, ",");
                        $$.str = new std::string(*($1.str) + ", " + *($3.str));
                        // *($$.str) = *($1.str) + ", " + *($3.str);
                    }
               | expr_pred 
                    { 
                        $$.val = 1; 
                        // temp_str_2.push_back(temp_str[0]); 
                        // temp_str.clear();
                        $$.str = new std::string(*($1.str));
                        // *($$.str) = *($1.str);
                    }
               ;

brak_pred : '{' brak_pred_list '}' 
                { 
                    $$.vector_int = $2.vector_int; 
                    // temp_str[0] = "{" + temp_str[0] + "}";
                    $$.str = new std::string("{" + *($2.str) + "}");
                    // *($$.str) = "{" + *($2.str) + "}";
                }
          | '{' expr_pred_list '}'      
                {
                    std::vector<int> *p = new std::vector<int>;
                    p->push_back($2.val);
                    $$.vector_int = p;
                    $$.str = new std::string("{" + *($2.str) + "}");
                    // *($$.str) = "{" + *($2.str) + "}";
                }
          ;

brak_pred_list : brak_pred_list ',' brak_pred 
                {
                    std::vector<int> *p = $1.vector_int;
                    std::vector<int> *q = $3.vector_int;
                    
                    for (int i = 1; i<p->size(); i++){
                        if (p->at(i) != q->at(i-1)){
                            yyerror("incorrect bracket format");
                        }
                    }

                    p->at(0) += 1;
                    $$.vector_int = p;
                    $$.str = new std::string(*($1.str) + ", " + *($3.str));
                    // *($$.str) = *($1.str) + ", " + *($3.str);
                }
               | brak_pred
                {
                    std::vector<int> *p = new std::vector<int>;
                    p->push_back(1);
                    for (auto i : *($1.vector_int)){
                        p->push_back(i);
                    }
                    $$.vector_int = p;
                    $$.str = new std::string(*($1.str));
                    // *($$.str) = *($1.str);
                }
               ;

/* const_list : const_list ',' const
         | const
         ; */
        
/* const : NUM_CONST
    | REAL_CONST
    | BOOL_CONST
    ; 
*/

id_list : 
    id_list ',' ID
        {
            std::vector<std::string> *p = $1;
            std::string *q = $3;
            p->push_back(*q);
            $$ = p;
        }
    | ID 
        {
            std::vector<std::string> *p = new std::vector<std::string>(1, *$1);
            $$ = p;
        }
        ;

/*------------------------------------------------------------------------
* Conditional Statements
*------------------------------------------------------------------------*/

conditional_stmt : if_block optional_new_lines else_if_block_list optional_new_lines else { fprintf(fparser, "conditional");} new_lines
                | if_block new_lines {fprintf(fparser, "conditional");}
                | if_block optional_new_lines else_if_block_list new_lines {fprintf(fparser, "conditional");}
                | if_block optional_new_lines else {fprintf(fparser, "conditional");} new_lines
                ;

if_block : IF optional_new_lines '(' expr_pred ')' optional_new_lines ARROW optional_new_lines if_codegen func_body 
        {
            if (!cast_bool($4.ti)) {
                yyerror("experssion cannot to evaluated to a boolean");
            } else {
                // fprintf(foutput, "}\n");
            }
        }
        ;

if_codegen : /* empty */
        {
            // std::string exp = temp_str[0];
            // temp_str.clear();
            // std::string str = "if (" + exp + ") {\n";
            // fprintf(foutput, str.c_str());
        }
        ;

else_if_block_list : else_if_block_list optional_new_lines ELSE_IF '(' expr_pred ')' optional_new_lines ARROW optional_new_lines else_if_codegen func_body {fprintf(foutput, "}\n");}
        | ELSE_IF '(' expr_pred ')' optional_new_lines ARROW optional_new_lines else_if_codegen func_body
        {
            if (!cast_bool($3.ti)) {
                yyerror("experssion cannot to evaluated to a boolean");
            } else {
                // fprintf(foutput, "}\n");
            }
        } 
        ;

else_if_codegen : /* empty */
        {
            // std::string exp = temp_str[0];
            // temp_str.clear();
            // std::string str = "else if (" + exp + ") {\n";
            // fprintf(foutput, str.c_str());
        }
        ;
        
else : ARROW optional_new_lines else_codegen func_body
        {
            // fprintf(foutput, "}\n");
        }
        ;

else_codegen : /* empty */
        {
            // fprintf(foutput, "else {\n");
        }
        ;

/*------------------------------------------------------------------------
* Loop Statements
*------------------------------------------------------------------------*/

loop_block : LOOP optional_new_lines '(' increment_scope optional_loop_expr ')' optional_new_lines loop_body decrement_scope {fprintf(fparser, "loop");}
        | LOOP optional_new_lines '(' increment_scope optional_loop_decl ';' optional_loop_expr ';' optional_loop_expr ')' optional_new_lines loop_body decrement_scope {fprintf(fparser, "loop");}
        ;

optional_loop_expr : expr_pred
        | /* empty */
        ;

optional_loop_decl : expr_or_decl_stmt 
        | /* empty */
        ;

loop_body : '{' new_lines loop_stmt_list '}'
        ;

loop_stmt_list : loop_stmt
        | loop_stmt_list loop_stmt
        ;
        
loop_stmt : decl_stmt /* new_lines is included in decl_stmt */
        | loop_conditional_stmt /* new_lines is included in conditional_stmt */
        | call_stmt new_lines
        | in_built_call_stmt new_lines
        | expr_stmt new_lines
        | return_stmt /* new_lines is included in expr_stmt */
        | loop_block new_lines
        | '{' increment_scope new_lines loop_stmt_list '}' decrement_scope new_lines /* This allows nested scopes */
        | '{' increment_scope new_lines '}' decrement_scope new_lines
        | '{' '}' new_lines
        | unary_stmt  /* new_lines is included in expr_stmt */
        | BREAK new_lines
        | CONTINUE new_lines
        ;

loop_if_block : IF optional_new_lines '(' expr_pred ')' optional_new_lines ARROW optional_new_lines loop_body 
        ;

loop_else : ARROW optional_new_lines loop_body
        ;

loop_else_if_block_list : loop_else_if_block_list optional_new_lines ELSE_IF '(' expr_pred ')' optional_new_lines ARROW optional_new_lines loop_body
        | ELSE_IF '(' expr_pred ')' optional_new_lines ARROW optional_new_lines loop_body 
        ;

loop_conditional_stmt : loop_if_block optional_new_lines loop_else_if_block_list optional_new_lines loop_else { fprintf(fparser, "conditional");} new_lines
                | loop_if_block new_lines {fprintf(fparser, "conditional");}
                | loop_if_block optional_new_lines loop_else_if_block_list new_lines {fprintf(fparser, "conditional");}
                | loop_if_block optional_new_lines loop_else {fprintf(fparser, "conditional");} new_lines
                ;

loop_expr_stmt : ID '=' expr_pred 
        | ID array_element '=' expr_pred 
        ;

/*------------------------------------------------------------------------
* Expressions
*------------------------------------------------------------------------*/
        
expr_stmt : 
    ID '=' expr_pred 
    {
        data_record* dr = SymbolTableVariable->get_variable(*$1, current_scope);
        struct type_info* t1 = $3.ti, *t2 = new struct type_info;
        t2->type = dr->get_type();
        t2->eleType = dr->get_ele_type();
        std::vector<int> temp_dim_list = dr->get_dim_list();
        t2->dim_list = new std::vector<int>;
        for (int i = 0; i < temp_dim_list.size(); i++){
            t2->dim_list->push_back(temp_dim_list[i]);
        }
        struct type_info* t = assignment_compatible(t2, t1);
        if (t == NULL) {
            yyerror("assignment not compatible");
        }
    }
        
    | ID array_element '=' expr_pred 
    ;     

expr_pred : 
        ID 
            {
                data_record* dr = SymbolTableVariable->get_variable(*$1, current_scope);
                struct type_info* ti = new struct type_info;
                ti->type = dr->get_type(); 

                ti->eleType = dr->get_ele_type(); 
                std::vector<int> temp_dim_list = dr->get_dim_list();

                // Copy elements of temp_dim_list to ti->dim_list
                ti->dim_list = new std::vector<int>;
                for (int i = 0; i < temp_dim_list.size(); i++){
                    ti->dim_list->push_back(temp_dim_list[i]);
                }
                ti->name = *$1;
                // print size of dim_list
                $$.ti = ti; 
                // *($$.str) = *$1;
                $$.str = new std::string(*$1);
                // $$ = ti;
                // temp_str.push_back(*$1);
            }
        | NUM_CONST
            {
                struct type_info* ti = new struct type_info;
                ti->type = TYPE::SIMPLE;
                ti->eleType = ELETYPE::ELE_NUM;
                $$.ti = ti;
                // *($$.str) = std::to_string($1);
                $$.str = new std::string(std::to_string($1));
                // temp_str.push_back(std::to_string($1));
            }
        | REAL_CONST
            {
                struct type_info* ti = new struct type_info;
                ti->type = TYPE::SIMPLE;
                ti->eleType = ELETYPE::ELE_REAL;
                $$.ti = ti;
                // *($$.str) = std::to_string($1);
                $$.str = new std::string(std::to_string($1));
                // temp_str.push_back(std::to_string($1));
            }
        | BOOL_CONST
            {
                struct type_info* ti = new struct type_info;
                ti->type = TYPE::SIMPLE;
                ti->eleType = ELETYPE::ELE_BOOL;
                $$.ti = ti;
                // if ($1 == 1) 
                //     temp_str.push_back("true");
                // else 
                //     temp_str.push_back("false");
                // if ($1 == 1) 
                //     *($$.str) = "true";
                // else 
                //     *($$.str) = "false";       
                if ($1 == 1) 
                    $$.str = new std::string("true");
                else 
                    $$.str = new std::string("false");
            }
        | expr_pred REL_OP expr_pred  
            {
                struct type_info *t1 = $1.ti, *t2 = $3.ti, *t = new struct type_info;
                t = relational_compatible(t1, t2, $2);
                $$.ti = t;
                // temp_str = codegen_operator($2, &temp_str);
                // *($$.str) = codegen_operator($2, *($1.str), *($3.str));
                $$.str = new std::string(codegen_operator($2, *($1.str), *($3.str)));
            }      
        | expr_pred LT expr_pred
            {
                struct type_info *t1 = $1.ti, *t2 = $3.ti, *t = new struct type_info;
                t = relational_compatible(t1, t2, $2);
                $$.ti = t;
                // temp_str = codegen_operator($2, &temp_str);
                $$.str = new std::string(codegen_operator($2, *($1.str), *($3.str)));
                // *($$.str) = codegen_operator($2, *($1.str), *($3.str));
            }
        | expr_pred GT expr_pred
            {
                struct type_info *t1 = $1.ti, *t2 = $3.ti, *t = new struct type_info;
                t = relational_compatible(t1, t2, $2);
                $$.ti = t;
                // temp_str = codegen_operator($2, &temp_str);
                // *($$.str) = codegen_operator($2, *($1.str), *($3.str));
                $$.str = new std::string(codegen_operator($2, *($1.str), *($3.str)));
            }
        | expr_pred LOG_OP expr_pred
            {
                /*TEST*/
                struct type_info *t1 = $1.ti, *t2 = $3.ti, *t = new struct type_info;
                t = relational_compatible(t1, t2, $2);
                $$.ti = t;
                // temp_str = codegen_operator($2, &temp_str);
                // *($$.str) = codegen_operator($2, *($1.str), *($3.str));
                $$.str = new std::string(codegen_operator($2, *($1.str), *($3.str)));
            }
        | '(' expr_pred ')'                 
            {
                $$.ti = $2.ti; 
                // temp_str[0] = "(" + temp_str[0] + ")";
                // *($$.str) = "(" + *($2.str) + ")";
                $$.str = new std::string("(" + *($2.str) + ")");
            } 
        | NEG_OP expr_pred                  
            {
                $$.ti = $2.ti; 
                // temp_str = codegen_operator($1, &temp_str);
                $$.str = new std::string(codegen_operator($1, *($2.str), ""));
                // *($$.str) = codegen_operator($1, *($2.str), "");
            } 
        | call_stmt                         
            {
                $$.ti = $1;
            } /*temp*/
        | in_built_call_stmt                
            {
                $$.ti = new struct type_info;
            } /*temp*/
        | ID array_element                  
            {
                $$.ti = new struct type_info;
            } /*temp*/
        | expr_pred BINARY_OP expr_pred
            {
                struct type_info *t1 = $1.ti, *t2 = $3.ti, *t = new struct type_info;
                // print address of t1's dim_list
                t = binary_compatible(t1, t2, $2);
                $$.ti = t;
                // temp_str = codegen_operator($2, &temp_str);
                $$.str = new std::string(codegen_operator($2, *($1.str), *($3.str)));
                // *($$.str) = codegen_operator($2, *($1.str), *($3.str));
            }
        | expr_pred UNARY_OP 
            {
                struct type_info *t1 = $1.ti, *t = new struct type_info;
                t = unary_compatible(t1, $2);
                $$.ti = t;
                // temp_str = codegen_operator($2, &temp_str);
                $$.str = new std::string(codegen_operator($2, *($1.str), ""));
                // *($$.str) = codegen_operator($2, *($1.str), "");
            }           
        | INV_OP expr_pred
            {
                struct type_info *t1 = $2.ti, *t = new struct type_info;
                t = unary_compatible(t1, $1);
                $$.ti = t;
                // temp_str = codegen_operator($1, &temp_str);
                $$.str = new std::string(codegen_operator($1, *($2.str), ""));
                // *($$.str) = codegen_operator($1, *($2.str), "");
            }
        ;

expr_or_decl_stmt : num_decl
        | real_decl
        | bool_decl
        | img_decl  
        | gray_img_decl 
        | vid_decl 
        | gray_vid_decl  
        | num_array_decl 
        | real_array_decl 
        | loop_expr_stmt
        ;

/*------------------------------------------------------------------------
* Call Statements
*------------------------------------------------------------------------*/

call_stmt : ID '(' arg_list ')' // Archit
                {
                    struct type_info* t = new struct type_info;
                    t = check_func_call(SymbolTableFunction, *$1, $3);
                    $$ = t;
                }
        | ID '(' ')' // Archit
                {
                    struct type_info* t = new struct type_info;
                    $$ = check_func_call(SymbolTableFunction, *$1);
                    $$ = t;
                }
        ;

in_built_call_stmt : 
    ID DOT_OP ID '(' arg_list ')'
        {
            data_record* dr = SymbolTableVariable->get_variable(*$1, current_scope);
            struct type_info* t1 = new struct type_info;
            t1->type = dr->get_type();
            t1->eleType = dr->get_ele_type();
            std::vector<int> temp_dim_list = dr->get_dim_list();
            t1->dim_list = new std::vector<int>;    
            for (int i = 0; i < temp_dim_list.size(); i++){
                t1->dim_list->push_back(temp_dim_list[i]);
            }
            // t1->dim_list = &temp_dim_list;
            $$ = check_inbuilt_func_call(t1, *$3, $5);
        }
    | ID DOT_OP ID '(' ')'
        {
            data_record* dr = SymbolTableVariable->get_variable(*$1, current_scope);
            struct type_info* t1 = new struct type_info;
            t1->type = dr->get_type();
            t1->eleType = dr->get_ele_type();
            std::vector<int> temp_dim_list = dr->get_dim_list();
            // t1->dim_list = &temp_dim_list;
            t1->dim_list = new std::vector<int>;
            for (int i = 0; i < temp_dim_list.size(); i++){
                t1->dim_list->push_back(temp_dim_list[i]);
            }
            std::vector<struct type_info*> *temp  = new std::vector<struct type_info*>;
            $$ = check_inbuilt_func_call(t1,*$3, temp);
        }
    | in_built_call_stmt DOT_OP ID '(' arg_list ')'     
        { 
            $$ = check_inbuilt_func_call($1, *$3, $5);
        }
    | in_built_call_stmt DOT_OP ID '(' ')'              
        { 
            std::vector<struct type_info*> *temp  = new std::vector<struct type_info*>;
            $$ = check_inbuilt_func_call($1, *$3, temp);
        }
        ;

// Archit
arg_list : arg_list ',' arg {
                std::vector<struct type_info*> *p = $1;

                std::vector<struct type_info*> *q = $3;
                p->push_back(q->at(0));
                $$ = p;
            }
        | arg 
        ;

// Archit  
arg : expr_pred  
        { 
            std::vector<struct type_info*> *p = new std::vector<struct type_info*>(1, $1.ti); 
            $$ = p;
        }
    | PATH 
        { 
            struct type_info* t = new struct type_info;
            t->type = TYPE::SIMPLE;
            t->eleType = ELETYPE::ELE_STR;
            std::vector<struct type_info*> *p = new std::vector<struct type_info*>(1, t); 
            $$ = p;
        }
    ;

/* -----------------------------------------------------------------------
 * Other Statements
 * ----------------------------------------------------------------------- */

unary_stmt : ID UNARY_OP new_lines
    {
        data_record* dr = SymbolTableVariable->get_variable(*$1, current_scope);
        struct type_info* t1 = new struct type_info;
        t1->type = dr->get_type();
        t1->eleType = dr->get_ele_type();
        std::vector<int> temp_dim_list = dr->get_dim_list();
        t1->dim_list = new std::vector<int>;
        for (int i = 0; i < temp_dim_list.size(); i++){
            t1->dim_list->push_back(temp_dim_list[i]);
        }
        struct type_info* t = unary_compatible(t1, $2, flag_type::stmt);
        if (t == NULL) {
            yyerror("unary operation not compatible");
        }
    }
    ;

return_stmt : RETURN expr_pred new_lines
        {   
            return_flag = true;
            ELETYPE last_ret_type = SymbolTableFunction->get_current_return_type();
            struct type_info* t = $2.ti;
            if (last_ret_type != t->eleType) {
                yyerror("return type must be same as function definition");
            }
        }
    | RETURN VOID new_lines 
        {
            return_flag = true;
            ELETYPE last_ret_type = SymbolTableFunction->get_current_return_type();
            if (last_ret_type != ELETYPE::ELE_VOID) {
                yyerror("return type must be same as function definition");
            }
        }
    ;

/* -----------------------------------------------------------------------
 * Useful Actions
 * ----------------------------------------------------------------------- */

increment_scope : /* empty */ {current_scope++;}
    ;

decrement_scope : /* empty */ 
    {
        SymbolTableVariable->delete_variable(current_scope);
        current_scope--;
    }
    ;

%%

int yywrap(){ 
    return 1;
}
void yyerror(const char* s){ 
    /* printf("\033[1;31mError at line %d:\033[0m %s\n", yylineno, s); */
    printf("\033[1;31mError at line %d:\033[0m %s\n", lineno, s);
    error_counter++;
    if (error_counter > 10){
        printf("\033[1;31mToo many errors. Exiting.\033[0m\n");
        exit(1);
    }
    fprintf(fparser, " : invalid statement");
}

int main(int argc, char* argv[]){
    ftoken = fopen("token.txt", "w");
    fparser = fopen("parsed.txt", "w");
    foutput = fopen("../output.cpp", "w");
    fprintf(foutput, "%s", codegen_headers().c_str());
    yyparse();
    fclose(ftoken);
    fclose(fparser);
    return 0;
}