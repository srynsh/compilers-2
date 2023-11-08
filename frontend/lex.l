/* Definitions */

%{
#include "y.tab.h"
#include <bits/stdc++.h>
extern FILE* ftoken; // Output file for tokens
extern FILE* fparser; // Output file for parser
extern int lineno;
%}

number [-]?[1-9][0-9]*|0|0[xX][0-9a-fA-F]+
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
comments (#.*)|("#*"([^\*]|[\n\r]|\*[^#])*"*#")
punctuators [\{\}\[\]\(\)';,]
operators "="|"+"|"-"|"*"|"/"|"^"|"~"|"++"|"--"
path ['][a-zA-Z0-9_\/\.\~]+[']


%%

{newline_token} {
    lineno++;
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"newline: \\n\n");
    return NEWLINE;
}

{operators} {
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"operator: %s\n", yytext);

    if(strcmp(yytext,"=")==0) return *yytext;
    else if(strcmp(yytext,"+")==0) return BINARY_OP;
    else if(strcmp(yytext,"-")==0) return BINARY_OP;
    else if(strcmp(yytext,"*")==0) return BINARY_OP;
    else if(strcmp(yytext,"/")==0) return BINARY_OP;
    else if(strcmp(yytext,"^")==0) return BINARY_OP;

    else if(strcmp(yytext,"~")==0) return INV_OP;

    else if(strcmp(yytext,"++")==0) return UNARY_OP;
    else if(strcmp(yytext,"--")==0) return UNARY_OP;
}

{blanks} {
    fprintf(fparser, "%s", yytext);
}

{access_operator} {
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"access_operator: %s\n", yytext);
    return DOT_OP;
}

{logical_operator} {
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"logical_operator: %s\n", yytext);
    if (strcmp(yytext,"!")==0) return NEG_OP;
    return LOG_OP;
}

{relational_operator} {
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"relational_operator: %s\n", yytext);
    if (strcmp(yytext, "<")==0) return LT;
    else if (strcmp(yytext, ">")==0) return GT;
    return REL_OP;
}

{number} {
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"constant: %s\n", yytext);
    yylval.ival = atoi(yytext);
    return NUM_CONST;
}

{real_number} {
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"constant: %s\n", yytext);
    yylval.fval = atof(yytext);
    return REAL_CONST;
}

{arrow} {
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"operator: %s\n", yytext);
    return ARROW;
}

{path} {
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"path: %s\n", yytext);
    return PATH;
}

{keywords} {
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"keyword: %s\n", yytext);

    if (strcmp(yytext,"img")==0) {
        yylval.etype = ELE_TYPE::ELE_IMG;
        return IMG;
    }
    else if (strcmp(yytext,"gray_img")==0) {
        yylval.etype = ELE_TYPE::ELE_GRAY_IMG;
        return GRAY_IMG;
    }
    else if (strcmp(yytext,"vid")==0) {
        yylval.etype = ELE_TYPE::ELE_VID;
        return VID;
    }
    else if (strcmp(yytext,"gray_vid")==0) {
        yylval.etype = ELE_TYPE::ELE_GRAY_VID;
        return GRAY_VID;
    }
    else if (strcmp(yytext,"num")==0) {
        yylval.etype = ELE_TYPE::ELE_NUM;
        return NUM;
    }
    else if (strcmp(yytext,"real")==0) {
        yylval.etype = ELE_TYPE::ELE_REAL;
        return REAL;
    }
    else if (strcmp(yytext,"void")==0) {
        yylval.etype = ELE_TYPE::ELE_VOID;
        return VOID;
    }
    else if (strcmp(yytext,"bool")==0) {
        yylval.etype = ELE_TYPE::ELE_BOOL;
        return BOOL;
    }
    else if (strcmp(yytext,"true")==0) {
        yylval.bval = true;
        return BOOL_CONST;
    } else if (strcmp(yytext,"false")==0) {
        yylval.bval = false;
        return BOOL_CONST;
    } 
    else if (strcmp(yytext,"ink")==0) return INK;
    else if (strcmp(yytext,"spool")==0) return LOOP;
    else if (strcmp(yytext,"break")==0) return BREAK;
    else if (strcmp(yytext,"continue")==0) return CONTINUE;
    else if (strcmp(yytext,"return")==0) return RETURN;
}

{if} {
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"keyword: %s\n", yytext);
    if (strcmp(yytext,"$")==0) return IF;
    else if (strcmp(yytext,"$$")==0) return ELSE_IF;
}

{identifiers} {
    fprintf(fparser,"%s", yytext);
    fprintf(ftoken,"ID: %s\n", yytext);
    auto sval_temp = new std::string(yytext);
    yylval.sval = sval_temp;
    return ID;
}

{comments} {
    // fprintf(fparser,"%s", yytext);
}

{punctuators} {
    fprintf(ftoken, "punctuation: %s\n", yytext);
    fprintf(fparser, "%s", yytext);
    return *yytext;
}
%%