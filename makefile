CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I./Lexer -I./Parser -I./semantic_analyzer/include

# Add specific source files to the list with correct directory names
SRC = main.cpp $(wildcard Lexer/*.cpp) $(wildcard Parser/*.cpp) $(wildcard semantic_analyzer/src/*.cpp)



# Object files corresponding to the above source files
OBJ = $(SRC:.cpp=.o)

# Executable file
EXE = simpl_lexer

# Default target: builds everything except running
all: $(EXE)

# Separate target to build and run
run: $(EXE)
	./$(EXE) test_file.simpl

# How to link object files into the final executable
$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# How to compile object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean everything generated (works on Unix-like systems)
clean:
	rm -f $(OBJ) $(EXE)
