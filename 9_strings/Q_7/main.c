#include <stdio.h>
#include <string.h>

/*
 * 题目: 编写 my_strnchr
 *
 * 类似 strchr，但第 3 个参数 which 指定查找 ch 第几次出现。
 *
 * 函数原型:
 *   char *my_strnchr(char const *str, int ch, int which);
 *
 * 参数:
 *   str   — 被搜索的字符串
 *   ch    — 要查找的字符
 *   which — 查找第几次出现（1 = 第一次，等同 strchr；2 = 第二次...）
 *
 * 返回:
 *   指向 ch 第 which 次出现位置的指针；不存在返回 NULL
 */

char *my_strnchr(char const *str, int ch, int which);

int main(void)
{
    char *p;

    /* --- 测试1: which=1，等同 strchr --- */
    {
        p = my_strnchr("hello", 'l', 1);
        printf("Test 1: my_strnchr(\"hello\", 'l', 1)\n");
        if (p) printf("  -> \"%s\" (expect \"llo\")\n\n", p);
        else   printf("  -> NULL\n\n");
    }

    /* --- 测试2: which=2，第二个 'l' --- */
    {
        p = my_strnchr("hello", 'l', 2);
        printf("Test 2: my_strnchr(\"hello\", 'l', 2)\n");
        if (p) printf("  -> \"%s\" (expect \"lo\")\n\n", p);
        else   printf("  -> NULL\n\n");
    }

    /* --- 测试3: which 超出出现次数 → NULL --- */
    {
        p = my_strnchr("hello", 'l', 3);
        printf("Test 3: my_strnchr(\"hello\", 'l', 3)\n");
        if (p) printf("  -> \"%s\"\n\n", p);
        else   printf("  -> NULL (expect NULL, 只有 2 个 'l')\n\n");
    }

    /* --- 测试4: 单次出现的字符 --- */
    {
        p = my_strnchr("hello", 'h', 1);
        printf("Test 4: my_strnchr(\"hello\", 'h', 1)\n");
        if (p) printf("  -> \"%s\" (expect \"hello\")\n\n", p);
        else   printf("  -> NULL\n\n");
    }

    /* --- 测试5: 完全不存在 --- */
    {
        p = my_strnchr("hello", 'z', 1);
        printf("Test 5: my_strnchr(\"hello\", 'z', 1)\n");
        if (p) printf("  -> \"%s\"\n\n", p);
        else   printf("  -> NULL (expect NULL)\n\n");
    }

    /* --- 测试6: which=0 → NULL --- */
    {
        p = my_strnchr("hello", 'l', 0);
        printf("Test 6: my_strnchr(\"hello\", 'l', 0)\n");
        if (p) printf("  -> \"%s\"\n\n", p);
        else   printf("  -> NULL (expect NULL)\n\n");
    }

    /* --- 测试7: 多个出现，找第 4 次 --- */
    {
        p = my_strnchr("banana", 'a', 3);
        printf("Test 7: my_strnchr(\"banana\", 'a', 3)\n");
        if (p) printf("  -> \"%s\" (expect \"a\", 第3个'a'在末尾)\n\n", p);
        else   printf("  -> NULL\n\n");
    }

    return 0;
}

/*
 * my_strnchr: 查找字符 ch 第 which 次出现的位置
 *
 * 参数:
 *   str   — 被搜索的字符串（只读）
 *   ch    — 要查找的字符
 *   which — 查找第几次出现
 *
 * 返回:
 *   指向 ch 第 which 次出现位置的指针；不存在返回 NULL
 *
 * 算法:
 *   从左到右扫描，用 count 记录匹配次数，
 *   每次匹配 count++，当 count == which 时立即返回当前位置。
 *
 * 关键: 计数只在"匹配成功"时递增，而不是每个字符都递增。
 *   which <= 0 视为无效，返回 NULL。
 */
char *my_strnchr(char const *str, int ch, int which)
{
    int count = 0;   /* 已匹配的次数 */

    /* which 必须至少为 1，否则无意义 */
    if (which <= 0)
        return NULL;

    while (*str != '\0') {
        if (*str == ch) {
            count++;               /* 只在匹配时计数 */
            if (count == which)    /* 达到目标次数 */
                return (char *)str;
        }
        str++;
    }

    return NULL;   /* 扫描完都没达到 which 次 */
}
