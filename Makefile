CXX		  := g++
CXX_FLAGS := -Wall -Wextra -std=c++20 -ggdb

BIN		:= bin
SRC		:= src
LIB		:= lib # in case we need linking external libraries

LIBRARIES	:=
EXECUTABLE	:= chip8

all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(SRC) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*
