OUTFILE = output

HEADERS = ./headers/sym_tab.cpp ./headers/semantic.cpp ./headers/utils.cpp

C_COMPILER = g++

YACC = bison
YACC_FLAGS = -vd

LEX = flex
LEX_FLAGS = 

TEST = test1.art

run: output
	@echo "------------------------------------"
	$(info Usage: make TEST=filename.txt or make TEST=filename.txt run)
	
	./output < $(TEST)
	@echo "------------------------------------"

output: lex yacc
	@echo "------------------------------------"
	$(C_COMPILER) -g yacc.tab.c $(HEADERS) -o $(OUTFILE)

yacc: yacc.y
	@echo "------------------------------------"
	$(YACC) $(YACC_FLAGS) yacc.y

lex: lex.l 
	@echo "------------------------------------"
	$(LEX) $(LEX_FLAGS) lex.l


clean:
	-rm output yacc.tab.h yacc.tab.c lex.yy.c yacc.output parsed.txt token.txt || true
	