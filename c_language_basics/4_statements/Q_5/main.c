/*
 * 从一个字符串中提取子字符串。
 * 从 src 起始位置向后偏移 start 个字符的位置开始，
 * 最多复制 len 个非 NUL 字符到 dst，dst 以 NUL 结尾。
 * 返回 dst 中字符串的长度。
 * 如果 start 越界或 start/len 为负，返回空字符串。
 */

#include <stdio.h>
#include <string.h>

int substr(char dst[], char src[], int start, int len);

int main(void) {
    char src[] = "Hello World";
    char dst[20];

    /* test 1: normal extraction */
    int n = substr(dst, src, 6, 10);          /* "World" */
    printf("test 1: \"%s\" len=%d\n", dst, n);

    /* test 2: start out of range */
    n = substr(dst, src, 20, 3);
    printf("test 2: \"%s\" len=%d\n", dst, n);

    /* test 3: negative len */
    n = substr(dst, src, 0, -1);
    printf("test 3: \"%s\" len=%d\n", dst, n);

    /* test 4: len longer than remaining characters */
    n = substr(dst, src, 0, 100);             /* "Hello World" */
    printf("test 4: \"%s\" len=%d\n", dst, n);

    return 0;
}

int substr(char dst[], char src[], int start, int len) {
    int src_len = 0;
    int i;

    /* find the length of src (number of non-NUL chars) */
    while (src[src_len] != '\0')
        src_len++;

    /* check error conditions: empty string result */
    if (start < 0 || len < 0 || start >= src_len) {
        dst[0] = '\0';
        return 0;
    }

    /* copy at most len characters, stop at NUL or end of src */
    for (i = 0; i < len && src[start + i] != '\0'; i++)
        dst[i] = src[start + i];

    dst[i] = '\0';     /* NUL-terminate */
    return i;          /* return the actual length */
}