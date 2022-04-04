# simple makefile for compiling

EXECUTABLE = lab5
COMPILE_FLAGS = -ansi -pedantic -Wall

$(EXECUTABLE): lab5.o get_wav_args.o
	gcc -o $(EXECUTABLE) lab5.o get_wav_args.o 

lab5.o: lab5.c get_wav_args.h 
	gcc $(COMPILE_FLAGS) -c lab5.c

get_wav_args.o: get_wav_args.h get_wav_args.c
	gcc $(COMPILE_FLAGS) -c get_wav_args.c

tidy: $(SOURCE)
	clang-tidy -checks='*' lab5.c -- -std=c99

clean:
	rm -rf $(EXECUTABLE) *.o
