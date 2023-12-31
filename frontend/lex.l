/* Definitions */

%{
#include <bits/stdc++.h>
#include "yacc.tab.h"
#include "headers/sym_tab.hpp"
#include "headers/semantic.hpp"
extern FILE* ftoken; // Output file for tokens
extern FILE* fparser; // Output file for parser
extern int lineno;
int block_comment = 0;
int inline_comment = 0;

extern void yyerror(const char *s);
%}

number [-]?[1-9][0-9]*|0|0[xX][0-9a-fA-F]+
real_number {number}(\.[0-9]+)?
identifiers [_a-zA-Z][a-zA-Z0-9_]*
keywords "img"|"gray_img"|"vid"|"gray_vid"|"num"|"real"|"spool"|"break"|"continue"|"return"|"ink"|"void"|"bool"|"true"|"false"|"sketch"
logical_operator "and"|"or"|"!"
relational_operator ">"|">="|"=="|"!="|"<"|"<=" 
access_operator "."
arrow "->"
newline_token [\n\r]
blanks [ \t]+
if "$"|"$$"
comments "#"|"#*"|"*#"
punctuators [\{\}\[\]\(\)';,]
operators "="|"+"|"-"|"*"|"/"|"^"|"~"|"++"|"--"|"<<"
path \'([^'\\\n])*\'
reserved "__i__"|"__t__"|"__tglobal__"

%%

{newline_token} {
    // std::cout << "hello 11" << yytext << std::endl;
    if(strcmp(yytext, "\n") == 0) {
        if (inline_comment)
            inline_comment = 0;
        lineno++;   // increment line number on encountering new line
        // std::cout << "Line number: " << yylineno << std::endl;
    }
    return NEWLINE;
}

{operators} {
    // std::cout << "hello 10" << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"operator: %s\n", yytext);

        if(strcmp(yytext,"=")==0) return *yytext;
        else if(strcmp(yytext,"+")==0) {
            OPERATOR op_temp = OPERATOR::ADD;
            yylval.opval = op_temp;
            return BINARY_OP;
        } else if(strcmp(yytext,"-")==0) {
            OPERATOR op_temp = OPERATOR::SUB;
            yylval.opval = op_temp;
            return BINARY_OP;
        } else if(strcmp(yytext,"*")==0) {
            OPERATOR op_temp = OPERATOR::MUL;
            yylval.opval = op_temp;
            return BINARY_OP;
        } else if(strcmp(yytext,"/")==0) {
            OPERATOR op_temp = OPERATOR::DIV;
            yylval.opval = op_temp;
            return BINARY_OP;
        } else if(strcmp(yytext,"^")==0) {
            OPERATOR op_temp = OPERATOR::XOR;
            yylval.opval = op_temp;
            return BINARY_OP;
        } else if(strcmp(yytext,"~")==0) {
            OPERATOR op_temp = OPERATOR::INV;
            yylval.opval = op_temp;
            return INV_OP;
        } else if(strcmp(yytext,"++")==0) {
            OPERATOR op_temp = OPERATOR::POST_INCR;
            yylval.opval = op_temp;
            return UNARY_OP;
        } else if(strcmp(yytext,"--")==0) {
            OPERATOR op_temp = OPERATOR::POST_DECR;
            yylval.opval = op_temp;
            return UNARY_OP;
        } else if(strcmp(yytext,"<<")==0) {
            OPERATOR op_temp = OPERATOR::LSHIFT;
            yylval.opval = op_temp;
            return SKETCH_OP;
        }
    }
}

{blanks} {
    // std::cout << "hello 9" << yytext << std::endl;
    fprintf(fparser, "%s", yytext);
}

{access_operator} {
    // std::cout << "hello 8" << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"access_operator: %s\n", yytext);
        return DOT_OP;
    }
}

{logical_operator} {
    // std::cout << "hello 7" << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"logical_operator: %s\n", yytext);
        if (strcmp(yytext,"!")==0) {
            OPERATOR op_temp = OPERATOR::NOT;
            yylval.opval = op_temp;
            return NEG_OP;
        }
        else if (strcmp(yytext,"and")==0) {
            OPERATOR op_temp = OPERATOR::AND;
            yylval.opval = op_temp;
            return LOG_OP;
        }
        else if (strcmp(yytext,"or")==0) {
            OPERATOR op_temp = OPERATOR::OR;
            yylval.opval = op_temp;
            return LOG_OP;
        }
    }
}

{relational_operator} {
    // std::cout << "hello 6" << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"relational_operator: %s\n", yytext);
        OPERATOR op_temp;
        if (strcmp(yytext, "<")==0) {
            op_temp = OPERATOR::LESSER;
        }
        else if (strcmp(yytext, "<=")==0) {
            op_temp = OPERATOR::LTEQ;
        }
        else if (strcmp(yytext, ">")==0) {
            op_temp = OPERATOR::GREATER;
        }
        else if (strcmp(yytext, ">=")==0) {
            op_temp = OPERATOR::GTEQ;
        }
        else if (strcmp(yytext, "==")==0) {
            op_temp = OPERATOR::EQUAL;
        }
        else if (strcmp(yytext, "!=")==0) {
            op_temp = OPERATOR::NEQ;
        }
        yylval.opval = op_temp;
        if (strcmp(yytext, "<")==0) return LT;
        else if (strcmp(yytext, ">")==0) return GT;
        return REL_OP;
    }
}

