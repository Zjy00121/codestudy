#include <stdio.h>

int main () {
    int ch;
    while ((ch = getchar()) != EOF) {
        if (ch >= 'A' && ch <= 'Z') {
            ch += 32;
        }
        putchar(ch);
    }
    return 0;
}