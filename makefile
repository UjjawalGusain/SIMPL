CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I./Lexer

SRC = main.cpp
OBJ = $(SRC:.cpp=.o)
EXE = simpl_lexer

# Default target: builds everything except running
all: $(EXE)

# Separate target to build and run
run: $(EXE)
	./$(EXE) test_file.simpl

# How to link
$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# How to compile object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Clean everything generated
clean:
	rm -f $(OBJ) $(EXE)
