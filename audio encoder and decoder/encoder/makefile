# simple makefile for compiling

EXECUTABLE = audio_encoder
COMPILE_FLAGS = -ansi -pedantic -Wall

$(EXECUTABLE): audio_encoder.o get_wav_args.o
	gcc -o $(EXECUTABLE) audio_encoder.o get_wav_args.o 

lab6.o: lab6.c get_wav_args.h 
	gcc $(COMPILE_FLAGS) -c audio_encoder.c

get_wav_args.o: get_wav_args.h get_wav_args.c
	gcc $(COMPILE_FLAGS) -c get_wav_args.c

tidy: $(SOURCE)
	clang-tidy -checks='*' audio_encoder.c -- -std=c99

clean:
	rm -rf $(EXECUTABLE) *.o
