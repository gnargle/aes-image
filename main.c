#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "aes-image.h"

#define NUM_OF_ARGS_enc 5
#define NUM_OF_ARGS_dec 4

int main(int argc, char *argv[]){
    srand(time(NULL));
    if (argc == NUM_OF_ARGS_enc){
        uint8_t mode[1];
        sprintf(mode,"%s",argv[1]);
        char filename_img[strlen(argv[3])];
        sprintf(filename_img, "%s", argv[3]);
        char filename_key[strlen(argv[4])];
        sprintf(filename_key, "%s", argv[4]);
        //get input from cmd line into a readable format for the encryption alg
            
        uint8_t input_enc[strlen(argv[2])];
        sprintf(input_enc,"%s",argv[2]);
        if (mode[0] == 'e'){
            int c = encryption_mode(filename_img, filename_key,input_enc);
            if (c==1) {printf("Encryption failed somewhere.\n");}
        }
    }
    else if (argc == NUM_OF_ARGS_dec){
        uint8_t mode[1];
        sprintf(mode,"%s",argv[1]);
        char filename_img[strlen(argv[2])];
        sprintf(filename_img, "%s", argv[2]);
        char filename_key[strlen(argv[3])];
        sprintf(filename_key, "%s", argv[3]);
        if (mode[0] == 'd'){
            int c = decryption_mode(filename_img, filename_key);
            if (c == 1) {printf("Decryption failed somewhere.\n");}
        }
    }
    else if (argc > NUM_OF_ARGS_enc) printf("Too many arguments.\n");
    else {
        printf("Usage: Encrypt mode: e, <encryption string>, <filename to encode in>, <key filename> \n");
        printf("Decrypt mode: d, <filename with message in>, <key filename>\n");
    }
    return 0;
}



/*
    This project would have been impossible without two wonderful open-source libraries:
    -Scott's Toolbox (https://github.com/nothings/stb)
    -tiny-AES128-C (https://github.com/kokke/tiny-AES128-C)
*/
