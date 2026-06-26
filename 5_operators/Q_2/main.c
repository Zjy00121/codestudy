#include <stdio.h>

int main () {
    int ch;
    while ((ch = getchar()) != EOF) {
        if (ch >= 'A' && ch <= 'Z') {
            ch += 32;
            if (ch <= 'm') ch +=13;
            else ch -=13;
            //ABCDEFGHIJKLM NOPQRSTUVWXYZ
        }
        else if (ch >= 'a' && ch <= 'z') {
            ch -=32;
            if (ch <= 'M') ch +=13;
            else ch -=13;
        }
        putchar(ch);
    }
    return 0;
}