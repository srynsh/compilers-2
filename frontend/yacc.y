%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "lex.yy.c"
    #include <string.h>
    int yylex (void);
    FILE* ftoken, *fparser;
    int lineno = 1; 
    void yyerror (char*);
    int return_flag = 0;
%}

/*
ink main() -> void 
{ 
    
}

or 

ink main() -> void {

}

(id = id} is invalid)
*/

%token IF ELSE_IF RETURN CONTINUE BREAK LOOP INK
%token ARROW DOT_OP LOG_OP REL_OP NEG_OP
%token IMG GRAY_IMG VID GRAY_VID NUM REAL VOID BOOL BOOL_LIT NEWLINE
%token ID NUM_LIT REAL_LIT PATH BINARY_OP UNARY_OP INV_OP 
%start program

%left DOT_OP
%left UNARY_OP
%right INV_OP NEG_OP
%left BINARY_OP
%left LOG_OP
%left REL_OP


/* %right IF */
/* %right ELSE_IF */
%right SHORT_IF
%right LONG_IF
%%

program : function new_lines program
        | function optional_new_lines
        ;

func_body : '{' stmt_list '}'
        | '{' new_lines stmt_list '}'
        ;

new_lines : new_lines NEWLINE
        | NEWLINE
        ;  

optional_new_lines : /* empty */
        | new_lines
        ; 

function : INK ID '(' par_list ')' ARROW RET_TYPE func_body
        | INK ID '(' ')' ARROW RET_TYPE func_body
        | INK ID '(' par_list ')' ARROW RET_TYPE new_lines func_body
        | INK ID '(' ')' ARROW RET_TYPE new_lines func_body
        ;

RET_TYPE : datatype
        | VOID
        ;
        
par_list : par_list ',' par
        | par
        ;

par : datatype ID
    ;

datatype : IMG
        | GRAY_IMG
        | VID
        | GRAY_VID
        | NUM
        | REAL
        | BOOL
        ;

stmt_list : stmt 
        | stmt_list stmt 
        ;

stmt : decl_stmt /* 'new_lines' is included in expr_stmt */
        | conditional_stmt /* 'new_lines' is included in conditional_stmt */
        | call_stmt new_lines
        | in_built_call_stmt new_lines
        | expr_stmt /* 'new_lines' is included in expr_stmt */
        | return_stmt /* 'new_lines' is included in expr_stmt */
        | loop_stmt new_lines
        | '{'new_lines stmt_list '}' new_lines /* This allows nested scopes */
        | '{' stmt_list '}' new_lines /* This allows nested scopes */
        | '{' new_lines '}' new_lines
        | '{' '}' new_lines
        ; 

optional_expr_pred : expr_pred
        | /* empty */
        ;

optional_num_data_decl : numeric_data_decl
        | /* empty */
        ;

loop_stmt : LOOP optional_new_lines '(' optional_expr_pred ')' optional_new_lines func_body {fprintf(fparser, "loop");}
        | LOOP optional_new_lines '(' optional_num_data_decl ';' optional_expr_pred ';' optional_expr_pred ')' optional_new_lines func_body {fprintf(fparser, "loop");}
        ;

if_block : IF optional_new_lines '(' expr_pred')' optional_new_lines ARROW optional_new_lines func_body 
        ;

else : ARROW optional_new_lines func_body
        ;

else_if_block_list : else_if_block_list optional_new_lines ELSE_IF '(' expr_pred ')' optional_new_lines ARROW optional_new_lines func_body
        | ELSE_IF '(' expr_pred ')' optional_new_lines ARROW optional_new_lines func_body 
        ;

conditional_stmt : if_block optional_new_lines else_if_block_list optional_new_lines else { fprintf(fparser, "conditional\n");} new_lines
                | if_block new_lines {fprintf(fparser, "conditional\n");}
                | if_block optional_new_lines else_if_block_list new_lines {fprintf(fparser, "conditional\n");}
                | if_block optional_new_lines else {fprintf(fparser, "conditional\n");} new_lines
                ;

numeric_data_decl : num_decl
        | real_decl
        | bool_decl
        ;

empty_return : /* empty */ {fprintf(fparser, "return");}
        ;

return_stmt : RETURN expr_pred empty_return new_lines
        | RETURN VOID empty_return new_lines 
        ;

decl_stmt : img_decl 
        | gray_img_decl
        | vid_decl
        | gray_vid_decl
        | num_decl new_lines
        | bool_decl new_lines
        | real_decl new_lines 
        | num_array_decl
        | real_array_decl
        ;

img_decl : IMG ID '<' NUM_LIT ',' NUM_LIT  '>' new_lines
        | IMG ID '<' NUM_LIT ',' NUM_LIT ',' NUM_LIT '>' new_lines
        | IMG ID '<' PATH '>' new_lines
        | IMG ID '=' expr_pred new_lines
        ; 

gray_img_decl : GRAY_IMG ID '<' NUM_LIT ',' NUM_LIT '>' new_lines
        | GRAY_IMG ID '<' NUM_LIT ',' NUM_LIT ',' NUM_LIT '>' new_lines
        | GRAY_IMG ID '<' PATH '>' new_lines
        | GRAY_IMG ID '=' expr_pred new_lines
        ;

vid_decl : VID ID '<' NUM_LIT ',' NUM_LIT '>' new_lines
        | VID ID '<' NUM_LIT ',' NUM_LIT ',' NUM_LIT '>' new_lines
        ;

gray_vid_decl : GRAY_VID ID '<' NUM_LIT ',' NUM_LIT '>' new_lines
        | GRAY_VID ID '<' NUM_LIT ',' NUM_LIT ',' NUM_LIT '>' new_lines
        ;

num_decl : NUM ID
        | NUM ID '=' expr_pred
        ;

bool_decl : BOOL ID
        | BOOL ID '=' expr_pred
        ;

real_decl : REAL ID
        | REAL ID '=' expr_pred
        ;

num_array_decl : NUM array_element ID new_lines
                | NUM array_element ID '=' ID new_lines   
                | NUM array_element ID '=' brak_pred new_lines
                ;

real_array_decl : REAL array_element ID new_lines
                | REAL array_element ID '=' ID new_lines
                | REAL array_element ID '=' brak_pred new_lines 
                ;

brak_pred : '{' brak_pred_list '}'
          | '{' lit_list '}'
          ;

brak_pred_list : brak_pred_list ',' brak_pred
               | brak_pred
               ;

lit_list : lit_list ',' lit
         | lit
         ;
        
lit : NUM_LIT
    | REAL_LIT
    | BOOL_LIT
    ;


call_stmt : ID '(' arg_list ')' 
        | ID '(' ')' 
        ;

arg_list : arg_list ',' expr_pred
        | expr_pred
        ;

in_built_call_stmt : ID DOT_OP ID '(' arg_list ')'
        | ID DOT_OP ID '(' ')'
        | in_built_call_stmt DOT_OP ID '(' arg_list ')'
        | in_built_call_stmt DOT_OP ID '(' ')'
        ;

expr_pred : ID 
        | NUM_LIT
        | REAL_LIT
        | BOOL_LIT
        | expr_pred REL_OP expr_pred
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


expr_stmt : ID '=' expr_pred new_lines
        | ID array_element '=' expr_pred new_lines
        ;

array_element : '[' expr_pred ']'
        |  '[' expr_pred ',' expr_pred ']'
        |  '[' expr_pred ',' expr_pred ',' expr_pred ']'
        ;


%%

int yywrap(){ return 1;}
void yyerror(char* s){ 
        printf("Error at line %d\n", lineno);
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