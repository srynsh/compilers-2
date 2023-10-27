%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "lex.yy.c"
    #include <string.h>
    int yylex (void);
    void yyerror (char*);
    FILE* ftoken, *fparser;
    int lineno = 1; 
    int return_flag = 0;
%}

%token IF ELSE_IF RETURN CONTINUE BREAK LOOP INK
%token ARROW DOT_OP LOG_OP REL_OP NEG_OP
%token IMG GRAY_IMG VID GRAY_VID NUM REAL VOID BOOL BOOLCONST NEWLINE
%token ID
%start program


%%
// program : program subprogram /* A program is a list of functions and classes; it can also be empty! */
//                 | /* empty */ 
//                 ;
// subprogram : function
//                 | class;
// function : function_definition '{' stmt_list '}' {if(return_flag == 0) {printf("Error at line %d", lineno); fprintf(fparser, " : invalid statement"); exit(1);} else {return_flag = 0;}} /* If there is no return statement, raise an error */
//             ;
// function_definition : scope datatype ID '[' NUM ']' '(' par_list ')' {if($5 == 0) {printf("Error at line %d", lineno); fprintf(fparser, " : invalid statement"); exit(1);}fprintf(fparser, " : function definition");} /* [0] is not allowed */
//                         |  scope datatype ID '(' ')' {fprintf(fparser, " : function definition");}
//                         | scope ID ID '[' NUM ']' '(' par_list ')' {if($5 == 0) {printf("Error at line %d", lineno); fprintf(fparser, " : invalid statement"); exit(1);}fprintf(fparser, " : function definition");} /* the function return type can also be a custom datatype */
//                         | scope ID ID '(' ')' {fprintf(fparser, " : function definition");}
//                         ;
// function_call : ID '[' NUM ']' '(' arg_list ')' {if($3 == 0) {printf("Error at line %d", lineno); fprintf(fparser, " : invalid statement"); exit(1);}} /* as before, [0] is not allowed. function calls are used in call statements, return statements and expressions */
//                 | ID '(' ')' 
//                 ;
// arg_list : arg_list ',' expr_pred /* an argument can consist of an entire expression RHS*/
//                 | expr_pred;
//                 ;
// const : ID /* a rule to encapsulate all constants */
//         | NUM
//         | BOOLCONST
//         | CONST
//         ;
// par_list : par_list ',' par /* the parameters of a function definition (not to be confused with the argument list) */
//             | par 
//             ;
// par : datatype ID 
//         | ID ID; /* for user defined datatypes (like obj_name x) */
//         ;
// stmt_list : stmt_list stmt /* A function body is composed of a statement list */
//             | stmt
//             ;
// stmt : decl_stmt {fprintf(fparser, " : declaration statement");} /* ';' is included in decl_stmt */
//         | conditional_stmt /* No ';' needed for conditional_stmt */
//         | unary_stmt ';'{fprintf(fparser, " : call statement");}
//         | call_stmt ';'{fprintf(fparser, " : call statement");}
//         | call_stmt_object ';'{fprintf(fparser, " : call statement with object");}
//         | expr_stmt {fprintf(fparser, " : expression statement");} /* ';' is included in expr_stmt */
//         | return_stmt  {fprintf(fparser, " : return statement"); return_flag = 1;} /* ';' is included in return_stmt */
//         | loop_stmt /* No ';' is needed for loop_stmt */
//         | '{' stmt_list '}' /* This allows nested scopes */
//         | '{''}' /* An empty scope, just for malicious test cases! */
//         ;
// decl_stmt : DECLARE datatype id_list ';' /* Multiple declares are allowed in a declare statement */
//                 | DECLARE ID id_list ';'  /* As before, the user can define his own datatypes */
//                 ;
// id_list : id_list ',' ID
//                 | ID
//                 ;
// call_stmt : CALL function_call /* If it's a statement, the semicolon is taken care of in the rules for stmt_list */
//             ;
// call_stmt_object : CALL ID ARROW function_call  
//                         | CALL THIS ARROW function_call 
//                         ;
// 
// unary_stmt : UN '(' const ')' /* If it's a statement, the semicolon is taken care of in the rules for stmt_list */
//                 | UN '(' unary_stmt')' /* Allows for nested unary_operators */
//             ;
// conditional_stmt : IN_CASE_THAT '(' expr_pred ')' conditional_action DO '{' stmt_list '}' OTHERWISE {fprintf(fparser, " : conditional statement");}  '{' stmt_list '}' /* As per the documentation, ": conditional statement" should be printed after both the predicate and 'otherwise' */
//                         | IN_CASE_THAT '(' expr_pred ')' conditional_action  DO '{' stmt_list '}' 
//                         ;
// conditional_action : /* empty */
//         {fprintf(fparser, " : conditional statement");} /* The reason for this is explained in the documentation! */
//         ;
// expr_stmt : EXPR ID ASSGN expr_pred ';' 
//             ;
// expr_pred : BIN '(' expr_pred ',' expr_pred ')' /* Takes care of both expressions and predicates, allows call statements, etc. */
//         | UN '(' expr_pred ')'
//         | const /* NUM, BOOLCONST, CONST */
//         | expr_pred REL expr_pred
//         | expr_pred LOG expr_pred
//         | '(' expr_pred ')'
//         | NEG expr_pred
//         | call_stmt /* function call */
//         | call_stmt_object /* function call with object */
//         ;
// 
// return_stmt :  RETURN VOID ';'
//                 | RETURN expr_pred ';' /* All sorts of things can be returned */
//                 ;
// loop_stmt : LOOP WHILE '(' expr_pred ')' {fprintf(fparser, " : loop");} DO '{' stmt_list '}' 
//             | FOR '(' expr_stmt  expr_pred ';' unary_stmt ')' {fprintf(fparser, " : loop");} '{' stmt_list '}' 
//             | FOR '(' expr_stmt  expr_pred ';' ')' {fprintf(fparser, " : loop");} '{' stmt_list '}' 
//             ;
// 
// class : class_definition '{' class_list '}' /* There are two types of classes, those with methods and those without */
//         | class_definition_2 '{' class_list_2 '}' 
//         | class_definition '{' '}' /* A corner case, classes with an empty body */
//         | class_definition_2 '{' '}'
//         ;
// class_definition : CLASS ID '[' NUM ']' {if($4 == 0) {printf("Error at line %d", lineno); fprintf(fparser, " : invalid statement"); exit(1);}fprintf(fparser, " : class definition");}
//                     ; /* As in other places, [0] is not allowed */
// class_definition_2 : CLASS ID {fprintf(fparser, " : class definition");} 
//                     ;
// 
// class_list : class_list class_stmt /* The class body with methods */
//                 | class_stmt
//                 ;
// class_stmt : decl_stmt {fprintf(fparser, " : declaration statement");}
//                 | function
//                 ;
// class_list_2 : class_list_2 class_stmt_2 /* The class body without methods */
//                 | class_stmt_2
//                 ;
// class_stmt_2 : decl_stmt {fprintf(fparser, " : declaration statement");}
//                 ;
// 
// scope : GLOBAL | LOCAL 
//         ;
// datatype : INT | CHAR | STRING | BOOL | VOID 
//             ; 



%%

int yywrap(){ return 1;}
void yyerror(char* s){ 
        printf("Error at line %d", lineno);
        fprintf(fparser, " : invalid statement");
        exit(1);
}
int main(int argc, char* argv[]){

    extern FILE* ftoken, *fparser;
    int n = strlen(argv[1]);
    char* name = (char*)malloc(sizeof(char)*(n+50));
    char* tname = (char*)malloc(sizeof(char)*(n+50));
    char* pname = (char*)malloc(sizeof(char)*(n+50));
    /* strcpy(tname, "seq_tokens_");
    strcpy(pname, "parser_") */
    sprintf(name, "%s.clike", argv[1]);
    sprintf(tname, "seq_tokens_%s.txt", argv[1]);
    sprintf(pname, "parser_%s.parsed", argv[1]);

    ftoken = fopen(tname, "w");
    fparser = fopen(pname, "w");

    fprintf(ftoken, "Name: Rahul Ramachandran\nID: CS21BTECH11049\n"); // First two lines of token file
    yyin = fopen(name, "r");
    yyout = fparser;
    yyparse(); 

    return 0;
}