#include <stdio.h>
#include <string.h>

/*
 * 题目: 编写 count_chars
 *
 * 函数原型:
 *   int count_chars(char const *str, char const *chars);
 *
 * 功能:
 *   统计 str 中有多少个字符属于 chars 这个"字符集合"。
 *   即：对 str 中的每个字符，判断它是否出现在 chars 里，是则计数。
 *
 * 示例:
 *   count_chars("hello", "aeiou")  → 2   ('e' 和 'o' 是元音)
 *   count_chars("hello", "l")      → 2   (两个 'l')
 */

int count_chars(char const *str, char const *chars);

int main(void)
{
    /* --- 测试1: 元音统计 --- */
    printf("Test 1: count_chars(\"hello\", \"aeiou\") = %d (expect 2)\n",
           count_chars("hello", "aeiou"));

    /* --- 测试2: 单个字符 --- */
    printf("Test 2: count_chars(\"hello\", \"l\")     = %d (expect 2)\n",
           count_chars("hello", "l"));

    /* --- 测试3: 集合中没有一个匹配 --- */
    printf("Test 3: count_chars(\"hello\", \"xyz\")   = %d (expect 0)\n",
           count_chars("hello", "xyz"));

    /* --- 测试4: 集合覆盖所有字符 --- */
    printf("Test 4: count_chars(\"hello\", \"hello\") = %d (expect 5)\n",
           count_chars("hello", "hello"));

    /* --- 测试5: 空字符串 --- */
    printf("Test 5: count_chars(\"\", \"abc\")        = %d (expect 0)\n",
           count_chars("", "abc"));

    /* --- 测试6: 空字符集合 --- */
    printf("Test 6: count_chars(\"hello\", \"\")      = %d (expect 0)\n",
           count_chars("hello", ""));

    /* --- 测试7: 统计数字个数 --- */
    printf("Test 7: count_chars(\"a1b2c3\", \"0123456789\") = %d (expect 3)\n",
           count_chars("a1b2c3", "0123456789"));

    return 0;
}

/*
 * count_chars: 统计 str 中属于 chars 集合的字符数量
 *
 * 参数:
 *   str   — 被查找的字符串（只读）
 *   chars — 字符集合（只读），每个字符都是匹配目标
 *
 * 返回:
 *   str 中属于 chars 集合的字符个数
 *
 * 算法:
 *   外层循环遍历 str 的每个字符，
 *   内层循环扫描 chars，看当前字符是否在集合中，
 *   找到匹配则计数 +1 并跳出内层，继续处理下一个字符。
 */
int count_chars(char const *str, char const *chars)
{
    int count = 0;
    char const *p;

    /* 外层: 遍历 str 每个字符 */
    while (*str != '\0') {

        /* 内层: 扫描 chars，判断 *str 是否在集合中 */
        for (p = chars; *p != '\0'; p++) {
            if (*p == *str) {
                count++;      /* 匹配到一个 */
                break;        /* 跳出内层，处理 str 的下一个字符 */
            }
        }

        str++;
    }

    return count;
}
