#include <stdio.h>

/*
 * 题目: 编写函数 find_char
 *
 * 在 source 字符串中查找第一个匹配 chars 集合中任意字符的字符，
 * 返回指向 source 中该位置的指针。如果 source 中无匹配、任一参数
 * 为 NULL、或任一字符串为空，返回 NULL。
 *
 * 要求:
 *   a. 不使用任何库函数 (strcpy, strcmp, strpbrk 等)
 *   b. 不使用下标引用 (全用指针操作)
 *
 * 原型:
 *   char *find_char(char const *source, char const *chars);
 *
 * 示例: source="ABCDEF", chars="XRCQEF" → 返回指向 'C' 的指针
 *       source="ABCDEF", chars="XYZ"    → 返回 NULL
 */

char *find_char(char const *source, char const *chars);

int main(void)
{
    /* 定义若干组 source 和 chars，用于测试各种边界情况 */
    char const *src;
    char const *chs;
    char *result;

    /* --- 测试1: 基本匹配 — chars="XRCQEF", 首次匹配是 source[2]='C' --- */
    src = "ABCDEF";
    chs = "XRCQEF";
    result = find_char(src, chs);
    printf("Test 1: source=\"%s\", chars=\"%s\"\n", src, chs);
    if (result != NULL)
        printf("  found '%c' at offset %d\n\n", *result, (int)(result - src));
    else
        printf("  not found\n\n");

    /* --- 测试2: 无匹配 — chars 全不在 source 中 --- */
    src = "ABCDEF";
    chs = "XYZ";
    result = find_char(src, chs);
    printf("Test 2: source=\"%s\", chars=\"%s\"\n", src, chs);
    printf("  %s\n\n", result ? "found" : "not found (expected NULL)");

    /* --- 测试3: chars 只有一个字符 --- */
    src = "ABCDEF";
    chs = "D";
    result = find_char(src, chs);
    printf("Test 3: source=\"%s\", chars=\"%s\"\n", src, chs);
    if (result != NULL)
        printf("  found '%c' at offset %d\n\n", *result, (int)(result - src));
    else
        printf("  not found\n\n");

    /* --- 测试4: chars 只有一个字符，且不匹配 --- */
    src = "ABCDEF";
    chs = "Z";
    result = find_char(src, chs);
    printf("Test 4: source=\"%s\", chars=\"%s\"\n", src, chs);
    printf("  %s\n\n", result ? "found" : "not found (expected NULL)");

    /* --- 测试5: source 为 NULL --- */
    result = find_char(NULL, "ABC");
    printf("Test 5: source=NULL, chars=\"ABC\"\n");
    printf("  %s\n\n", result ? "found" : "returned NULL (correct)");

    /* --- 测试6: chars 为 NULL --- */
    result = find_char("ABC", NULL);
    printf("Test 6: source=\"ABC\", chars=NULL\n");
    printf("  %s\n\n", result ? "found" : "returned NULL (correct)");

    /* --- 测试7: source 为空串 --- */
    result = find_char("", "ABC");
    printf("Test 7: source=\"\", chars=\"ABC\"\n");
    printf("  %s\n\n", result ? "found" : "returned NULL (correct)");

    /* --- 测试8: chars 为空串 --- */
    result = find_char("ABC", "");
    printf("Test 8: source=\"ABC\", chars=\"\"\n");
    printf("  %s\n\n", result ? "found" : "returned NULL (correct)");

    /* --- 测试9: chars 包含重复字符 (JURY → 每个都不匹配) --- */
    src = "ABCDEF";
    chs = "JURY";
    result = find_char(src, chs);
    printf("Test 9: source=\"%s\", chars=\"%s\"\n", src, chs);
    printf("  %s\n\n", result ? "found" : "not found (expected NULL)");

    /* --- 测试10: chars 包含重复字符 (QQQQ → 无匹配) --- */
    src = "ABCDEF";
    chs = "QQQQ";
    result = find_char(src, chs);
    printf("Test 10: source=\"%s\", chars=\"%s\"\n", src, chs);
    printf("  %s\n\n", result ? "found" : "not found (expected NULL)");

    /* --- 测试11: source 和 chars 都只有一个字符，正好匹配 --- */
    src = "A";
    chs = "A";
    result = find_char(src, chs);
    printf("Test 11: source=\"%s\", chars=\"%s\"\n", src, chs);
    if (result != NULL)
        printf("  found '%c'\n\n", *result);
    else
        printf("  not found\n\n");

    return 0;
}

/*
 * find_char: 在 source 中查找第一个匹配 chars 集合中任意字符的字符
 *
 * 参数:
 *   source — 被搜索的字符串 (只读)
 *   chars  — 字符集合 (只读)
 *
 * 返回:
 *   成功: 指向 source 中第一个匹配字符的指针
 *   失败: NULL (source 或 chars 为 NULL / 空串 / 无匹配)
 *
 * 算法:
 *   外层循环遍历 source 每个字符，
 *   内层循环用 scan 指针扫描 chars 全部字符做比对。
 *   一旦匹配，立即返回 source 当前位置。
 *   全程不使用下标，纯指针操作。
 */
char *find_char(char const *source, char const *chars)
{
    char const *scan;   /* 内层工作指针，用于扫描 chars */

    /* NULL 参数或空串 → 直接返回 NULL */
    if (source == NULL || chars == NULL)
        return NULL;
    if (*source == '\0' || *chars == '\0')
        return NULL;

    /* 外层: 逐个遍历 source 的字符 */
    while (*source != '\0') {

        /* 内层: 对当前 source 字符，从头扫描 chars 看是否匹配 */
        scan = chars;
        while (*scan != '\0') {
            if (*source == *scan)
                return (char *)source;   /* 找到匹配，返回当前位置 */
            scan++;
        }

        source++;   /* 当前 source 字符不匹配任何 chars 字符，前进 */
    }

    return NULL;   /* source 全部扫完，无一匹配 */
}
