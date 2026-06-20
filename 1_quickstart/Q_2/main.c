#include <stdio.h>

int main(){
    unsigned char checknum;
    int ch;
    checknum = -1;
    printf ( "I am ready\n" );
    while ( (ch = getchar ()) != EOF )
    {
        checknum = checknum + ch;
        putchar ( ch );
        if ( ch == '\n' )
        {
            printf ( "%d\n",checknum );
        }
    }
    return 0;
}