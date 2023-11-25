

C_COMPILER = g++
C_FLAGS = -g

YACC = bison
YACC_FLAGS = -vd

LEX = flex

TEST_FILE := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))

BINARY_PATH = ./bin
HEADER_PATH = ./headers

HEADERS = $(wildcard $(HEADER_PATH)/*.hpp) # might have to check if make works with this
SOURCES := $(wildcard $(HEADER_PATH)/*.cpp)

OBJ := $(patsubst $(HEADER_PATH)/%.cpp,$(BINARY_PATH)/%.o,$(SOURCES)) 

EXECUTABLE = $(BINARY_PATH)/output

##@ Frontend

all: lex.yy.c yacc.tab.c $(EXECUTABLE) ## compiles everything

$(EXECUTABLE): $(OBJ) yacc.output ## makes final executable
	@echo "------------------------------------"
	@echo "\033[36mLinking $@...\033[0m"
	$(C_COMPILER) $(C_FLAGS) yacc.tab.c $(OBJ) -o $@
	@echo "\033[32mDone! You can now run $(EXECUTABLE) \033[0m"
	@echo "------------------------------------"


$(BINARY_PATH)/%.o: $(HEADER_PATH)/%.cpp ## compiles all source files without linking
	@echo "------------------------------------"
	@echo "\033[36mCompiling $<...\033[0m"
	$(C_COMPILER) $(C_FLAGS) -c $< -o $@
	@echo "\033[32mDone!\033[0m"

yacc.output: yacc.tab.c ## compiles yacc.tab.c
	@echo "------------------------------------"
	@echo "\033[36mCompiling $<...\033[0m"
	$(C_COMPILER) $(C_FLAGS) -c yacc.tab.c
	@echo "\033[32mDone!\033[0m"


test: $(EXECUTABLE) ## runs the compiler on .art file
	@echo "------------------------------------"
	@echo "\033[36mRunning $<...\033[0m"
	$(EXECUTABLE) < $(TEST_FILE)
	@echo "\033[32mDone!\033[0m"
	@echo "------------------------------------"

##@ Helpers

.PHONY: help
help: ## help command
	@awk 'BEGIN {FS = ":.*##"; printf ("\n\033[1mUsage\033[0m\n  make \033[36m<target>\033[0m\n")} /^[a-zA-Z_0-9\.]+:.*?##/ { printf ("  \033[36m%-15s\033[0m %s\n", $$1, $$2) } /^##@/ { printf ("\n\033[1m%s\033[0m\n", substr($$0, 5)) } '  $(MAKEFILE_LIST)
	
	@echo "\n\033[1mVariables\033[0m"
	@awk '/^[a-zA-Z0-9_-]+ *=/{printf "%-2s\033[36m%-30s\033[0m %s\n","",$$1, $$3}' $(MAKEFILE_LIST) | sort

yacc.tab.c: yacc.y ## Generate yacc.tab.c and yacc.tab.h
	@echo "------------------------------------"
	@echo "\033[36mCompiling $<...\033[0m"
	$(YACC) $(YACC_FLAGS) yacc.y
	@echo "\033[32mDone!\033[0m"


lex.yy.c: lex.l ## Generate lex.yy.c
	@echo "------------------------------------"
	@echo "\033[36mCompiling $<...\033[0m"
	$(LEX) lex.l
	@echo "\033[32mDone!\033[0m"

.PHONY: clean
clean: ## Clean the output files
	-rm output yacc.tab.h yacc.tab.c lex.yy.c yacc.output parsed.txt token.txt $(BINARY_PATH)/*.o $(EXECUTABLE) || true
	