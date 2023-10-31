run: output
	@echo "------------------------------------"
	$(info Usage: make file=filename.txt or make file=filename.txt run)
	./output < $(file)
	@echo "------------------------------------"

output: y.tab.c lex.yy.c 
	@echo "------------------------------------"
	gcc y.tab.c -o output

y.tab.c y.tab.h: yacc.y
	@echo "------------------------------------"
	yacc -d -Wcounterexamples yacc.y

lex.yy.c: y.tab.h lex.l 
	@echo "------------------------------------"
	flex lex.l


clean:
	rm output y.tab.h y.tab.c lex.yy.c y.output parsed.txt token.txt