CC		:= gcc
C_FLAGS := -g -Wextra

BIN		:= bin
SRC		:= src
INCLUDE	:= include 
LIB		:= lib

LIBRARIES	:= -lpthread

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= main.exe
else
EXECUTABLE	:= main
endif

all: $(BIN)/$(EXECUTABLE)

clean:
	-$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*
	$(CC) $(C_FLAGS) -I$(INCLUDE) -I$(SRC) -L$(LIB) $^ -o $@ $(LIBRARIES)