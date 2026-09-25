#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * 题目: 判断回文
 *
 * 函数原型:
 *   int palindrome(char const *string);
 *
 * 规则:
 *   - 回文: 从左往右读和从右往左读一样
 *   - 比较时忽略所有非字母字符
 *   - 字符比较不区分大小写
 *
 * 示例:
 *   palindrome("madam")                     → 真
 *   palindrome("Madam")                     → 真（忽略大小写）
 *   palindrome("A man, a plan, a canal")    → 真（忽略空格标点）
 */

int palindrome(char const *string);

int main(void)
{
    /* --- 全小写回文 --- */
    printf("Test 1: \"madam\"                    -> %s (expect true)\n",
           palindrome("madam") ? "true" : "false");

    /* --- 忽略大小写 --- */
    printf("Test 2: \"Madam\"                    -> %s (expect true)\n",
           palindrome("Madam") ? "true" : "false");

    /* --- 忽略空格和标点 --- */
    printf("Test 3: \"A man, a plan, a canal: Panama\" -> %s (expect true)\n",
           palindrome("A man, a plan, a canal: Panama") ? "true" : "false");

    /* --- 经典回文（忽略空格标点大小写） --- */
    printf("Test 4: \"No 'x' in Nixon\"          -> %s (expect true)\n",
           palindrome("No 'x' in Nixon") ? "true" : "false");

    /* --- 非回文 --- */
    printf("Test 5: \"hello\"                    -> %s (expect false)\n",
           palindrome("hello") ? "true" : "false");

    /* --- 非回文（含空格） --- */
    printf("Test 6: \"not a palindrome\"         -> %s (expect false)\n",
           palindrome("not a palindrome") ? "true" : "false");

    /* --- 单字符 --- */
    printf("Test 7: \"a\"                        -> %s (expect true)\n",
           palindrome("a") ? "true" : "false");

    /* --- 空字符串 --- */
    printf("Test 8: \"\"                         -> %s (expect true)\n",
           palindrome("") ? "true" : "false");

    /* --- 纯数字（全是非字母，忽略后为空） --- */
    printf("Test 9: \"12321\"                    -> %s (expect true)\n",
           palindrome("12321") ? "true" : "false");

    return 0;
}

/*
 * palindrome: 判断字符串是否为回文
 *
 * 参数:
 *   string — 待判断的字符串（只读，不修改原串）
 *
 * 返回:
 *   1 — 是回文
 *   0 — 不是回文
 *
 * 算法:
 *   双指针 left/right 分别从字符串两端向中间靠拢，
 *   每步先跳过非字母字符，再用 tolower 不区分大小写比较。
 *   只要有一对字符不同就返回 0，全部相同返回 1。
 *
 * 关键点:
 *   - 用 isalpha 跳过非字母（"忽略"而非"删除"，不修改原串）
 *   - 用 tolower 统一转小写后再比较
 *   - const 参数: 只读访问，允许传入字符串字面量
 */
int palindrome(char const *string)
{
    char const *left  = string;                     /* 左指针，从开头向右 */
    char const *right = string + strlen(string) - 1; /* 右指针，从结尾向左 */

    while (left < right) {

        /* 跳过左侧的非字母字符 */
        while (left < right && !isalpha((unsigned char)*left))
            left++;

        /* 跳过右侧的非字母字符 */
        while (left < right && !isalpha((unsigned char)*right))
            right--;

        /* 不区分大小写比较两个字母 */
        if (tolower((unsigned char)*left) != tolower((unsigned char)*right))
            return 0;

        left++;      /* 左指针右移 */
        right--;     /* 右指针左移 */
    }

    return 1;   /* 所有字符对都匹配 */
}
