#include <stdio.h>

int main () {
    int num;

    printf("Enter a number to get its square root, Ctrl+Z to quit\n");

    while (scanf("%d", &num) == 1) 
    {
        double a_i = 1;
        double a_i1 = (1.0 + num) / 2;

        /* 牛顿迭代法 */
        while (a_i1 - a_i > 0.00000001 || a_i - a_i1 > 0.00000001) 
        {
            a_i = a_i1;
            a_i1 = (a_i + num / a_i) / 2;
        }
        printf("sqrt(%d) = %f\n", num, a_i1);
    }

    return 0;
}