/*
 * 从一个字符串中去除多余的空格。
 * 当函数发现字符串中有一个地方由一个或多个连续的空格组成时，
 * 就把它们改成单个空格字符。遍历整个字符串，确保以 NUL 字符结尾。
 *
 * 示例：
 *   "Hello    Wor l   d!"  →  "Hello Wor l d!"
 */

#include <stdio.h>

void deblank (char string[]);

int main () {
    char string[] = "Hello    Wor l   d!";
    deblank (string);
    printf ("%s\n",string);
    return 0;
}

void deblank (char string[]) {
    int str_len = 0;
    int at_beg = 1;
    while (string[str_len] != '\0')
        str_len ++;
    for (int i = 0; i <= str_len; i ++) {
        if (string[i] == ' ' && at_beg == 1) {
            at_beg = 0;
        }
        else if (string[i] == ' ' && at_beg == 0) {
            for (int j = i; j <= str_len; j++) {
                string[j] = string[j+1];
            }
            i --;   /* 左移后回头检查补上来的字符，避免跳过 */
        }
        else if (string[i] != ' ' && at_beg == 0) {
            at_beg = 1;
        }
        
    }
}