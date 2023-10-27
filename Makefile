output: y.tab.c lex.yy.c 
	gcc y.tab.c -o output

y.tab.c y.tab.h: yacc.y
	yacc -d yacc.y

lex.yy.c: y.tab.h lex.l 
	flex lex.l

clean:
	rm output y.tab.h y.tab.c lex.yy.c