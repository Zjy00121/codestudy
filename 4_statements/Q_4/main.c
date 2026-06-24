/*
 * 从标准输入一行一行地读取文本，如果有两行或更多行相邻的文本内容相同，
 * 就打印出其中一行，其余的行不打印。
 * 假设文本行长度不超过 128 字符（含换行符）。
 *
 * 示例输入：
 *   This is the first line.
 *   Another line.
 *   And another.
 *   And another.
 *   And another.
 *   And another.
 *   Still more.
 *   Almost done now--
 *   Almost done now--
 *   Another line
 *   Still more.
 *   Finished!
 *
 * 输出：相邻重复行只保留一行。
 */

#include <stdio.h>
#include <string.h>

#define MAX_LEN 128

int main(void) {
    char cur[MAX_LEN];
    char prev[MAX_LEN];
    int is_first = 1;

    printf("Enter lines, Ctrl+Z to finish:\n");

    while (fgets(cur, MAX_LEN, stdin) != NULL) {
        if (is_first) {
            printf("%s", cur);
            strcpy(prev, cur);
            is_first = 0;
        } else if (strcmp(cur, prev) != 0) {
            printf("%s", cur);
            strcpy(prev, cur);
        }
    }

    return 0;
}
