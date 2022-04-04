# simple makefile for compiling

EXECUTABLE = lab5
COMPILE_FLAGS = -ansi -pedantic -Wall

$(EXECUTABLE): audio_decoder.o get_wav_args.o
	gcc -o $(EXECUTABLE) audio_decoder.o get_wav_args.o 

lab5.o: lab5.c get_wav_args.h 
	gcc $(COMPILE_FLAGS) -c audio_decoder.c

get_wav_args.o: get_wav_args.h get_wav_args.c
	gcc $(COMPILE_FLAGS) -c get_wav_args.c

tidy: $(SOURCE)
	clang-tidy -checks='*' audio_decoder.c -- -std=c99

clean:
	rm -rf $(EXECUTABLE) *.o
