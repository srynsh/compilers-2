%{
    #include <bits/stdc++.h>
    #include "lex.yy.c"
    #include "headers/sym_tab.hpp"
    #include "headers/semantic.hpp"

    int yylex (void);
    FILE* ftoken, *fparser;
    int lineno = 1; 
    void yyerror (const char*);
    
    symbol_table_function* SymbolTableFunction = new symbol_table_function();
    symbol_table_variable* SymbolTableVariable = new symbol_table_variable();
    int current_scope = 0;

%}

%union {
    int ival;
    float fval;
    bool bval;
    struct type_info* tval;
    std::string* sval;
    std::vector<int> *dim_list;
    std::vector<std::string> *id_vec;

    std::vector<std::pair<std::string, type_info*>> *par_vec;

}

%type <dim_list> brak
%type <tval> datatype
%type <tval> RET_TYPE
%type <par_vec> par_list par
%type <id_vec> id_list

%token <sval> ID /* Identifiers */

/* Literals */
%token <fval> REAL_CONST
%token <ival> NUM_CONST
%token <bval> BOOL_CONST

%token <tval> IMG GRAY_IMG VID GRAY_VID NUM REAL VOID BOOL /* Datatypes */
%token IF ELSE_IF RETURN CONTINUE BREAK LOOP INK /* Control flow keywords */
%token ARROW DOT_OP LOG_OP REL_OP GT LT NEG_OP /* Operators */
%token NEWLINE

%token PATH BINARY_OP UNARY_OP INV_OP /* Operators */ 
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

function : function_definition optional_new_lines func_body 
        ;

