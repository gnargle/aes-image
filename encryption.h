#ifndef _ENCRYPTION_H
#define _ENCRYPTION_H

void phex(uint8_t* str);

void encrypt_str(uint8_t* str, uint8_t* out, int iter);

void decrypt_str(uint8_t* str, uint8_t* out, int iter);

int findIterations(uint8_t* str);

#endif //_ENCRYPTION_H