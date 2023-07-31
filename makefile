CXX = $(shell root-config --cxx)
FLAGS = $(shell root-config --cflags) -O2 -Wall -Wextra
LIBS = $(shell root-config --evelibs)
ARCH = x86_64
SOURCES = ./analiz.cpp
PROGRAM = satforce
YELLOW = \033[1;33m
CYAN = \033[1;36m
GREEN = \033[1;32m
RESET = \033[0m

all: clean $(PROGRAM) run 

$(PROGRAM): $(SOURCES)
	@echo "$(YELLOW)Compiling...$(RESET)"
	$(CXX) $^ $(FLAGS) $(LIBS) -o $@  
	@echo "$(GREEN)Done!$(RESET)"
	@echo "$(GREEN)Execute the command$(RESET) $(CYAN)./$(PROGRAM)$(RESET) $(GREEN)or$(RESET) $(CYAN)make run$(RESET) $(GREEN)to run the program.$(RESET)"

run: 
	@./$(PROGRAM) 


clean:
	rm -rf ./$(PROGRAM)


again: 
	@make clean
	@make
	@make run
