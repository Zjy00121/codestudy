#include <stdio.h>

int main () {
    int a, b, c;
    scanf ("%d %d %d", &a, &b, &c);
    if (a + b > c && a + c > b && b + c > a) {
        if (a == b && b == c)
            printf ("Equilateral triangle!\n");
        else if (a == b || a == c || b == c)
            printf ("Isosceles triangle!\n");
        else
            printf ("Scalene triangle!\n");
    }
    else printf ("it is not triangle!\n");
    return 0;
}