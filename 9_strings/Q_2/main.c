#include <stdio.h>
#include <string.h>

/*
 * 题目: 编写 my_strlen
 *
 * 类似 strlen，但能处理"没有 NUL 结尾"的字符串
 * （例如 strncpy 截断后产生的、缺少 '\0' 的字符串）。
 *
 * 函数原型:
 *   int my_strlen(char *p, int size);
 *
 * 参数:
 *   p    — 待测字符串的起始地址
 *   size — 存放字符串的数组长度（搜索上限）
 *
 * 返回:
 *   第一个 '\0' 出现的位置（即字符串长度）；
 *   若在 size 个字符内都没有 '\0'，返回 size。
 */

int my_strlen(char const *p, int size);

int main(void)
{
    /* --- 测试1: 正常 NUL 结尾的字符串 --- */
    {
        char s[] = "hello";                    /* "hello\0"，数组长度 6 */
        printf("Test 1: \"hello\"       len=%d (expect 5)\n",
               my_strlen(s, sizeof(s)));
    }

    /* --- 测试2: 没有 NUL 结尾（模拟 strncpy 截断） --- */
    {
        char s[5] = {'h', 'e', 'l', 'l', 'o'}; /* 5 字节全被占满，无 '\0' */
        printf("Test 2: \"hello\" no NUL len=%d (expect 5)\n",
               my_strlen(s, sizeof(s)));
    }

    /* --- 测试3: 空字符串 --- */
    {
        char s[] = "";                         /* 只有 '\0' */
        printf("Test 3: \"\"            len=%d (expect 0)\n",
               my_strlen(s, sizeof(s)));
    }

    /* --- 测试4: 单字符 --- */
    {
        char s[] = "a";                        /* "a\0" */
        printf("Test 4: \"a\"           len=%d (expect 1)\n",
               my_strlen(s, sizeof(s)));
    }

    /* --- 测试5: '\0' 在数组最后一个位置 --- */
    {
        char s[4] = {'a', 'b', 'c', '\0'};     /* '\0' 在末尾 */
        printf("Test 5: \"abc\\0\"       len=%d (expect 3)\n",
               my_strlen(s, sizeof(s)));
    }

    /* --- 测试6: 用 memcpy 产生无 NUL 结尾的截断串 --- */
    {
        char s[4];                             /* 只能放 3 个字符 */
        memcpy(s, "hello world", 3);           /* 拷贝 "hel"，无 '\0' */
        printf("Test 6: memcpy 3 bytes  len=%d (expect 3)\n",
               my_strlen(s, 3));
    }

    /* --- 测试7: '\0' 在中间 --- */
    {
        char s[10] = {'a', 'b', '\0', 'c', 'd'};
        printf("Test 7: \"ab\\0cd\"       len=%d (expect 2)\n",
               my_strlen(s, sizeof(s)));
    }

    return 0;
}

/*
 * my_strlen: 计算字符串长度（支持无 NUL 结尾的字符串）
 *
 * 参数:
 *   p    — 字符串起始地址（只读）
 *   size — 数组长度，即搜索的字符数上限
 *
 * 返回:
 *   第一个 '\0' 的下标；若 size 范围内无 '\0'，返回 size
 *
 * 算法:
 *   遍历最多 size 个字符，找到 '\0' 就返回当前位置；
 *   遍历完 size 个都没找到，说明字符串没有结尾，返回 size。
 *
 * 和 strlen 的区别:
 *   strlen 无限往后找 '\0'，遇到没有结尾的串会越界读；
 *   my_strlen 用 size 限制搜索范围，不会越界。
 */
int my_strlen(char const *p, int size)
{
    int len;

    for (len = 0; len < size; len++) {
        if (*p++ == '\0')   /* 读到 '\0' → 结束，len 就是字符串长度 */
            break;
    }

    return len;   /* 没找到 '\0' 时，len 恰好等于 size */
}
