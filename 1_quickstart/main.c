#include <stdio.h>
#include <stdlib.h>

int main(){
    int ch;
    int line;
    int at_beginning;
    line = 0;
    at_beginning = 1;
    printf ( "I am ready\n" );
    while ( (ch = getchar()) != EOF )
    {
        if ( at_beginning == 1 )
        {
            at_beginning = 0;
            line += 1;
            printf( "%d ", line );
        }
        putchar( ch );
        if ( ch == '\n' )
        {
            at_beginning = 1;
        }
        
    }
    return 0;
}