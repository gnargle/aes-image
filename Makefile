all:
	gcc main.c aes.c aes-image.c -o aes-image -lm
	/opt/mingw32/bin/i686-w64-mingw32-gcc main.c aes.c aes-image.c -o aes-image.exe -lm
linux:
	gcc main.c aes.c aes-image.c -o aes-image -lm
windows:
	/opt/mingw32/bin/i686-w64-mingw32-gcc main.c aes.c aes-image.c -o aes-image.exe -lm
