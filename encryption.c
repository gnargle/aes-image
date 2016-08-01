#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "aes.h"
#include "encryption.h"

const uint8_t key[4] = {0xef,0x45,0x3f,0xa6};

void phex(uint8_t* str)
{
    unsigned char i;
    for(i = 0; i < 16; ++i)
        printf("%.2x", str[i]);
    //printf("\n");
}

void encrypt_str(uint8_t* str, uint8_t* out, int iter){
    uint8_t i;
    for (i = (uint8_t) 0; i <= (uint8_t) iter; ++i){
        AES128_ECB_encrypt(str + (i*16),key,out + (i*16));
    }
}

void decrypt_str(uint8_t* str, uint8_t* out, int iter){
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