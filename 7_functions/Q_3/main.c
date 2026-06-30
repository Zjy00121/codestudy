#include <stdio.h>

/*
 * 题目: 字符串转整数 (atoi 简化版)
 *
 * 将数字字符串转换为对应的整数值。遇到第一个非数字字符时停止转换，
 * 返回已转换的值。空串或首字符非数字返回 0。
 */

int ascii_to_integer(char *string);

int main(void)
{
    /* --- 正常转换 --- */
    printf("Test 1: \"1223\"       -> %d (expect 1223)\n",
           ascii_to_integer("1223"));
    printf("Test 2: \"0\"          -> %d (expect 0)\n",
           ascii_to_integer("0"));
    printf("Test 3: \"9\"          -> %d (expect 9)\n",
           ascii_to_integer("9"));

    /* --- 多位大数 --- */
    printf("Test 4: \"99999\"      -> %d (expect 99999)\n",
           ascii_to_integer("99999"));

    /* --- 前导零 --- */
    printf("Test 5: \"007\"        -> %d (expect 7)\n",
           ascii_to_integer("007"));

    /* --- 字母开头 / 空串 / 混合 --- */
    printf("Test 6: \"abc\"        -> %d (expect 0)\n",
           ascii_to_integer("abc"));
    printf("Test 7: \"\"           -> %d (expect 0)\n",
           ascii_to_integer(""));
    printf("Test 8: \"123abc\"     -> %d (expect 123)\n",
           ascii_to_integer("123abc"));

    /* --- NULL 保护 --- */
    printf("Test 9: NULL          -> %d (expect 0)\n",
           ascii_to_integer(NULL));

    return 0;
}

/*
 * ascii_to_integer: 将数字字符串转换为整数值
 *
 * 参数:
 *   string — 待转换的字符串 (只读)，允许 NULL 或空串
 *
 * 返回:
 *   成功 — 转换后的整数
 *   失败 — 0 (NULL / 空串 / 首字符非数字)
 *
 * 算法:
 *   value = 0
 *   从左到右逐字符扫描:
 *     value = value * 10 + (字符 - '0')
 *   遇到非数字字符立即停止，返回当前 value
 *
 *   例子: "123" → 0*10+1=1 → 1*10+2=12 → 12*10+3=123
 */
int ascii_to_integer(char *string)
{
    int value = 0;

    /* NULL 指针保护 */
    if (string == NULL)
        return 0;

    /* 空串保护 */
    if (*string == '\0')
        return 0;

    /* 逐字符转换，遇到非数字立即停止 */
    while (*string != '\0') {

        /* 非数字字符 → 停止转换，返回已累积的值 */
        if (*string < '0' || *string > '9')
            break;

        /* 核心公式: 低位进位 + 当前数字 */
        value = value * 10 + (*string - '0');

        string++;   /* ← 指针前进！原代码缺这一行导致死循环 */
    }

    return value;
}
