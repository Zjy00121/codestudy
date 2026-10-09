#include <stdio.h>

/*
 * 题目: 反转字符串
 *
 * 函数原型:
 *   void reverse_string(char *string);
 *
 * 功能:
 *   将参数字符串中的字符反向排列（原地反转）。
 *
 * 要求:
 *   a. 使用指针而非数组下标
 *   b. 不使用任何 C 标准库字符串函数
 *   c. 不声明局部数组来临时存储参数字符串
 *
 * 示例:
 *   "abc"  → "cba"
 *   "ab"   → "ba"
 *   "a"    → "a"
 *   ""     → ""
 */

void reverse_string(char *string);

int main(void)
{
    char buf[32];
    char *s, *d;

    /* --- 测试1: 奇数长度 --- */
    s = "abc"; d = buf;
    while ((*d++ = *s++) != '\0');
    reverse_string(buf);
    printf("Test 1: \"abc\" -> \"%s\" (expect \"cba\")\n", buf);

    /* --- 测试2: 偶数长度 --- */
    s = "abcd"; d = buf;
    while ((*d++ = *s++) != '\0');
    reverse_string(buf);
    printf("Test 2: \"abcd\" -> \"%s\" (expect \"dcba\")\n", buf);

    /* --- 测试3: 单字符 --- */
    s = "a"; d = buf;
    while ((*d++ = *s++) != '\0');
    reverse_string(buf);
    printf("Test 3: \"a\" -> \"%s\" (expect \"a\")\n", buf);

    /* --- 测试4: 空串 --- */
    buf[0] = '\0';
    reverse_string(buf);
    printf("Test 4: \"\" -> \"%s\" (expect \"\")\n", buf);

    /* --- 测试5: 长字符串 --- */
    s = "ABCDEFGH"; d = buf;
    while ((*d++ = *s++) != '\0');
    reverse_string(buf);
    printf("Test 5: \"ABCDEFGH\" -> \"%s\" (expect \"HGFEDCBA\")\n", buf);

    /* --- 测试6: 回文 --- */
    s = "aba"; d = buf;
    while ((*d++ = *s++) != '\0');
    reverse_string(buf);
    printf("Test 6: \"aba\" -> \"%s\" (expect \"aba\")\n", buf);

    /* --- 测试7: NULL 指针，函数应安全返回 --- */
    reverse_string(NULL);
    printf("Test 7: NULL -> no crash\n");

    return 0;
}

/*
 * reverse_string: 原地反转字符串
 *
 * 参数:
 *   string — 待反转的字符串 (可写)，允许为 NULL
 *
 * 算法:
 *   1. NULL / 空串检查，直接返回
 *   2. 用指针算出字符串长度（找 '\0'）
 *   3. 首尾双指针 p_left, p_right 向中间靠拢，交换字符
 *   4. 不使用下标、不使用库函数、不使用临时数组
 */
void reverse_string(char *string)
{
    char *p_left;        /* 左指针，从开头向右走 */
    char *p_right;       /* 右指针，从结尾向左走 */
    char  temp;          /* 交换用临时变量 */
    int   len;

    /* NULL 或空串，无需处理 */
    if (string == NULL || *string == '\0')
        return;

    /* 计算字符串长度 */
    len = 0;
    while (*(string + len) != '\0')
        len++;

    /* 双指针相向而行，交换首尾字符 */
    p_left  = string;               /* 指向第一个字符 */
    p_right = string + len - 1;     /* 指向最后一个字符（非 '\0'） */

    while (p_left < p_right) {
        /* 交换 */
        temp     = *p_left;
        *p_left  = *p_right;
        *p_right = temp;

        p_left++;                    /* 左指针右移 */
        p_right--;                   /* 右指针左移 */
    }
}
