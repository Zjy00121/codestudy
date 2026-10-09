#include <stdio.h>

int increment ( int a );
int negate ( int a );

int main () {
    int a, b;
    a = increment ( 10 );
    b = negate ( 10 );
    printf ( "%d %d\n", a, b );
    a = increment ( 0 );
    b = negate ( 0 );
    printf  ( "%d %d\n", a, b );
    a = increment ( -10 );
    b = negate ( -10 );
    printf  ( "%d %d\n", a, b );
    return 0;
}