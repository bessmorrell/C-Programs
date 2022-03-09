/**
 * This program reads a WAV audio file and hides a secret message from a text file in the LSBs. 
 * The file names is provided using command line arguments. If the file name is not provided or 
 * the file is not readable, the program will exit and provide an error message.
 *
 * @author Bess Morrell {@literal <pluf@wfu.edu>}
 * @date Apr. 5, 2021
 * @assignment Lab 6
 * @course CSC 250
 **/

#include "get_wav_args.h"
#include <stdio.h>
#include <stdlib.h>  
#include <string.h>  
#include <math.h>  

int read_wav_header(FILE* in_file, FILE* out_file, short *sample_size_ptr, int *num_samples_ptr, int *sample_rate_ptr, int* bit);
int read_wav_data(FILE* in_file, FILE* out_file, short bits_per_sample, int num_samples, int* bit, char text_file_name[], char new_wav_file_name[]);


int main(int argc, char *argv[]) {
    FILE *in_file;      /* pointer to WAV file */
    FILE *out_file;     /* pointer to output file */
    short sample_size;  /* size of an audio sample (bits) */
    int sample_rate;    /* sample rate (samples/second) */
    int num_samples;    /* number of audio samples */ 
    int wav_ok = 0;     /* 1 if the WAV file si ok, 0 otherwise */ 
    int bit;            /* num of LSBs for decoding */
    char wav_file_name[256];    /* c-string of wav file name */
    char text_file_name[256];   /* c-string of text file name */
    char new_wav_file_name[256]; /*name of output file with hidden message */
    int args_ok;        /* bool for if command line arguments are correct */ 

    /* contains error messages for file reading */
    args_ok = get_wav_args(argc, argv, &bit, wav_file_name, text_file_name);

    /* if argument requirements are not met */
    if(!args_ok){
        return 2;
    }

    /* error check to make sure wav file is actually a .wav file */
    if(strcmp((wav_file_name + strlen(wav_file_name) - 4), ".wav")){
        printf("wav file %s missing \".wav\" \n", wav_file_name);
        return 0;
    }

    /* add _msg to output file name */
    strcpy(new_wav_file_name, wav_file_name);
    strcpy((new_wav_file_name + strlen(new_wav_file_name) - 4), "_msg.wav");

    in_file = fopen(wav_file_name, "rbe"); /* open wav file for reading */
    /*error check if file cannot be opened */
    if(!in_file) {
        printf("could not open wav file %s \n", argv[1]);
        return 2;
    }
    
    out_file = fopen(new_wav_file_name, "wbe"); /*open output file for writing */
    if(!out_file) {
        printf("could not create wav file %s \n", new_wav_file_name);
    }

    wav_ok = read_wav_header(in_file, out_file, &sample_size, &num_samples, &sample_rate, &bit);
    /*error check for incompatible wav file */
    if(!wav_ok) {
       printf("wav file %s has incompatible format \n", wav_file_name);   
       return 3;
    }
    else{
        read_wav_data(in_file, out_file, sample_size, num_samples, &bit, text_file_name, new_wav_file_name);
    }

    /* close file pointers */
    if(in_file) fclose(in_file);
    if(out_file) fclose(out_file);
    return 0;
}


/**
 *  This function reads the RIFF, fmt, and start of the data chunk and copies to the output file. 
 *  Uses in_file ptr, out_file ptr, sample size pointer, num samples pointer, sample rate pointer, num LSBs
 *  Returns int (1 for true)
 */
