#include <stdio.h>
#include "int_sizes.h"

int main () {
    int_8 a;
    int_16 b;
    int_32 c;
    int_64 d;
    printf ( "%zu %zu %zu %zu\n", sizeof(a)*8, sizeof(b)*8, sizeof(c)*8, sizeof(d)*8 );
    return 0;
}