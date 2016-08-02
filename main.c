#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "encryption.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define NUM_OF_ARGS 4

int offsetmult = 11;

void encryption_mode(char* filename, int argc, char *argv[]){
    //get input from cmd line into a readable format for the encryption alg
            
    uint8_t input_enc[strlen(argv[1])];
    sprintf(input_enc,"%s",argv[1]);

    int i;
    //work out how many 16 character cycles must be done on the
    //input string

    int iter = find_iterations(input_enc);
    printf("encrypted:\n");

    //encrypt the string using library

    uint8_t enc[strlen(input_enc)];
    encrypt_str(input_enc,enc, iter);

    //print the encrypted string out in hex, for the user to copy

    char* out = (char*)calloc(1,4*16*iter);

    for (i = (uint8_t) 0; i <= (uint8_t) iter; ++i){
        char* temp = (char*) calloc(1,65);
        phex_to_string(enc + i * (uint8_t) 16,temp);
        strncat(out, temp, strlen(temp));
                
    };
    printf("%s\n",out);
    //printf("%c",out[4]); 
    // prints out a single character of out
    // so I can remember what I'm doing
    printf("\n");

    //we now have a hex string. Each character is one hex digit.
    //we need to embed each digit in an RGB cluster. Do we want to
    //do it singularly or two at a time? Maybe try the former first 
    //because it should be easier, see how it looks
            

    // Preliminary image manipulation work
    // loads in an image by filename and outputs it as a png
    int img_x = 0;
    int img_y = 0;
    int img_comp = 0;
    char *data = stbi_load(filename,&img_x,&img_y,&img_comp,0);
    if (img_x != 0){
        printf("image load success\n");
        int offset = rand()%126+2; // +2 so we never get a 0/overwrite the metadata pixel
        data[0] = offset;
        data[1] = iter; 
        // embed the number of iterations in the image. This * 16 should give us 
        // a rough value of how many pixels we need to read. If we encode the nullbyte
        // as well we'll ensure we get an end-of-string, so we don't need to worry about 
        // any data read in after that.

        printf("RGB of offset & length pixel: %i,%i,%i\n", data[0],data[1],data[2]);
        // this for embeds the encoded data in the image, every offset*offsetmult pixels.
        for (i = 1; i <strlen(out)+1;i++){
            // the strlen()+1 ensures we grab the null byte as well
            data[i*offset*offsetmult] = out[i-1];
            //printf("data of %ith pixel after encode: %i\n",i*offset*offsetmult, data[i*offset*offsetmult]);
        }
                
        stbi_write_png("out.png",img_x,img_y,img_comp,data,img_x*img_comp);
    }

}

void decryption_mode(char* filename, int argc, char *argv[]){
    //uint8_t* input_enc = NULL;
    //input_enc = (uint8_t*)calloc(1,strlen(argv[1]));
    //strcpy(input_enc, argv[1]);

    int img_x = 0;
    int img_y = 0;
    int img_comp = 0;
    char *data = stbi_load(filename,&img_x,&img_y,&img_comp,0);
    if (img_x != 0){
        printf("image load success\n");
        printf("RGB of first pixel: %i,%i,%i\n", data[0],data[1],data[2]);
        int offset = data[0];
        int read_iter = data[1];

        char* enc = (char*)calloc(1,4*16*read_iter);
        int i;
        for (i = 1; i <4*16*read_iter+1;i++){ 
            enc[i-1] = data[i*offset*offsetmult];
        }
        printf("enc = %s\n",enc);
        //convert hex string to ascii. I have no idea why this works now
        //when it didn't before 

        uint8_t* asc = (uint8_t*)calloc(1,strlen(enc));
        convert_hex_str_to_asc(enc, asc);
        int iter = find_iterations(enc);
        uint8_t* dec = (uint8_t*)calloc(1,strlen(enc)); 
        decrypt_str(asc,dec,iter);

        printf("decrypted string:\n%s\n",dec);
    }

    
}

int main(int argc, char *argv[]){
    srand(time(NULL));
    if (argc == NUM_OF_ARGS){
        uint8_t mode[1];
        sprintf(mode,"%s",argv[2]);
        char filename[strlen(argv[3])];
        sprintf(filename, "%s", argv[3]);
        if (mode[0] == 'e'){
            encryption_mode(filename, argc, argv);
        }
        else if (mode[0] == 'd'){
            decryption_mode(filename, argc, argv);
        }
        else{
            printf("Could not understand arguments.\nUsage: ./a.out <string> \"d\" <decrypt mode> \"e\" <encrypt mode>\n");
        }
        return 0;
    }
    else if (argc > NUM_OF_ARGS) printf("Too many arguments.\n");
    else printf("Usage: <encryption string>, mode, <filename> \n");
}


/*
    This project wuld have been impossible without two wonderful open-source libraries:
    -Scott's Toolbox (https://github.com/nothings/stb)
    -tiny-AES128-C (https://github.com/kokke/tiny-AES128-C)
*/