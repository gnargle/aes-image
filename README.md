# encrypted-message-image
This is a C library that will AES-128 encrypt a string, and embed it into a PNG image file. The key is generated from another image file.
Included is an example program that simply uses the api as above.

# API usage:
```C
#include "aes-image.h"
encryption_mode(filename_to_save_to, filename_of_key_image, string_to_encode (in uint8_t format preferably));
decryption_mode(filename_to_read_from, filename_of_key_image);
```
You should also generate a random seed so the pixel offset is different each time.

# Program usage:


To compile test program:
make all - compiles for both windows and linux

make linux

make windows - do what they say on the tin.

Compiling for windows will fail if you do not have mingw for 32 bit windows installed.

* Encryption: ./aes-image "encryption string" e "filename to encrypt into" "key filename"
* Decryption ./aes-image d "filename with encrypted message" "key filename"

Three third party libraries are used and included (according to their licenses):

* tiny-AES128-C: https://github.com/kokke/tiny-AES128-C
* Scott's Toolbox image.h and image_write.h: https://github.com/nothings/stb
