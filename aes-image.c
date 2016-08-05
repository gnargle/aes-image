#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include "aes-image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//uint8_t key[4] = {0xef,0x45,0x3f,0xa6};

int offsetmult = 11;
int keylength = 16;

void phex(uint8_t* str)
{
    unsigned char i;
    for(i = 0; i < 16; ++i)
        printf("%.2x", str[i]);
    //printf("\n");
}

void phex_to_string(uint8_t* str, char* out)
{
    unsigned char i;
    char fullstring[65];
    for(i = 0; i < 16; ++i){
        char temp[4] = "";
        snprintf(temp,4,"%.2x", str[i]);
        if (i==0){
            strncpy(fullstring,temp,4);
        }
        else{
            strncat(fullstring,temp,4);
        } // this fixes the weird extra character issue
    }
    strncpy(out,fullstring,sizeof(fullstring));
}

void encrypt_str(uint8_t* str, uint8_t* out, int iter, uint8_t* key){
    uint8_t i;
    for (i = (uint8_t) 0; i <= (uint8_t) iter; ++i){
        AES128_ECB_encrypt(str + (i*16),key,out + (i*16));
    }
}

void decrypt_str(uint8_t* str, uint8_t* out, int iter, uint8_t* key){
    uint8_t i;
    for (i = (uint8_t) 0; i <= (uint8_t) iter; ++i){
        AES128_ECB_decrypt(str + (i*16),key,out + (i*16)); 
    }
}

int find_iterations(uint8_t* str){
    int iter;
    if (strlen(str) > 16){
        iter = (strlen(str)+16 -1) /16 ;
    }
    else{
        iter = 1;
    }
    return iter;
}

int hex_to_int(char c){
    if(c >=97) c=c-32;
    int first = c / 16 - 3;
    int second = c % 16;
    int result = first*10 + second;
    if(result > 9) result--;
    return result;
}

int hex_to_ascii(char c, char d){
        int high = hex_to_int(c) * 16;
        int low = hex_to_int(d);
        return high+low;
}

void convert_hex_str_to_asc(char* str, char* done){
	int i;
	int u = 0;
	char buf = 0;
	for (i=0;i<strlen(str);i++){
		if(i % 2 != 0){
            done[u] = hex_to_ascii(buf, str[i]);
            //printf("%s\n", done);
            u++;
        }else{
            buf = str[i];
        }
	}
}

void encryption_mode(char* filename, char* filename_key, uint8_t* input_enc){
    //first get the key from the key image.
    uint8_t* key = (uint8_t*) calloc(1,16);
    keygen(filename_key, key, keylength);

    int i;
    //work out how many 16 character cycles must be done on the
    //input string

    int iter = find_iterations(input_enc);

    //encrypt the string using library

    uint8_t enc[strlen(input_enc)];
    encrypt_str(input_enc,enc, iter, key);

    char* out = (char*)calloc(1,4*16*iter);

    for (i = (uint8_t) 0; i <= (uint8_t) iter; ++i){
        char* temp = (char*) calloc(1,65);
        phex_to_string(enc + i * (uint8_t) 16,temp);
        strncat(out, temp, strlen(temp));
        //free(temp);   // for now this is commented as it crashes the program on windows  
    };
            

    // Image manipulation work
    // loads in an image by filename and outputs it as a png
    int img_x = 0;
    int img_y = 0;
    int img_comp = 0;
    char *data = stbi_load(filename,&img_x,&img_y,&img_comp,0);
    if (img_x != 0){
        printf("image load success\n");
        int offset = rand()%126+2; // +2 so we never get a 0/overwrite the metadata pixel
        //RGB is signed so we use %126+2 to get between 1 and 127 
        //any greater than that would turn negative, which would break the offsetting

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

void decryption_mode(char* filename, char* filename_key){

    uint8_t* key = (uint8_t*) calloc(1,16);
    keygen(filename_key, key, keylength);

    int img_x = 0;
    int img_y = 0;
    int img_comp = 0;
    char *data = stbi_load(filename,&img_x,&img_y,&img_comp,0);
    if (img_x != 0){
        printf("image load success\n");
        int offset = data[0];
        int read_iter = data[1];

        char* enc = (char*)calloc(1,((img_x * img_y)/11)+1); 
        // should hopefully provide a large enough buffer to read in all the data
        // without causing a memory crash
        int i;
        for (i = 1; i <4*16*read_iter+1;i++){ 
            enc[i-1] = data[i*offset*offsetmult];
        }
        //convert hex string to ascii. I have no idea why this works now
        //when it didn't before 

        //*2 in following callocs necessary to avoid windows crash.
        //Is it better to do this than spending 8 hours finding the memory issue?
        //Probably, I'm not made of time.
        uint8_t* asc = (uint8_t*)calloc(1,2*strlen(enc));
        convert_hex_str_to_asc(enc, asc);
        int iter = find_iterations(enc);
        uint8_t* dec = (uint8_t*)calloc(1,2*strlen(enc)); 
        decrypt_str(asc,dec,iter, key);

        printf("decrypted string:\n%s\n",dec);
        free(enc);
        free(asc);
        free(dec);
    }
    else{
        printf("image load failed, aborting...\n");
    }
    free(data);    
}

void keygen(char* filename, char* key, int keylength){
    /*
    just thinking about key generation - what if we used the first x RGB values from an image? 
    That way when you send it, it's not suspicious because the image is 
    *exactly the same* as another downloaded off the internet or whatever. 
    Only thinking is, if you send an image via facebook and it gets downloaded again, will that
    corrupt the key due to JPG degradation? Maybe we need to ensure that it's a png?
    We'll see.
    */
    int img_x = 0;
    int img_y = 0;
    int img_comp = 0;
    char *data = stbi_load(filename,&img_x,&img_y,&img_comp,0);
    if (img_x != 0){
        int i;
        for (i = 1; i < keylength+1;i++){ 
            key[i-1] = (uint8_t)data[i];
        }
        free(data);
    }
    else{
        printf("failed to load key image, aborting\n");
        free(data);
        exit(EXIT_FAILURE);
    }
}