function_definition 
    : INK ID  '(' par_list ')' ARROW RET_TYPE 
        {
            std::string func_name = *$2;
            SymbolTableFunction->add_function_record(func_name, $7->eleType);
            function_record* func = SymbolTableFunction->get_function(func_name);
            // for (auto p : *$4){
            //     if (p.second->type == TYPE::ARRAY)
            //         func->add_parameter(&(p.first), p.second->type, p.second->eleType, p.second->dim_list);
            //     else
            //         func->add_parameter(&(p.first), p.second->type, p.second->eleType);
            // }
        }
    | INK ID  '(' ')' ARROW RET_TYPE 
        {
            std::string func_name = *$2;
            SymbolTableFunction->add_function_record(func_name, $6->eleType);
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

func_body : '{' set_scope_function new_lines stmt_list '}' set_scope_outside_function
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

/*------------------------------------------------------------------------
 * Declaration Statements
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

img_decl : IMG ID LT NUM_CONST ',' NUM_CONST  GT 
        | IMG ID LT NUM_CONST ',' NUM_CONST ',' NUM_CONST GT 
        | IMG ID LT PATH GT 
        | IMG ID '=' expr_pred 
        ; 

gray_img_decl : GRAY_IMG ID LT NUM_CONST ',' NUM_CONST GT 
        | GRAY_IMG ID LT NUM_CONST ',' NUM_CONST ',' NUM_CONST GT 
        | GRAY_IMG ID LT PATH GT 
        | GRAY_IMG ID '=' expr_pred 
        ;

vid_decl : VID ID LT NUM_CONST ',' NUM_CONST GT 
        | VID ID LT NUM_CONST ',' NUM_CONST ',' NUM_CONST GT 
        ;

gray_vid_decl : GRAY_VID ID LT NUM_CONST ',' NUM_CONST GT 
        | GRAY_VID ID LT NUM_CONST ',' NUM_CONST ',' NUM_CONST GT 
        ;

num_decl : NUM id_list
        {
            std::vector<std::string> *p = $2;
            struct type_info* t = $1;
            for (auto q : *p){
                SymbolTableVariable->add_variable(q, t->type, t->eleType, current_scope);
            }
        }
        | NUM ID '=' expr_pred
        ;

bool_decl : BOOL id_list
        {
            std::vector<std::string> *p = $2;
            for (auto q : *p){
                SymbolTableVariable->add_variable(q, TYPE::SIMPLE, ELETYPE::ELE_BOOL, current_scope);
            }
        }
        | BOOL ID '=' expr_pred
        ;

real_decl : REAL id_list
        {
            std::vector<std::string> *p = $2;
            for (auto q : *p){
                SymbolTableVariable->add_variable(q, TYPE::SIMPLE, ELETYPE::ELE_REAL, current_scope);
            }
        }
        | REAL ID '=' expr_pred
        ;

num_array_decl : 
    NUM array_element id_list
    {
        std::vector<std::string> *p = $3;
        std::vector<int> *q = $2;
        for (auto r : *p){
            SymbolTableVariable->add_variable(r, TYPE::ARRAY, ELETYPE::ELE_NUM, current_scope, q);
        }
    }
    | NUM array_element ID '=' ID    
    | NUM array_element ID '=' brak_pred 
                ;

real_array_decl : REAL array_element id_list
                | REAL array_element ID '=' ID 
                | REAL array_element ID '=' brak_pred  
                ;

array_element : '[' expr_pred ']'
        |  '[' expr_pred ',' expr_pred ']'
        |  '[' expr_pred ',' expr_pred ',' expr_pred ']'
        ;

brak_pred : '{' brak_pred_list '}'
          | '{' const_list '}'
          ;

brak_pred_list : brak_pred_list ',' brak_pred
               | brak_pred
               ;

const_list : const_list ',' const
         | const
         ;
        
const : NUM_CONST
    | REAL_CONST
    | BOOL_CONST
    ;

id_list : id_list ',' ID
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

conditional_stmt : if_block optional_new_lines else_if_block_list optional_new_lines else { fprintf(fparser, "conditional\n");} new_lines
                | if_block new_lines {fprintf(fparser, "conditional\n");}
                | if_block optional_new_lines else_if_block_list new_lines {fprintf(fparser, "conditional\n");}
                | if_block optional_new_lines else {fprintf(fparser, "conditional\n");} new_lines
                ;

if_block : IF optional_new_lines '(' expr_pred')' optional_new_lines ARROW optional_new_lines func_body 
        ;

else_if_block_list : else_if_block_list optional_new_lines ELSE_IF '(' expr_pred ')' optional_new_lines ARROW optional_new_lines func_body
        | ELSE_IF '(' expr_pred ')' optional_new_lines ARROW optional_new_lines func_body 
        ;
        
else : ARROW optional_new_lines func_body
        ;

/*------------------------------------------------------------------------
* Loop Statements
*------------------------------------------------------------------------*/

loop_block : LOOP optional_new_lines '(' optional_loop_expr ')' optional_new_lines loop_body {fprintf(fparser, "loop");}
        | LOOP optional_new_lines '(' optional_loop_decl ';' optional_loop_expr ';' optional_loop_expr ')' optional_new_lines loop_body {fprintf(fparser, "loop");}
        ;

optional_loop_expr : expr_pred
        | /* empty */
        ;

optional_loop_decl : expr_or_decl_stmt 
        | /* empty */
        ;

loop_body : '{' increment_scope new_lines loop_stmt_list '}' decrement_scope
        ;

loop_stmt_list : loop_stmt
        | loop_stmt_list loop_stmt
        ;
        
loop_stmt : decl_stmt /* new_lines is included in expr_stmt */
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

loop_conditional_stmt : loop_if_block optional_new_lines loop_else_if_block_list optional_new_lines loop_else { fprintf(fparser, "conditional\n");} new_lines
                | loop_if_block new_lines {fprintf(fparser, "conditional\n");}
                | loop_if_block optional_new_lines loop_else_if_block_list new_lines {fprintf(fparser, "conditional\n");}
                | loop_if_block optional_new_lines loop_else {fprintf(fparser, "conditional\n");} new_lines
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

expr_pred : ID 
        | NUM_CONST
        | REAL_CONST
        | BOOL_CONST
        | expr_pred REL_OP expr_pred
        | expr_pred LT expr_pred
        | expr_pred GT expr_pred
        | expr_pred LOG_OP expr_pred
        | '(' expr_pred ')'
        | NEG_OP expr_pred
        | call_stmt
        | in_built_call_stmt 
        | ID array_element
        | expr_pred BINARY_OP expr_pred
        | expr_pred UNARY_OP
        | INV_OP expr_pred
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

call_stmt : ID '(' arg_list ')' 
        | ID '(' ')' 
        ;

in_built_call_stmt : ID DOT_OP ID '(' arg_list ')'
        | ID DOT_OP ID '(' ')'
        | in_built_call_stmt DOT_OP ID '(' arg_list ')'
        | in_built_call_stmt DOT_OP ID '(' ')'
        ;

arg_list : arg_list ',' arg
        | arg
        ;

arg : expr_pred
    | PATH
    ;

/* -----------------------------------------------------------------------
 * Other Statements
 * ----------------------------------------------------------------------- */

unary_stmt : ID UNARY_OP new_lines
    ;

return_stmt : RETURN expr_pred new_lines
    | RETURN VOID new_lines 
    ;

/* -----------------------------------------------------------------------
 * Useful Actions
 * ----------------------------------------------------------------------- */

set_scope_function : /* empty */ {current_scope = 2;}
    ;

set_scope_outside_function : /* empty */ {current_scope = 0;}
    ;

increment_scope : /* empty */ {current_scope++;}
    ;

decrement_scope : /* empty */ {current_scope--;}
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