CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I./Lexer

SRC = main.cpp
OBJ = $(SRC:.cpp=.o)
EXE = simpl_lexer

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(EXE)
