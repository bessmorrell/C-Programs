#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include "ezipc.h"

int producer(char filename[]);
void squash();
void print();

//semaphores
int empty1; //number of empty buffers for shared memory 1
int mutex1; //mutual exclusion for shared memory 1
int full1; //number of full buffers for shared memory 1
int empty2; //number of empty buffers for shared memory 2
int mutex2; //mutual exclusion for shared memory 2
int full2; //number of full buffers for shared memory 2

//shared memory
char *ch1; //shared memory between producer and sqash
char *ch2; //shared memory between squash and print

int main(int argc, char *argv[]) {

    SETUP();

    int pid; //process id used to identify child processes
    int status; //status of concurrent processes

    //shared memory 1
    ch1 = SHARED_MEMORY(sizeof(char));
    //shared memory 2
    ch2 = SHARED_MEMORY(sizeof(char));
    //semaphores empty (initialized to 1)
    empty1 = SEMAPHORE(SEM_BIN, 1);
    empty2 = SEMAPHORE(SEM_BIN, 1);
    //semaphores mutex (initialized to 1)
    mutex1 = SEMAPHORE(SEM_BIN, 1);
    mutex2 = SEMAPHORE(SEM_BIN, 1);
    //semaphore full (initialized to 0)
    full1 = SEMAPHORE(SEM_BIN, 0);
    full2 = SEMAPHORE(SEM_BIN, 0);

//fork 3 processes

//Producer
    pid=fork();
    if(pid == -1){
        perror("error creating process");
        exit(1);
    }
    else if(pid==0) producer("conway.txt");


//Squash
    pid=fork();
    if(pid == -1){
        perror("error creating process");
        exit(1);
    }
    else if(pid==0) squash();


//Print
    pid=fork();
    if(pid == -1){
        perror("error creating process");
        exit(1);
    }
    else if(pid==0) print();

    //wait for processes to terminate
    while (wait(&status)!=-1);

    return 0;
}

/*
this process reads chars from file and places them into shared memory ch1
    with squash(). If producer reads '\n', it will instead send '<EOL>'
*/
int producer(char filename[]){

    FILE *in_file;//pointer to file
    char temp; //holds char that we read in from file

    in_file = fopen(filename, "r");

    //error check file
    if(!in_file) {
        printf("could not open text file %s \n", filename);
        return 2;
    }

    //loop length of file and read char temp
    while((temp = fgetc(in_file)) != EOF){

        //BOUNDED BUFFER PRODUCER-CONSUMER MODEL

        //decrease empty1 and mutex
        P(empty1);
        P(mutex1);

        //Special case: add <EOF> in place of \n
        if(temp == '\n'){
            *ch1 = '<';
            V(mutex1);
            V(full1);

            P(empty1);
            P(mutex1);
            *ch1 = 'E';
            V(mutex1);
            V(full1);

            P(empty1);
            P(mutex1);
            *ch1 = 'O';
            V(mutex1);
            V(full1);

            P(empty1);
            P(mutex1);
            *ch1 = 'L';
            V(mutex1);
            V(full1);

            P(empty1);
            P(mutex1);
            *ch1 = '>';

        }

        //Regular case: put temp in shared memory
        else {
            *ch1 = temp;
        }
        
        //increase mutex1 and full1
        V(mutex1);
        V(full1);
    }

    //add one last <EOF> after reaching the end of the file
    P(empty1);
    P(mutex1);
    *ch1 = '<';
    V(mutex1);
            V(full1);

            P(empty1);
            P(mutex1);
            *ch1 = 'E';
            V(mutex1);
            V(full1);

            P(empty1);
            P(mutex1);
            *ch1 = 'O';
            V(mutex1);
            V(full1);

            P(empty1);
            P(mutex1);
            *ch1 = 'L';
            V(mutex1);
            V(full1);

            P(empty1);
            P(mutex1);
            *ch1 = '>';
            V(mutex1);
            V(full1);

    //close file
    if(in_file) fclose(in_file);

    return 0;
}

/* 
    This process reads from shared memory ch1 and passes characters to shared 
    memory ch2. If squash reads two *s in a row, it will send '#' instead.
*/
void squash(){

    char temp;//holds the char in shared memory

    while(1){//always true

        //decrease pointers full1, mutex1
        P(full1);
        P(mutex1);

        //read character from buffer
        temp = *ch1;

        //increase pointers mutex1, full1
        V(mutex1);
        V(empty1);

        //Special case: if char is an asterix
        if(temp == '*'){
            P(full1);
            P(mutex1);

            //check if next char is also asterix
            temp = *ch1;

            V(mutex1);
            V(empty1);

            //if we have two *s in row, replace with #
            if(temp == '*'){
                temp = '#';
            }

            //if we only have 1 asterix, send * to print
            else{
                P(empty2);
                P(mutex2);

                *ch2 = '*';

                V(mutex2);
                V(full2);
            }
        }

        P(empty2);
        P(mutex2);

        //put char is second shared memory
        *ch2 = temp;

        V(mutex2);
        V(full2);
    }
}

/*
    This process reads characters from shared memory ch2 and prints them
    in lines of 25.
*/
void print(){
    int counter = 0;
    int i;
    //printf("in print process\n");
    while(1){
        P(full2);
        P(mutex2);
        //if(*ch2 == '\n') {printf("<EOL>");}
        if(counter == 25){
            printf("\n");
            counter = 0;
        }
        
            printf("%c", *ch2);
            fflush(stdout);
        

        counter++;

        V(mutex2);
        V(empty2);
    }
}

