output: y.tab.c
	gcc y.tab.c -o output

y.tab.c y.tab.h: yacc.y
	yacc -d yacc.y

lex.yy.c: lex.l
	flex lex.l

clean:
	rm output