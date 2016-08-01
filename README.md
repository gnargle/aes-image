# encrypted-message-image
This is a C program that will AES-128 encode a string, and encode it into a PNG image file (when it's done).

To compile: gcc main.c aes.c encryption.c -lm

To encrypt: ./a.out <string to encrypt> e <image filename> 

To decrypt: ./a.out <string to decrypt> d <image filename>
