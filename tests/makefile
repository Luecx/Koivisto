CC       = g++
SRC      = *.cpp
FOLDER   = bin/
ROOT     = ../
NAME     = tests
EXE      = $(ROOT)$(FOLDER)$(NAME)

ifeq ($(OS),Windows_NT)
	NAME := $(NAME)-windows.exe
else
	NAME := $(NAME)-linux
endif

WFLAGS = -Wall -Wextra -Wshadow -pedantic -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2
DFLAGS = -g -fsanitize=address
FLAGS  = $(WFLAGS) $(DFLAGS) -O0 -std=c++20

all:
	mkdir -p $(ROOT)$(FOLDER)
	$(CC) $(FLAGS) $(SRC) -o $(EXE)
