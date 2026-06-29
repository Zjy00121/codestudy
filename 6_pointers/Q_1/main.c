#include <stdio.h>

/* 宏末尾不加分号，否则展开后多一个空语句 */
#define TRUE  1
#define FALSE 0

int find_char(char **strings, char value);

int main(void)
{
    /* ================================================================
     * find_char 测试 — 在字符串数组中查找字符
     * ================================================================ */
    printf("\n--- find_char test ---\n");
    char *words[] = {"hello", "world", "test", NULL};  /* NULL 作为数组结束标记 */
    printf("Search 'o' in {\"hello\",\"world\",\"test\"}: %s\n",
           find_char(words, 'o') ? "found" : "not found");
    printf("Search 'z' in {\"hello\",\"world\",\"test\"}: %s\n",
           find_char(words, 'z') ? "found" : "not found");

    return 0;
}

/*
 * find_char: 在 NULL 结尾的字符串数组中查找字符 value
 *   strings — 指向 char* 数组的指针 (即 char**)
 *   value   — 要查找的字符
 *   找到返回 TRUE，否则返回 FALSE
 *
 *   关键行: string = *strings++
 *     第1步: *strings  — 取出当前字符串指针
 *     第2步: strings++ — strings 指向下一个 char*
 *     第3步: 把第1步的值赋给 string
 *     当取出的指针为 NULL 时循环结束（哨兵值）
 */
int find_char(char **strings, char value)
{
    char *string;                     /* 指向当前字符串中某个字符 */
    while ((string = *strings++) != NULL) {
        while (*string != '\0') {
            if (*string++ == value)   /* 比较当前字符，然后指针后移 */
                return TRUE;
        }
    }
    return FALSE;
}
