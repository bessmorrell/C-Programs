/**
 * This program counts the frequency of words in a text file,
 * reports the number of unique words, and outputs the words 
 * sorted by their frequencies in an output file.
 * March 7, 2021
 * 
 * */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STRING_SIZE 20 /* maximum C-string size */

struct WordFreq {
    char *word;   /* word to store */
    int count;  /* frequency of word */
};

/* function declarations */
int processWords(char filename[], struct WordFreq **list, int *n);
void addWord(struct WordFreq **list_ptr, int *n_ptr, char str[]);
int wordInList(char str[], struct WordFreq **list_ptr, int *n_ptr);
void sortFreqs(struct WordFreq **list_ptr, int *n_ptr);

int main(int argc, char* argv[]){
    int fileOK = 1;
    struct WordFreq *list = NULL; /* list of words and their frequencies */
    int n = 0; /* number of words in list */
    FILE *fptr; /* pointer to output file */
    int i;

    /* when user does not include a file to read */
    if(argc < 2){
        printf("Usage: %s filename \n", argv[0]);
        return 1;
    }

    fileOK = processWords(argv[1], &list, &n);
    /* if file is inaccessable or nonexistent */
    if(!fileOK){
        printf("%s could not open file %s \n", argv[0], argv[1]);
        return 2;
    }

    /*sort words by frequency */
    sortFreqs(&list, &n);
    
    /*print out number of unique words */
    printf("%s has %d unique words\n", argv[1], n);

    /*write list to text file */
    fptr = fopen(argv[2], "w");
    for(i = n-1; i >= 0; i--){
        fprintf(fptr,"%s %d\n",list[i].word, list[i].count);
    }
    fclose(fptr);

    /* free the list */
    if(list) free(list);

    return 0;
}

/* 
This function reads chars and adds them to the c string str
until it encounters a non-letter, meaning the c string is a full word.
It compares this word to the preexisting list of words. If the word is already
on the list, it updates count. If not, it adds the word to the list.
Uses c string filename, pointer to list of WordFreqs, and pointer to list length.
Returns int (1 for true, 0 for false)
*/
int processWords(char filename[], struct WordFreq **list_ptr, int *n_ptr){
    FILE *file_ptr = NULL;
    char str[MAX_STRING_SIZE];
    char ch;
    int a = 0;
    str[0] = '\0'; /* adding null terminator */

    /* error message if file cannot be opened */
    file_ptr = fopen(filename, "re");
    if(file_ptr ==0){
        printf("file %s could not be opened \n", filename);
        return 0;
    }

    ch = fgetc(file_ptr);
    while(ch != EOF){
        /* if we have a find non-letter and str length is greater than 0*/
        if(!isalpha(ch) && a > 0){ 
            /* if word is not in current list of words, wordInList returns 0 */
            if(wordInList(str, list_ptr, n_ptr)==0){
                addWord(list_ptr, n_ptr, str); /* add word to list */
            }
            str[0] = '\0'; /*resetting to read next word*/
            a=0;
        }
        /* if char is letter, add to str */
        else if(isalpha(ch)){
            str[a] = tolower(ch);
            a++;
            str[a] = '\0'; /*move null terminator to end*/
        }
        ch = fgetc(file_ptr);
    }
    return 1;
}

/* 
This function adds a word to the current list of words and their frequencies.
Uses pointer to list of WordFreqs, pointer to list lenght, and c string of word to add.
Returns void.
*/
void addWord(struct WordFreq **list_ptr, int *n_ptr, char str[]){
    char *word = {str};          /* word we are adding */
    struct WordFreq *list = *list_ptr;      /* local list */
    int n = *n_ptr;                         /* words in the local list */
    struct WordFreq *temp_list = NULL; /* temp list for copying */
    int j;

    /* add 1 word to our list by allocating a new list */
        /* create a temp list that is can store one more element */
        temp_list = (struct WordFreq *)malloc((n + 1) * sizeof(struct WordFreq));
        /* copy the old list to temp_list */
        for(j = 0; j < n; j++) {
            temp_list[j] = list[j];
        }

        /* add a new word to the end of the new list */
        /* make space to store the new word */
        temp_list[j].word = (char *) malloc(strlen(word) + 1);
        /* copy the word over */
        strcpy(temp_list[j].word, word);
        /* update count */
        temp_list[j].count =1;
        /* free the old list (no memory leaks) */
        if(list) free(list);
        /* point to the new, larger list */
        list = temp_list;
        /* added one more word, increase the count of list */
        n++;

    /* save the pointers in order to "pass back" the list*/
    *list_ptr = list;
    *n_ptr = n;
}

/* 
This function compares a word to the current list of words. If the word is on
the list, its count is increased by one.
Uses c string word, pointer to list of WordFreqs, and pointer to list length.
Returns int (1 for true, 0 for false)
*/
int wordInList(char str[], struct WordFreq **list_ptr, int *n_ptr){
    char *word = {str}; /* word to add */
    struct WordFreq *list = *list_ptr; /*local list */
    int n = *n_ptr; /*words in local list */
    int i;
    /* loop through all words on current list and compare */
    for(i = 0; i < n; i++){
        if(strcmp(word, list[i].word)==0){
            list[i].count++; /*increase count*/
            return 1;
        }
    }
    /*save the pointer */
    *list_ptr = list;
    return 0;
}

/*
This function sorts the list of WordFreqs by count from smallest to largest.
Uses pointer to list of WordFreqs and pointer to length of list.
Returns void
*/
void sortFreqs(struct WordFreq **list_ptr, int *n_ptr){
    struct WordFreq *list = *list_ptr; /*local list */
    int n = *n_ptr; /*words in local list */
    struct WordFreq temp; /*temp WordFreq for copying */
    int i, j;

    /* bubble sort */
    for(i = 0; i < n; i++){
        for(j = i + 1; j < n; j++){
            if(list[i].count > list[j].count){
                temp = list[i];
                list[i] = list[j];
                list[j] = temp;
            }
        }
    }
    /*save pointer */
    *list_ptr = list;
}
