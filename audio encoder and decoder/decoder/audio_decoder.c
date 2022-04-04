/**
 * This program reads a WAV audio file and prints the decrypted message hidden in the LSBs. The file name 
 * is provided using command line arguments. If the file name is not provided or the file is not readable, 
 * the program will exit and provide an error message.
 * @date Mar. 29, 2021
 **/

#include "get_wav_args.h"
#include <stdio.h>
#include <stdlib.h>  
#include <string.h>  
#include <math.h>  

int read_wav_header(FILE* in_file, short *sample_size_ptr, int *num_samples_ptr, int *sample_rate_ptr, int* bit, char text_file_name[]);
int read_wav_data(FILE* in_file, short sample_size, int num_samples, int* bit, char text_file_name[]);


int main(int argc, char *argv[]) {
    FILE *in_file;      /* pointer to WAV file */
    short sample_size;  /* size of an audio sample (bits) */
    int sample_rate;    /* sample rate (samples/second) */
    int num_samples;    /* number of audio samples */ 
    int wav_ok = 0;     /* 1 if the WAV file si ok, 0 otherwise */ 
    int bit;            /* num of LSBs for decoding */
    char wav_file_name[256];    /* c-string of wav file name */
    char text_file_name[256];   /* c-string of output file name */
    int args_ok;        /* bool for if command line arguments are correct */ 

    /* contains error messages for file reading */
    args_ok = get_wav_args(argc, argv, &bit, wav_file_name, text_file_name);

    /* if argument requirements are not met */
    if(!args_ok){
        return 2;
    }

    in_file = fopen(wav_file_name, "rbe"); 
    /*error check if file cannot be opened */
    if(!in_file) {
        printf("could not open wav file %s \n", argv[1]);
        return 2;
    }
  
    wav_ok = read_wav_header(in_file, &sample_size, &num_samples, &sample_rate, &bit, text_file_name);
    /*error check for incompatible wav file */
    if(!wav_ok) {
       printf("wav file %s has incompatible format \n", wav_file_name);   
       return 3;
    }

    if(in_file) fclose(in_file);
    return 0;
}


/**
 *  This function reads the RIFF, fmt, and start of the data chunk. 
 *  Uses in_file ptr, sample size pointer, num samples pointer, sample rate pointer, num LSBs, and the output filename
 *  Returns int (1 for true)
 */
int read_wav_header(FILE* in_file, short *sample_size_ptr, int *num_samples_ptr, int *sample_rate_ptr, int* bit, char text_file_name[]) {
    char chunk_id[] = "    ";  /* chunk id, note initialize as a C-string */
    char data[] = "    ";      /* chunk data */
    int chunk_size = 0;        /* number of bytes remaining in chunk */
    short audio_format = 0;    /* audio format type, PCM = 1 */
    short num_channels = 0;    /* number of audio channels */ 
    int sample_rate = 0;       /* audio samples per second */
    short bits_per_sample = 0; /* number of bits per sample */
    int num_samples = 0;       /* number of samples of the audio */
    int i;


    /* first chunk is the RIFF chunk, let's read that info */  
    fread(chunk_id, 4, 1, in_file);
    fread(&chunk_size, 4, 1, in_file);
    fread(data, 4, 1, in_file);

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

    /* if we are here, then we must have the fmt chunk, now read that data */  
    fread(&chunk_size, 1, 4, in_file);
    fread(&audio_format, 1,  sizeof(audio_format), in_file);
    fread(&num_channels, 1,  sizeof(num_channels), in_file);
    fread(&sample_rate, 1,  sizeof(sample_rate), in_file);
    for(i = 0; i < 4; i++)
        fread(&bits_per_sample, 1,  sizeof(bits_per_sample), in_file); /* skip to num bits per sample */  

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

    /* if we are here, then we must have the data chunk, now read that data */ 
    fread(&num_samples, 4, 1, in_file); /* bytes of data */
    num_samples = num_samples / (bits_per_sample / 8); /* divide total bytes of data by bytes per sample to find num of samples */
    if(num_channels == 2) /* if there are two channels we divide the bytes in half again */
        num_samples = num_samples / 2;

    /*read wav data by sending to read_wav_data function */
    read_wav_data(in_file, bits_per_sample, num_samples, bit, text_file_name);

    return (audio_format == 1);
}


/**
 *  This function reads the WAV audio samples and decrypts the LSBs in order to print the secret message to the output file
 *  Uses parameters: in_file pointer, bits per sample, num samples, num of LSBs, and output filename
 *  returns int (1 for true)
 */
int read_wav_data(FILE* in_file, short bits_per_sample, int num_samples, int* bit, char text_file_name[]) {
    short bytes_per_sample = bits_per_sample / 8; /* divide bits per sample by 8 to find bytes per sample */
    int i = 0;  /* keeps track of num of samples decrypted */
    int j;  /* used in for loop */
    int sample; /* sample defined as int in order to hold any number */
    unsigned char ch = 0;   /* holds decrypted char */
    unsigned int mask; /* to weed out all but LSBs */
    short shift = 8 - *bit; /* num of places to shift left */
    char message[num_samples]; /* c-string of decrypted message */
    int num_chars = 0;  /*number of chars decrypted */
    FILE *fptr;         /* pointer to output file */

    /* assign mask value */
    if(*bit==1){
        mask = 0x1; /* 00000001 */
    }
    else if(*bit==2){
        mask = 0x3; /* 00000011 */
    }
    else if(*bit==4){
        mask = 0xF; /* 00001111 */
    }

    while(i < num_samples){
        /*check to see if it's the end of the message (look for :) )*/
        if(message[num_chars - 2] == ':' && message[num_chars-1] == ')'){
            break;
        }

       fread(&sample, 1, bytes_per_sample, in_file); /* read in sample */

        if (bytes_per_sample == 2){ /* 2 byte samples are cast to short type */
            sample = (short) sample;
        }

        ch = (sample & mask) << shift; /* read section of char by masking and shifting*/
        
        for(j = 1; j < (8 / *bit); j++){ /* read additional samples into char until it is complete*/
            fread(&sample, 1, bytes_per_sample, in_file);

            if (bytes_per_sample == 2) sample = (short) sample;

            ch = ch|((sample & mask) << (shift - j * (*bit))); /* add to ch by masking, shifting, and ORing */

            i++;
        }

        message[num_chars] = ch; /* add ch to message */
        message[num_chars+1] = '\0';

        num_chars++; /* increase count */
    
       i++;
    } 

    /* print num of chars recovered from num of samples */
    printf("%d characters recovered from %d samples\n", num_chars, i);

    /* print decrypted message to output file */
    fptr = fopen(text_file_name, "w");
    for(i = 0; i < num_chars; i++){
        fprintf(fptr,"%c",message[i]);
    }
    fclose(fptr);

   return 1;
}
