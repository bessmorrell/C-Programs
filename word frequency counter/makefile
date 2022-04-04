# simple makefile for compiling

SOURCE = word_freqs.c
EXECUTABLE = word_freqs
COMPILE_FLAGS = -ansi -pedantic -Wall -g

all: $(SOURCE)
	gcc $(COMPILE_FLAGS) -o $(EXECUTABLE) $(SOURCE)

tidy:
	clang-tidy -checks='*' $(SOURCE) -- -std=c99

clean:
	rm -rf $(EXECUTABLE)