int read_wav_header(FILE* in_file, FILE* out_file, short *sample_size_ptr, int *num_samples_ptr, int *sample_rate_ptr, int* bit) {
    char chunk_id[] = "    ";  /* chunk id, note initialize as a C-string */
    char data[] = "    ";      /* chunk data */
    int chunk_size = 0;        /* number of bytes remaining in chunk */
    short audio_format = 0;    /* audio format type, PCM = 1 */
    short num_channels = 0;    /* number of audio channels */ 
    int sample_rate = 0;       /* audio samples per second */
    short bits_per_sample = 0; /* number of bits per sample */
    int num_samples = 0;       /* number of samples of the audio */
    int i;


    /* first chunk is the RIFF chunk, let's read and copy that info */  
    fread(chunk_id, 4, 1, in_file);
    fwrite(chunk_id, 4, 1, out_file);
    fread(&chunk_size, 4, 1, in_file);
    fwrite(&chunk_size, 4, 1, out_file);
    fread(data, 4, 1, in_file);
    fwrite(data, 4, 1, out_file);

    /* let's try to read the next chunk, it always starts with an id */
    fread(chunk_id, 1, 4, in_file);
    /* if the next chunk is not "fmt " then let's skip over it */  
    while(strcmp(chunk_id, "fmt ") != 0) {
        fread(&chunk_size, 1, 4, in_file);
        /* skip to the end of this chunk */  
        fseek(in_file, chunk_size, SEEK_CUR);
        /* read the id of the next chuck */  
        fread(chunk_id, 1, 4, in_file);
    }  
    fwrite(chunk_id, 1, 4, out_file);

    /* if we are here, then we must have the fmt chunk, now read and copy that data */  
    fread(&chunk_size, 1, 4, in_file);
    fwrite(&chunk_size, 1, 4, out_file);
    fread(&audio_format, 1,  sizeof(audio_format), in_file);
    fwrite(&audio_format, 1,  sizeof(audio_format), out_file);
    fread(&num_channels, 1,  sizeof(num_channels), in_file);
    fwrite(&num_channels, 1,  sizeof(num_channels), out_file);
    fread(&sample_rate, 1,  sizeof(sample_rate), in_file);
    fwrite(&sample_rate, 1,  sizeof(sample_rate), out_file);
    
    for(i = 0; i < 4; i++){
        fread(&bits_per_sample, 1,  sizeof(bits_per_sample), in_file); /* skip to num bits per sample */  
        fwrite(&bits_per_sample, 1,  sizeof(bits_per_sample), out_file);
    }

    /* id of next chunk */
    fread(chunk_id, 4, 1, in_file);

    /* read the data chunk next */
    while(strcmp(chunk_id, "data") != 0) {
        fread(&chunk_size, 1, 4, in_file);
        /* skip to the end of this chunk */  
        fseek(in_file, chunk_size, SEEK_CUR);
        /* read the id of the next chuck */  
        fread(chunk_id, 1, 4, in_file);
    }

    fwrite(chunk_id, 4, 1, out_file);

    /* if we are here, then we must have the data chunk, now read and copy that data */ 
    fread(&num_samples, 4, 1, in_file); /* bytes of data */
    fwrite(&num_samples, 4, 1, out_file);
    num_samples = num_samples / (bits_per_sample / 8); /* divide total bytes of data by bytes per sample to find num of samples */
    if(num_channels == 2) /* if there are two channels we divide the bytes in half again */
        num_samples = num_samples / 2;

    
    /* save pointers */
    *sample_size_ptr = bits_per_sample;
    *num_samples_ptr = num_samples;

    return (audio_format == 1);
}


/**
 *  This function reads the WAV audio samples and encrypts the LSBs in order to write the secret message to the output file
 *  Uses parameters: in_file pointer, bits per sample, num samples, num of LSBs, and output filename
 *  returns int (1 for true)
 */
int read_wav_data(FILE* in_file, FILE* out_file, short bits_per_sample, int num_samples, int* bit, char text_file_name[], char new_wav_file_name[]) {
    FILE *text_file;    /* pointer to text file */
    short bytes_per_sample = bits_per_sample / 8; /* divide bits per sample by 8 to find bytes per sample */
    int i = 0;  /* keeps track of num of samples decrypted */
    int j;  /* used in for loop */
    int sample; /* sample defined as int in order to hold any number */
    unsigned char c = 0;   /* holds character of the message to encrypt */
    unsigned int mask; /* to weed out all but LSBs */
    unsigned int char_mask; /* to isolate certain sections of a char */
    int num_chars = 0;  /*number of chars encrypted */
    unsigned char temp_char = 0; /* temp char for adding to sample */
    int temp_sample = 0; /* temp sample that has altered LSBs */
    char eyes = ':'; /* for adding smiley face */
    char smile = ')'; /* for adding smiley face */

    text_file = fopen(text_file_name, "a"); /*open text file for appending*/
    fwrite(&eyes, 1, sizeof(char), text_file); /* add smiley face to end of secret message */
    fwrite(&smile, 1, sizeof(char), text_file);
    if(text_file) fclose(text_file); /* close text file */

    text_file = fopen(text_file_name, "r"); /*open text file for reading*/
    
    /* assign mask value */
    if(*bit==1){
        mask = 0xFE; /* 11111110 */
    }
    else if(*bit==2){
        mask = 0xFC; /* 11111100 */
    }
    else if(*bit==4){
        mask = 0xF0; /* 11110000 */
    }

    char_mask = mask ^ 0xFF; /* XOR mask with all 1s to get char mask (mirror opposite)*/

    while(i < num_samples){

       if(fread(&c, 1, 1, text_file) == 0){ /* read character of text file */
           break; /* break if no character read (EOF) */
       }
        
        for(j = 1; j <= (8 / *bit); j++){ /* hide LSBs in samples until character is complete*/
            fread(&sample, 1, bytes_per_sample, in_file);

            if (bytes_per_sample == 2) sample = (short) sample; /* 2 byte sample are cast to short */

            temp_char = c & (char_mask << (8 - (*bit * j))); /* take LSBs from char */
            temp_char = temp_char >> (8 - (*bit * j)); /* shift sample to proper LSB place */

            temp_sample = (temp_sample & mask); /* empty the LSB places in wav sample */
            temp_sample = temp_sample | temp_char; /* OR with new LSBs */

            fwrite(&temp_sample, 1, bytes_per_sample, out_file); /* write to output file */

            i++; /* increase count */
        }

        num_chars++; /* increase count */
    } 

    if(text_file) fclose(text_file); /* close text file */

    while(i < num_samples){ /* copy the rest of the samples in wav file */
        fread(&sample, 1, bytes_per_sample, in_file);
        fwrite(&sample, 1, bytes_per_sample, out_file);
        i++;
    }

    /* print num of chars recovered from num of samples */
    printf("%d characters written to %s\n", num_chars, new_wav_file_name);

   return 1;
}