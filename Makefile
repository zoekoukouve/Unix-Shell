# paths
INCLUDE = include
MODULES = modules

# compiler
CXX = g++

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CXXFLAGS = -Wall -Werror -g -I$(INCLUDE)

# Αρχεία .o
SRCS = main.cpp execution.o wildcards.o redirection.o pipes.o history.o
OBJS = main.o $(MODULES)/history.o $(MODULES)/wildcards.o $(MODULES)/redirection.o $(MODULES)/pipes.o $(MODULES)/execution.o

# Το εκτελέσιμο πρόγραμμα
EXEC = main

# Παράμετροι για δοκιμαστική εκτέλεση


$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC) $(CXXFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)

valgrind: $(EXEC)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)

	
