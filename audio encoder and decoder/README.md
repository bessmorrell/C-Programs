The audio encoder reads a WAV audio file and hides a secret message from a text file in the least significant bits (LSBs). 
The file names is provided using command line arguments. If the file names are not provided or 
the file is not readable, the program will exit and provide an error message.

The audio decoder reads a WAV file and decrypts the message hidden in the LSBs.
The audio file name in provided using command line arguments. If the file name is not provided or 
the file is not readable, the program will exit and provide an error message.

Both programs require get_wav_args.c and get_wav_args.h
