# encrypted-message-image
This is a C program that will AES-128 encrypt a string, and embed it into a PNG image file. The key is currently fixed, next on my agenda is generating a key and embedding it in a separate file.

To compile: make

To encrypt: ./aes-image \<string to encrypt\> e \<image filename\> 

To decrypt: ./aes-image d \<image filename\>
