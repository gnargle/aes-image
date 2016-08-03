# encrypted-message-image
This is a C program that will AES-128 encrypt a string, and embed it into a PNG image file. The key is generated from another image file.

To compile: make all - compiles for both windows and linux
make linux
make windows - do what they say on the tin.

Encryption: ./aes-image "encryption string" e "filename to encrypt into" "key filename"
Decryption ./aes-image d "filename with encrypted message" "key filename"

Three third party libraries are used and included (according to their licenses):

* tiny-AES128-C: https://github.com/kokke/tiny-AES128-C

* Scott's Toolbox image.h and image_write.h: https://github.com/nothings/stb
