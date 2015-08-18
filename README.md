Jogo da Forca
==============

Compile instructions
--------------

This software uses PDCurses, so the compiler should have it installed and link it

Also, the compiler directive should be C99
The command to compile is the following:
	gcc jogodaforca.c -o jogodaforca.exe -lpdcursesw -std=c99

This software was developed and tested on Windows 10 x64, compiling to other platforms may need some code edition.

Wordlists are read from files on wordlists dir, that should have the extension .wordlist.tdb, with one word on each line. Also, the files should have a blank line on the end, and the encoding should be Windows 1252.