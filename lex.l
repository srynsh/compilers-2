/* Definitions */

%{
#include "y.tab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
extern FILE* ftoken; // Output file for tokens
extern FILE* fparser; // Output file for parser
extern int lineno;

int inline_comment = 0;
int block_comment = 0;
%}

number [-]?[1-9][0-9]*|0
real_number {number}(\.[0-9]+)?
identifiers [_a-zA-Z][a-zA-Z0-9_]*
keywords "img"|"gray_img"|"vid"|"gray_vid"|"num"|"real"|"spool"|"break"|"continue"|"return"|"ink"|"void"|"bool"|"true"|"false"
logical_operator "and"|"or"|"!"
relational_operator ">"|">="|"=="|"!="|"<"|"<=" 
access_operator "."
arrow "->"
newline_token [\n\r]
blanks [ \t]+
if "$"|"$$"
comments "#"|"#*"|"*#"
punctuators [\{\}\[\]\(\)';,]


%%

{newline_token} {
    if (inline_comment) {
        inline_comment = 0;
    }

    if (!block_comment) {
        fprintf(fparser,"%s", yytext);
        return NEWLINE;
    }
}

{blanks} {
    fprintf(fparser, "%s", yytext);
}

{access_operator} {
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"access_operator: %s\n", yytext);
        return DOT_OP;
    }
}

{logical_operator} {
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"logical_operator: %s\n", yytext);
        if (strcmp(yytext,"!")==0) return NEG_OP;
        return LOG_OP;
    }
}

{relational_operator} {
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"relational_operator: %s\n", yytext);
        if (strcmp(yytext, "<")==0) return *yytext;
        else if (strcmp(yytext, ">")==0) return *yytext;
        return REL_OP;
    }
}

{number} {
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"constant: %s\n", yytext);
        return NUM;
    }
}

{real_number} {
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"constant: %s\n", yytext);
        return REAL;
    }
}

{arrow} {
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"operator: %s\n", yytext);
        return ARROW;
    }
}

{keywords} {
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"keyword: %s\n", yytext);

        if (strcmp(yytext,"img")==0) return IMG;
        else if (strcmp(yytext,"gray_img")==0) return GRAY_IMG;
        else if (strcmp(yytext,"vid")==0) return VID;
        else if (strcmp(yytext,"gray_vid")==0) return GRAY_VID;
        else if (strcmp(yytext,"num")==0) return NUM;
        else if (strcmp(yytext,"real")==0) return REAL;
        else if (strcmp(yytext,"void")==0) return VOID;
        else if (strcmp(yytext,"bool")==0) return BOOL;
        else if (strcmp(yytext,"true")==0) return BOOLCONST;
        else if (strcmp(yytext,"false")==0) return BOOLCONST;
        else if (strcmp(yytext,"ink")==0){
            printf("Ink\n");
            return INK;
        }
        else if (strcmp(yytext,"spool")==0) return LOOP;
        else if (strcmp(yytext,"break")==0) return BREAK;
        else if (strcmp(yytext,"continue")==0) return CONTINUE;
        else if (strcmp(yytext,"return")==0) return RETURN;
    }
}

{if} {
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"keyword: %s\n", yytext);
        if (strcmp(yytext,"$")==0) return IF;
        else if (strcmp(yytext,"$$")==0) return ELSE_IF;
    }
}

{identifiers} {
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"ID: %s\n", yytext);
        return ID;
    }
}

{comments} {
    fprintf(fparser,"%s", yytext);
    if (!inline_comment && !block_comment) {
        if (strcmp(yytext,"#*")==0) block_comment = 1;
        else if (strcmp(yytext,"#")==0) inline_comment = 1;
    } else if (block_comment) {
        if (strcmp(yytext,"*#")==0) block_comment = 0;
    }
}

{punctuators} {
    fprintf(ftoken, "punctuation: %s\n", yytext);
    fprintf(fparser, "%s", yytext);
    return *yytext;
}
%%