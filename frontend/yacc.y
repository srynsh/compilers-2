%{
    #include <bits/stdc++.h>
    #include "lex.yy.c"
    #include "headers/sym_tab.hpp"
    #include "headers/semantic.hpp"
    #include "headers/utils.hpp"

    int yylex (void);
    FILE* ftoken, *fparser;
    int lineno = 1; 
    void yyerror (const char*);
    
    symbol_table_function* SymbolTableFunction = new symbol_table_function();
    symbol_table_variable* SymbolTableVariable = new symbol_table_variable();
    int current_scope = 0;
%}

%code requires {
    #include "headers/semantic.hpp"
    #include "headers/sym_tab.hpp"
    #include "headers/utils.hpp"
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

    OPERATOR opval;
}

%type <dim_list> brak array_element
%type <tval> datatype expr_pred expr_pred_list brak_pred brak_pred_list
%type <tval> RET_TYPE in_built_call_stmt
%type <par_vec> par_list par 
%type <arg_vec> arg_list arg
%type <id_vec> id_list

%token <sval> ID /* Identifiers */

/* Literals */
%token <fval> REAL_CONST
%token <ival> NUM_CONST
%token <bval> BOOL_CONST

/* Operators */
%token <opval> BINARY_OP
%token <opval> UNARY_OP
%token <opval> INV_OP

