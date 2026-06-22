#include <stdio.h>

int main () {
    int ch;
    int brace = 0;
    while ( (ch = getchar ()) != EOF )
    {
        if ( ch == '{' )
        {
            brace ++;
        }
        if ( ch == '}' )
        {
            if ( brace == 0 )
                printf ( "Extra right brace!\n" );
            else
                brace --;
        }
    }
    if ( brace > 0 )
         printf ( "%d unmatched opening brace(s)!\n", brace );
    return 0;
}