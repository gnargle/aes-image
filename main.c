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

#define NUM_OF_ARGS_enc 4
#define NUM_OF_ARGS_dec 3

void encryption_mode(char* filename, int argc, char *argv[]);

void decryption_mode(char* filename, int argc, char *argv[]);

int offsetmult = 11;

int main(int argc, char *argv[]){
    srand(time(NULL));
    if (argc == NUM_OF_ARGS_enc){
        uint8_t mode[1];
        sprintf(mode,"%s",argv[2]);
        char filename[strlen(argv[3])];
        sprintf(filename, "%s", argv[3]);
        if (mode[0] == 'e'){
            encryption_mode(filename, argc, argv);
        }
    }
    else if (argc == NUM_OF_ARGS_dec){
        uint8_t mode[1];
        sprintf(mode,"%s",argv[1]);
        char filename[strlen(argv[2])];
        sprintf(filename, "%s", argv[2]);
        if (mode[0] == 'd'){
            decryption_mode(filename, argc, argv);
        }
    }
    else if (argc > NUM_OF_ARGS_enc) printf("Too many arguments.\n");
    else {
        printf("Usage: Encrypt mode: <encryption string>, e, <filename> \n");
        printf("Decrypt mode: d, <filename>\n");
    }
    return 0;
}

void encryption_mode(char* filename, int argc, char *argv[]){
    //get input from cmd line into a readable format for the encryption alg
            
    uint8_t input_enc[strlen(argv[1])];
    sprintf(input_enc,"%s",argv[1]);

    int i;
    //work out how many 16 character cycles must be done on the
    //input string

    int iter = find_iterations(input_enc);

    //encrypt the string using library

    uint8_t enc[strlen(input_enc)];
    encrypt_str(input_enc,enc, iter);

    char* out = (char*)calloc(1,4*16*iter);

    for (i = (uint8_t) 0; i <= (uint8_t) iter; ++i){
        char* temp = (char*) calloc(1,65);
        phex_to_string(enc + i * (uint8_t) 16,temp);
        strncat(out, temp, strlen(temp));
        //free(temp);     
    };
            

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

        // this for embeds the encoded data in the image, every offset*offsetmult pixels.
        for (i = 1; i <strlen(out)+1;i++){
            // the strlen()+1 ensures we grab the null byte as well
            data[i*offset*offsetmult] = out[i-1];
        }
                
        stbi_write_png("out.png",img_x,img_y,img_comp,data,img_x*img_comp);
        printf("Image conversion success. Stored as out.png\n");
    }
    else{
        printf("image load failed, aborting...\n");
    }
    free(data);
    free(out);

}

void decryption_mode(char* filename, int argc, char *argv[]){

    int img_x = 0;
    int img_y = 0;
    int img_comp = 0;
    char *data = stbi_load(filename,&img_x,&img_y,&img_comp,0);
    if (img_x != 0){
        printf("image load success\n");
        int offset = data[0];
        int read_iter = data[1];

        char* enc = (char*)calloc(1,4*16*read_iter);
        int i;
        for (i = 1; i <4*16*read_iter+1;i++){ 
            enc[i-1] = data[i*offset*offsetmult];
        }
        //convert hex string to ascii. I have no idea why this works now
        //when it didn't before 

        uint8_t* asc = (uint8_t*)calloc(1,strlen(enc));
        convert_hex_str_to_asc(enc, asc);
        int iter = find_iterations(enc);
        uint8_t* dec = (uint8_t*)calloc(1,strlen(enc)); 
        decrypt_str(asc,dec,iter);

        printf("decrypted string:\n%s\n",dec);
        free(enc);
        //free(asc);
        //free(dec);
    }
    else{
        printf("image load failed, aborting...\n");
    }
    free(data);

    
}

/*
    This project wuld have been impossible without two wonderful open-source libraries:
    -Scott's Toolbox (https://github.com/nothings/stb)
    -tiny-AES128-C (https://github.com/kokke/tiny-AES128-C)
*/