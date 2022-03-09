/**
 * header file for parsing the command line arguments
 **/  
#ifndef PROC_ARGS
#define PROC_ARGS

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>


void print_command_usage(char executable_name[]);
int get_wav_args(int argc, char *argv[], int* bit_ptr, char wav_file_name[], char text_file_name[]); 
#endif