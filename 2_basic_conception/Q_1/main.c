#include <stdio.h>

void say_hello(void);  /* 声明：告诉编译器这个函数存在 */

int main() {
    printf("This is main.c\n");
    say_hello();
    return 0;
}
