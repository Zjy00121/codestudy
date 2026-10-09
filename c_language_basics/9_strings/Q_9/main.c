#include <stdio.h>
#include <string.h>

/*
 * 题目: 子串匹配
 *
 * 编写函数 count_substr，统计子串 substr 在字符串 str 中出现的次数。
 *
 * 函数原型:
 *   int count_substr(char const *str, char const *substr);
 *
 * 示例:
 *   count_substr("hello world", "o")   → 2
 *   count_substr("ababab", "ab")       → 3
 *   count_substr("banana", "ana")      → 2
 *   count_substr("hello", "xyz")       → 0
 */

int count_substr(char const *str, char const *substr);

int main(void)
{
    /* --- 测试1: 单个字符子串 --- */
    printf("Test 1: count_substr(\"hello world\", \"o\")   = %d (expect 2)\n",
           count_substr("hello world", "o"));

    /* --- 测试2: 多字符子串 --- */
    printf("Test 2: count_substr(\"ababab\", \"ab\")       = %d (expect 3)\n",
           count_substr("ababab", "ab"));

    /* --- 测试3: 重叠子串 --- */
    printf("Test 3: count_substr(\"banana\", \"ana\")      = %d (expect 2)\n",
           count_substr("banana", "ana"));

    /* --- 测试4: 不存在 --- */
    printf("Test 4: count_substr(\"hello\", \"xyz\")       = %d (expect 0)\n",
           count_substr("hello", "xyz"));

    /* --- 测试5: 空子串 --- */
    printf("Test 5: count_substr(\"hello\", \"\")          = %d (expect 0)\n",
           count_substr("hello", ""));

    /* --- 测试6: 空字符串 --- */
    printf("Test 6: count_substr(\"\", \"abc\")            = %d (expect 0)\n",
           count_substr("", "abc"));

    /* --- 测试7: 子串比原串长 --- */
    printf("Test 7: count_substr(\"hi\", \"hello\")        = %d (expect 0)\n",
           count_substr("hi", "hello"));

    /* --- 测试8: 子串等于原串 --- */
    printf("Test 8: count_substr(\"abc\", \"abc\")         = %d (expect 1)\n",
           count_substr("abc", "abc"));

    return 0;
}

/*
 * count_substr: 统计子串 substr 在字符串 str 中出现的次数
 *
 * 参数:
 *   str    — 被搜索的字符串（只读）
 *   substr — 要匹配的子串（只读）
 *
 * 返回:
 *   substr 在 str 中出现的次数（0 表示不存在）
 *
 * 算法:
 *   外层循环遍历 str 的每个字符作为匹配起点，
 *   内层用 p/q 双指针从该起点尝试完整匹配 substr，
 *   匹配成功（q 走到 substr 的 '\0'）则计数 +1。
 *
 * 注意:
 *   - 空子串按 0 次处理（空串匹配无意义）
 *   - 支持重叠匹配：count_substr("aaa", "aa") = 2
 */
int count_substr(char const *str, char const *substr)
{
    int count = 0;
    char const *p;   /* str 的匹配游标 */
    char const *q;   /* substr 的匹配游标 */

    /* 空子串无意义，直接返回 0 */
    if (*substr == '\0')
        return 0;

    /* 外层: 以 str 每个字符为起点尝试匹配 */
    while (*str != '\0') {

        /* 从 str 当前位置开始，和 substr 逐字符比较 */
        p = str;
        q = substr;

        while (*q != '\0' && *p == *q) {
            p++;
            q++;
        }

        /* q 走到 substr 末尾 → 整个子串匹配成功 */
        if (*q == '\0')
            count++;

        str++;   /* 起点前进一格 */
    }

    return count;
}
