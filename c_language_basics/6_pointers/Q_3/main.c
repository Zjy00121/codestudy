#include <stdio.h>

/*
 * 题目: 删除字符串中的子串
 *
 * 函数原型:
 *   int del_substr(char *str, char const *substr);
 *
 * 功能:
 *   判断 substr 是否出现在 str 中。如果出现，将 str 中位于该子串后面的
 *   所有字符复制到该子串的位置，从而删除这个子串（只删第一次出现），返回 1。
 *   如果未出现，返回 0，str 不变。
 *
 * 特殊情况:
 *   - substr 为空串 "" → 是任何字符串的子串，不修改 str，返回 1
 *   - str 为空串     → 无子串可删，返回 0
 *   - 任一参数为 NULL → 返回 0
 *
 * 要求:
 *   a. 不使用库函数
 *   b. 不使用下标引用
 *   c. 第二个参数的内容不会被修改
 *
 * 示例:
 *   str="ABCDEFG", substr="CDE" → str 变成 "ABFG", 返回 1
 *   str="ABCDEFG", substr="FGH" → 返回 0, str 不变
 */

int del_substr(char *str, char const *substr);

int main(void)
{
    char buf[32];
    int  ret;

    /* --- 测试1: 正常删除中间子串 --- */
    {
        char *s = "ABCDEFG", *d = buf;
        while ((*d++ = *s++) != '\0');
        ret = del_substr(buf, "CDE");
        printf("Test 1: \"ABCDEFG\" del \"CDE\"\n");
        printf("  ret=%d str=\"%s\" (expect 1, \"ABFG\")\n\n", ret, buf);
    }

    /* --- 测试2: 子串在开头 --- */
    {
        char *s = "ABCDEFG", *d = buf;
        while ((*d++ = *s++) != '\0');
        ret = del_substr(buf, "ABC");
        printf("Test 2: \"ABCDEFG\" del \"ABC\"\n");
        printf("  ret=%d str=\"%s\" (expect 1, \"DEFG\")\n\n", ret, buf);
    }

    /* --- 测试3: 子串在结尾 --- */
    {
        char *s = "ABCDEFG", *d = buf;
        while ((*d++ = *s++) != '\0');
        ret = del_substr(buf, "EFG");
        printf("Test 3: \"ABCDEFG\" del \"EFG\"\n");
        printf("  ret=%d str=\"%s\" (expect 1, \"ABCD\")\n\n", ret, buf);
    }

    /* --- 测试4~6: 子串不存在 --- */
    {
        char *s = "ABCDEFG", *d = buf;
        while ((*d++ = *s++) != '\0');
        ret = del_substr(buf, "FGH");
        printf("Test 4: \"ABCDEFG\" del \"FGH\"\n");
        printf("  ret=%d (expect 0)\n\n", ret);

        s = "ABCDEFG"; d = buf;
        while ((*d++ = *s++) != '\0');
        ret = del_substr(buf, "XABC");
        printf("Test 5: \"ABCDEFG\" del \"XABC\"\n");
        printf("  ret=%d (expect 0)\n\n", ret);

        s = "ABCDEFG"; d = buf;
        while ((*d++ = *s++) != '\0');
        ret = del_substr(buf, "CDF");
        printf("Test 6: \"ABCDEFG\" del \"CDF\"\n");
        printf("  ret=%d (expect 0)\n\n", ret);
    }

    /* --- 测试7: 空子串 — 返回1，str不变 --- */
    {
        char *s = "ABCDEFG", *d = buf;
        while ((*d++ = *s++) != '\0');
        ret = del_substr(buf, "");
        printf("Test 7: \"ABCDEFG\" del \"\"\n");
        printf("  ret=%d str=\"%s\" (expect 1, unchanged)\n\n", ret, buf);
    }

    /* --- 测试8: str=NULL --- */
    ret = del_substr(NULL, "ABC");
    printf("Test 8: NULL del \"ABC\"\n");
    printf("  ret=%d (expect 0)\n\n", ret);

    /* --- 测试9: substr=NULL --- */
    {
        char *s = "ABC", *d = buf;
        while ((*d++ = *s++) != '\0');
        ret = del_substr(buf, NULL);
        printf("Test 9: \"ABC\" del NULL\n");
        printf("  ret=%d (expect 0)\n\n", ret);
    }

    /* --- 测试10: str空串 --- */
    {
        buf[0] = '\0';
        ret = del_substr(buf, "ABC");
        printf("Test 10: \"\" del \"ABC\"\n");
        printf("  ret=%d (expect 0)\n\n", ret);
    }

    /* --- 测试11: 重复子串，只删第一个 --- */
    {
        char *s = "ABABAB", *d = buf;
        while ((*d++ = *s++) != '\0');
        ret = del_substr(buf, "AB");
        printf("Test 11: \"ABABAB\" del \"AB\"\n");
        printf("  ret=%d str=\"%s\" (expect 1, \"ABAB\")\n\n", ret, buf);
    }

    return 0;
}

/*
 * del_substr: 删除 str 中第一次出现的子串 substr
 *
 * 参数:
 *   str    — 被操作的字符串 (可写)
 *   substr — 要删除的子串 (只读)
 *
 * 返回:
 *   1 — 找到并删除了子串
 *   0 — 未找到 / 参数无效
 *
 * 算法:
 *   1. 处理 NULL / 空串边界条件
 *   2. 外层遍历 str 每个位置，作为匹配起点
 *   3. 用 scan(读指针) 和 pat(模式指针) 同步比较，匹配全部 substr
 *   4. 完整匹配后，write(写指针) 和 read(读指针) 把后面字符整体前移
 *   5. 全程使用指针副本，不破坏 str 和 substr 的原始指向
 */
int del_substr(char *str, char const *substr)
{
    char *scan;          /* 匹配读指针 */
    char const *pat;     /* 模式指针 (substr 副本，不动原参数) */
    char *write;         /* 删除时的写指针 */
    char *read;          /* 删除时的读指针 */

    /* NULL 参数检查 */
    if (str == NULL || substr == NULL)
        return 0;

    /* 空子串是任何串的子串，不修改 str，返回 1 */
    if (*substr == '\0')
        return 1;

    /* str 为空串，无子串可删 */
    if (*str == '\0')
        return 0;

    /* 外层: 遍历 str 每个字符作为匹配起点 */
    while (*str != '\0') {

        /* 从当前位置尝试完整匹配 substr */
        scan = str;
        pat  = substr;

        while (*pat != '\0' && *scan == *pat) {
            scan++;
            pat++;
        }

        /* pat 走到 '\0' → 子串全部匹配成功 */
        if (*pat == '\0') {
            /* 把 scan 之后的字符搬到 str 位置，覆盖掉子串 */
            write = str;         /* 写指针: 覆盖起点 = 子串起始位置 */
            read  = scan;        /* 读指针: 子串后的第一个字符 */

            while (*read != '\0') {
                *write++ = *read++;
            }
            *write = '\0';       /* 结束符 */

            return 1;
        }

        str++;  /* 当前不匹配，前进一格继续尝试 */
    }

    return 0;   /* str 遍历完毕，未找到匹配 */
}
