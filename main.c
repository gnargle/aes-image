#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "encryption.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define NUM_OF_ARGS 4

int main(int argc, char *argv[]){
    if (argc == NUM_OF_ARGS){
        uint8_t mode[1];
        sprintf(mode,"%s",argv[2]);
        char filename[strlen(argv[3])];
        sprintf(filename, "%s", argv[3]);
        if (mode[0] == 'e'){

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

            for (i = (uint8_t) 0; i <= (uint8_t) iter; ++i){
                phex(enc + i * (uint8_t) 16);
            }
            printf("\n");

            // Preliminary image manipulation work
            // loads in an image by filename and outputs it as a png
            // TODO: embed the encoded hex characters
            int img_x = 0;
            int img_y = 0;
            int img_comp = 0;
            char *data = stbi_load(filename,&img_x,&img_y,&img_comp,0);
            if (img_x != 0){
                printf("image load success\n");
                printf("image width = %i\n",img_x);
                printf("image_height = %i\n",img_y);
                printf("image components = %i\n",img_comp);
                printf("RGB of first pixel: %i,%i,%i\n", data[0],data[1],data[2]);
            }

            stbi_write_png("out.png",img_x,img_y,img_comp,data,img_x*img_comp);
        }

        else if (mode[0] == 'd'){
            uint8_t* input_enc = NULL;
            input_enc = (uint8_t*)calloc(1,strlen(argv[1]));
            strcpy(input_enc, argv[1]);

            //convert hex string to ascii. I have no idea why this works now
            //when it didn't before 

            uint8_t* asc = (uint8_t*)calloc(1,strlen(input_enc));
            convert_hex_str_to_asc(input_enc, asc);
            int iter = find_iterations(input_enc);
            uint8_t* dec = (uint8_t*)calloc(1,strlen(input_enc)); 
            decrypt_str(asc,dec,iter);

            printf("decrypted string:\n%s\n",dec);
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