{number} {
    // std::cout << "hello 5" << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"constant: %s\n", yytext);
        // If not hex
        if (yytext[0] != '0' || yytext[1] != 'x')
            yylval.ival = atoi(yytext);
        else 
            yylval.ival = (int)strtol(yytext, NULL, 16);
        return NUM_CONST;
    }
}

{real_number} {
    // std::cout << "hello4 " << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"constant: %s\n", yytext);
        yylval.fval = atof(yytext);
        return REAL_CONST;
    }
}

{arrow} {
    // std::cout << "hello3 " << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"operator: %s\n", yytext);
        return ARROW;
    }
}

{path} {
    // std::cout << "hello2 " << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"path: %s\n", yytext);
        struct type_info* tinfo = new struct type_info;
        tinfo->eleType = ELETYPE::ELE_STR;
        tinfo->name = yytext;
        yylval.tval = tinfo;
        return PATH;
    }
}

{reserved} {
    // std::cout << "hello1 " << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"reserved: %s\n", yytext);
        std::string s = "Reserved identifier used: " + std::string(yytext);
        yyerror(s.c_str());
        exit(1);
    }

}

{keywords} {
    // std::cout << "hello " << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"keyword: %s\n", yytext);

        struct type_info* tinfo = new struct type_info;
        tinfo->type = TYPE::SIMPLE;

        if (strcmp(yytext,"img")==0) {
            tinfo->eleType = ELETYPE::ELE_IMG;
            std::vector<int>* dim_list = new std::vector<int>(3);
            dim_list->at(0) = -1; dim_list->at(1) = -1; dim_list->at(2) = 3;
            tinfo->dim_list = dim_list;
            yylval.tval = tinfo;
            return IMG;
        }
        else if (strcmp(yytext,"gray_img")==0) {
            tinfo->eleType = ELETYPE::ELE_GRAY_IMG;
            std::vector <int>* dim_list = new std::vector<int>(3);
            dim_list->at(0) = -1; dim_list->at(1) = -1; dim_list->at(2) = 3;
            tinfo->dim_list = dim_list;
            yylval.tval = tinfo;
            return GRAY_IMG;
        }
        else if (strcmp(yytext,"vid")==0) {
            tinfo->eleType = ELETYPE::ELE_VID;
            std::vector<int>* dim_list = new std::vector<int>(4);
            dim_list->at(0) = -1; dim_list->at(1) = -1; dim_list->at(2) = 3, dim_list->at(3) = -1; 
            tinfo->dim_list = dim_list; 
            yylval.tval = tinfo;
            return VID;
        }
        else if (strcmp(yytext,"gray_vid")==0) {
            tinfo->eleType = ELETYPE::ELE_GRAY_VID;
            std::vector <int>* dim_list = new std::vector<int>(4);
            dim_list->at(0) = -1; dim_list->at(1) = -1; dim_list->at(2) = 1; dim_list->at(3) = -1; 
            tinfo->dim_list = dim_list;
            yylval.tval = tinfo;
            return GRAY_VID;
        }
        else if (strcmp(yytext,"num")==0) {
            tinfo->eleType = ELETYPE::ELE_NUM;
            yylval.tval = tinfo;
            return NUM;
        }
        else if (strcmp(yytext,"real")==0) {
            tinfo->eleType = ELETYPE::ELE_REAL;
            yylval.tval = tinfo;
            return REAL;
        }
        else if (strcmp(yytext,"void")==0) {
            tinfo->eleType = ELETYPE::ELE_VOID;
            yylval.tval = tinfo;
            return VOID;
        }
        else if (strcmp(yytext,"bool")==0) {
            tinfo->eleType = ELETYPE::ELE_BOOL;
            yylval.tval = tinfo;
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
        else if (strcmp(yytext,"sketch")==0) return SKETCH;
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
    // std::cout << "hello 12" << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"ID: %s\n", yytext);
        auto sval_temp = new std::string(yytext);
        yylval.sval = sval_temp; 
        return ID;
    }
}

{comments} {
    // std::cout << "hello 13" << yytext << std::endl;
    if (!inline_comment && !block_comment) {
        if (strcmp(yytext,"#*")==0) block_comment = 1;
        else if (strcmp(yytext,"#")==0) inline_comment = 1;
        else if (strcmp(yytext,"*#")==0) {
            fprintf(fparser,"%s", yytext);
            fprintf(ftoken,"error: %s\n", yytext);
            yyerror("Invalid token");
            // exit(1);
        }
    } else if (block_comment) {
        if (strcmp(yytext,"*#")==0) block_comment = 0;
    }
} 

{punctuators} {
    if (!inline_comment && !block_comment) {
        fprintf(ftoken, "punctuation: %s\n", yytext);
        fprintf(fparser, "%s", yytext);
        return *yytext;
    }
}

. {
    if (!inline_comment && !block_comment) {
        fprintf(fparser,"%s", yytext);
        fprintf(ftoken,"error: %s\n", yytext);
        yyerror("Invalid token");
        // exit(1);
        // return *yytext;
    }
}
 
%%