%token <tval> IMG GRAY_IMG VID GRAY_VID NUM REAL VOID BOOL /* Datatypes */
%token IF ELSE_IF RETURN CONTINUE BREAK LOOP INK /* Control flow keywords */
%token ARROW DOT_OP LOG_OP REL_OP GT LT NEG_OP /* Operators */
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
            exit(1);
        }
        if (last_ret_type != ELETYPE::ELE_VOID){
            yyerror("last function does not return void");
            exit(1);
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
        ;

function_definition 
    : INK ID  '(' increment_scope par_list ')' ARROW RET_TYPE 
        {
            std::string func_name = *$2;
            SymbolTableFunction->add_function_record(func_name, $8->eleType, $5); 
            assert(current_scope == 1);
            SymbolTableVariable->add_variable(*($5), current_scope);
        }
    | INK ID  '(' increment_scope ')' ARROW RET_TYPE 
        {
            std::string func_name = *$2;
            SymbolTableFunction->add_function_record(func_name, $7->eleType);
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
            std::pair<std::string, type_info*> *p = new std::pair<std::string, type_info*>(*$2, $1);
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
        | expr_stmt /* new_lines is included in expr_stmt */
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
        | num_decl new_lines
        | bool_decl new_lines
        | real_decl new_lines 
        | num_array_decl new_lines
        | real_array_decl new_lines
        ;

img_decl : IMG ID LT NUM_CONST ',' NUM_CONST  GT                { declare_img(SymbolTableVariable, $1, *$2, $4, $6, 0, current_scope);}
        | IMG ID LT NUM_CONST ',' NUM_CONST ',' NUM_CONST GT    { declare_img(SymbolTableVariable, $1, *$2, $4, $6, $8, current_scope);}
        | IMG ID LT PATH GT                                     { declare_img(SymbolTableVariable, $1, *$2, current_scope);}
        | IMG ID '=' expr_pred                                  { struct type_info* t = assignment_compatible($1, $4); declare_img(SymbolTableVariable, t, *$2, current_scope);}
        ; 

gray_img_decl : GRAY_IMG ID LT NUM_CONST ',' NUM_CONST GT           { declare_gray_img(SymbolTableVariable, $1, *$2, $4, $6, 0, current_scope);}
        | GRAY_IMG ID LT NUM_CONST ',' NUM_CONST ',' NUM_CONST GT   { declare_gray_img(SymbolTableVariable, $1, *$2, $4, $6, $8, current_scope);}
        | GRAY_IMG ID LT PATH GT                                    { declare_gray_img(SymbolTableVariable, $1, *$2, -1, -1, 0, current_scope);}
        | GRAY_IMG ID '=' expr_pred 
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
        }
    | NUM ID '=' expr_pred
        {
            struct type_info* t = $1, *t_res = new type_info;
            t_res = assignment_compatible(t, $4);
            SymbolTableVariable->add_variable(*$2, t_res->type, t_res->eleType, current_scope);
        }
        ;

bool_decl : 
    BOOL id_list
        {
            struct type_info* t = $1;
            SymbolTableVariable->add_variable(*$2, t->type, t->eleType, current_scope);
        }
    | BOOL ID '=' expr_pred
        {
            struct type_info* t = $1, *t_res = new type_info;
            t_res = assignment_compatible(t, $4);
            SymbolTableVariable->add_variable(*$2, t_res->type, t_res->eleType, current_scope);
        }
    ;

real_decl : 
    REAL id_list
        {
            std::vector<std::string> *p = $2;
            struct type_info* t = $1;
            SymbolTableVariable->add_variable(*$2, t->type, t->eleType, current_scope);
        }
        
    | REAL ID '=' expr_pred
        {
            struct type_info* t = $1, *t_res = new type_info;
            t_res = assignment_compatible(t, $4);
            SymbolTableVariable->add_variable(*$2, t_res->type, t_res->eleType, current_scope);
        }
    ;

num_array_decl : 
    NUM array_element id_list
    {
        struct type_info* t = $1;
        t->type = TYPE::ARRAY;
        SymbolTableVariable->add_variable(*$3, t->type, t->eleType, *$2, current_scope);
    }
    | NUM array_element ID '=' ID 
        {
            data_record* dr = SymbolTableVariable->get_variable(*$5, current_scope);
            struct type_info* t1 = $1, *t_res = new type_info, *t2 = new type_info;
            t2->type = dr->get_type();
            t2->eleType = dr->get_ele_type();
            std::vector<int> temp_dim_list = dr->get_dim_list();
            t2->dim_list = &temp_dim_list;

            t1->type = TYPE::ARRAY;
            t1->dim_list = $2;
            
            t_res = assignment_compatible(t1, t2);
            SymbolTableVariable->add_variable(*$3, t_res->type, t_res->eleType, *(t_res->dim_list), current_scope);
        }
    | NUM array_element ID '=' brak_pred 
    ;

real_array_decl : 
    REAL array_element id_list
        {
            struct type_info* t = $1;
            t->type = TYPE::ARRAY;
            SymbolTableVariable->add_variable(*$3, t->type, t->eleType, *$2, current_scope);    
        }
    | REAL array_element ID '=' ID 
        {
            data_record* dr = SymbolTableVariable->get_variable(*$5, current_scope);
            struct type_info* t1 = $1, *t_res = new type_info, *t2 = new type_info;
            t2->type = dr->get_type();
            t2->eleType = dr->get_ele_type();
            std::vector<int> temp_dim_list = dr->get_dim_list();
            t2->dim_list = &temp_dim_list;

            t1->type = TYPE::ARRAY;
            std::vector<int> *temp_dim_list2 = $2;
            t1->dim_list = temp_dim_list2;
            
            t_res = assignment_compatible(t1, t2);
            SymbolTableVariable->add_variable(*$3, t_res->type, t_res->eleType, *(t_res->dim_list), current_scope);
        }
    | REAL array_element ID '=' brak_pred  
    ;

array_element : '[' expr_pred ']' { $$ = new std::vector<int>(1, -1);}
        |  '[' expr_pred ',' expr_pred ']' { $$ = new std::vector<int>(2, -1);}
        |  '[' expr_pred ',' expr_pred ',' expr_pred ']' { $$ = new std::vector<int>(3, -1);}
        ;

brak_pred : '{' brak_pred_list '}'      {$$ = $2;}
          /* | '{' const_list '}' */
          | '{' expr_pred_list '}'      {$$ = $2;}
          ;

brak_pred_list : brak_pred_list ',' brak_pred
               | brak_pred
               ;

/* const_list : const_list ',' const
         | const
         ; */
        
/* const : NUM_CONST
    | REAL_CONST
    | BOOL_CONST
    ; */

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

expr_pred_list : expr_pred_list ',' expr_pred
               | expr_pred
               ;

/*------------------------------------------------------------------------
* Conditional Statements
*------------------------------------------------------------------------*/

conditional_stmt : if_block optional_new_lines else_if_block_list optional_new_lines else { fprintf(fparser, "conditional");} new_lines
                | if_block new_lines {fprintf(fparser, "conditional");}
                | if_block optional_new_lines else_if_block_list new_lines {fprintf(fparser, "conditional");}
                | if_block optional_new_lines else {fprintf(fparser, "conditional");} new_lines
                ;

if_block : IF optional_new_lines '(' expr_pred ')' optional_new_lines ARROW optional_new_lines func_body 
        ;

else_if_block_list : else_if_block_list optional_new_lines ELSE_IF '(' expr_pred ')' optional_new_lines ARROW optional_new_lines func_body
        | ELSE_IF '(' expr_pred ')' optional_new_lines ARROW optional_new_lines func_body 
        ;
        
else : ARROW optional_new_lines func_body
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
        | expr_stmt /* new_lines is included in expr_stmt */
        | return_stmt /* new_lines is included in expr_stmt */
        | loop_block new_lines
        | '{' increment_scope new_lines loop_stmt_list '}' decrement_scope new_lines /* This allows nested scopes */
        | '{' increment_scope new_lines '}' decrement_scope new_lines
        | '{' '}' new_lines
        | unary_stmt  /* new_lines is included in expr_stmt */
        | BREAK new_lines
        | CONTINUE new_lines
        ;

loop_if_block : IF optional_new_lines '(' expr_pred')' optional_new_lines ARROW optional_new_lines loop_body 
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
        
expr_stmt : ID '=' expr_pred new_lines
        | ID array_element '=' expr_pred new_lines
        ;     

expr_pred : 
        ID 
            {
                data_record* dr = SymbolTableVariable->get_variable(*$1, current_scope);
                struct type_info* ti = new type_info;
                ti->type = dr->get_type(); 
                ti->eleType = dr->get_ele_type(); 
                std::vector<int> temp_dim_list = dr->get_dim_list();
                ti->dim_list = &temp_dim_list;
                $$ = ti;
            }
        | NUM_CONST
            {
                struct type_info* ti = new type_info;
                ti->type = TYPE::SIMPLE;
                ti->eleType = ELETYPE::ELE_NUM;
                $$ = ti;
            }
        | REAL_CONST
            {
                struct type_info* ti = new type_info;
                ti->type = TYPE::SIMPLE;
                ti->eleType = ELETYPE::ELE_REAL;
                $$ = ti;
            }
        | BOOL_CONST
            {
                struct type_info* ti = new type_info;
                ti->type = TYPE::SIMPLE;
                ti->eleType = ELETYPE::ELE_BOOL;
                $$ = ti;
            }
        | expr_pred REL_OP expr_pred
        | expr_pred LT expr_pred
        | expr_pred GT expr_pred
        | expr_pred LOG_OP expr_pred
        | '(' expr_pred ')'                 {$$ = $2;} 
        | NEG_OP expr_pred                  {$$ = $2;} /*temp*/
        | call_stmt                         {$$ = new type_info;} /*temp*/
        | in_built_call_stmt                {$$ = new type_info;} /*temp*/
        | ID array_element                  {$$ = new type_info;} /*temp*/
        | expr_pred BINARY_OP expr_pred
            {
                struct type_info *t1 = $1, *t2 = $3, *t = new type_info;
                t = binary_compatible(t1, t2, $2);
                $$ = t;
            }
        | expr_pred UNARY_OP 
            {
                struct type_info *t1 = $1, *t = new type_info;
                t = unary_compatible(t1, $2);
                $$ = t;
            }           
        | INV_OP expr_pred
            {
                struct type_info *t1 = $2, *t = new type_info;
                t = unary_compatible(t1, $1);
                $$ = t;
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
                    check_func_call(SymbolTableFunction, *$1, $3);
                }
        | ID '(' ')' // Archit
                {
                    check_func_call(SymbolTableFunction, *$1);
                }
        ;

in_built_call_stmt : 
    ID DOT_OP ID '(' arg_list ')'
        {
            data_record* dr = SymbolTableVariable->get_variable(*$1, current_scope);
            struct type_info* t1 = new type_info;
            t1->type = dr->get_type();
            t1->eleType = dr->get_ele_type();
            std::vector<int> temp_dim_list = dr->get_dim_list();
            t1->dim_list = &temp_dim_list;
            $$ = check_inbuilt_func_call(t1, *$3, $5);
        }
    | ID DOT_OP ID '(' ')'
        {
            data_record* dr = SymbolTableVariable->get_variable(*$1, current_scope);
            struct type_info* t1 = new type_info;
            t1->type = dr->get_type();
            t1->eleType = dr->get_ele_type();
            std::vector<int> temp_dim_list = dr->get_dim_list();
            t1->dim_list = &temp_dim_list;
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
arg : expr_pred  { std::vector<struct type_info*> *p = new std::vector<struct type_info*>(1, $1); $$ = p;}
    | PATH 
        { 
            struct type_info* t = new type_info;
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
    ;

return_stmt : RETURN expr_pred new_lines
        {
            ELETYPE last_ret_type = SymbolTableFunction->get_current_return_type();
            struct type_info* t = $2;
            if (last_ret_type != t->eleType) {
                yyerror("return type must be same as function definition");
                exit(1);
            }
        }
    | RETURN VOID new_lines 
        {
            ELETYPE last_ret_type = SymbolTableFunction->get_current_return_type();
            if (last_ret_type != ELETYPE::ELE_VOID) {
                yyerror("return type must be same as function definition");
                exit(1);
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
    printf("Error at line %d %s\n", lineno, s);
    fprintf(fparser, " : invalid statement");
    exit(1);
}

int main(int argc, char* argv[]){
    ftoken = fopen("token.txt", "w");
    fparser = fopen("parsed.txt", "w");
    yyparse();
    fclose(ftoken);
    fclose(fparser);
    return 0;
}