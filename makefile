CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I./Lexer -I./Parser -I./semantic_analyzer/include -I./IR -I./CodeGeneration -I./Interpreter

SRC = main.cpp $(wildcard Lexer/*.cpp) $(wildcard Parser/*.cpp) $(wildcard semantic_analyzer/src/*.cpp) $(wildcard IR/*.cpp) $(wildcard CodeGeneration/*.cpp) $(wildcard Interpreter/*.cpp)


OBJ = $(SRC:.cpp=.o)

EXE = simpl_lexer

all: $(EXE)

run: $(EXE)
	./$(EXE) test_file.simpl

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q /F $(subst /,\,$(OBJ)) $(EXE) 2>nul
	for /d %%D in (Lexer Parser semantic_analyzer\src IR CodeGeneration Interpreter) do (del /Q /F %%D\*.o 2>nul)

