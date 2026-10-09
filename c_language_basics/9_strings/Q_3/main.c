#include <stdio.h>
#include <string.h>

/*
 * 题目: 编写 my_strcpy
 *
 * 类似 strcpy，但不会溢出目标数组，且复制结果一定是真正的字符串
 * （即一定以 '\0' 结尾）。
 *
 * 函数原型:
 *   void my_strcpy(char *dst, char const *src, int size);
 *
 * 参数:
 *   dst  — 目标缓冲区
 *   src  — 源字符串
 *   size — 目标缓冲区的容量（字节数）
 *
 * 行为:
 *   最多复制 size-1 个字符（留 1 字节给 '\0'），
 *   源串更长时会被截断，但结果始终以 '\0' 结尾，绝不溢出。
 */

void my_strcpy(char *dst, char const *src, int size);

int main(void)
{
    /* --- 测试1: 源串比目标短，正常复制 --- */
    {
        char dst[20] = "xxxxxx";                 /* 预置内容，测试会覆盖 */
        my_strcpy(dst, "hello", sizeof(dst));
        printf("Test 1: copy \"hello\" into 20-byte buf\n");
        printf("  -> \"%s\" (expect \"hello\")\n\n", dst);
    }

    /* --- 测试2: 源串比目标长，截断但不溢出 --- */
    {
        char dst[5];                             /* 只能放 4 个字符 + '\0' */
        my_strcpy(dst, "hello world", sizeof(dst));
        printf("Test 2: copy \"hello world\" into 5-byte buf\n");
        printf("  -> \"%s\" (expect \"hell\", truncated)\n\n", dst);
    }

    /* --- 测试3: 恰好放下（src 长度 = size-1） --- */
    {
        char dst[4];                             /* 放 "abc" + '\0' 正好 */
        my_strcpy(dst, "abc", sizeof(dst));
        printf("Test 3: copy \"abc\" into 4-byte buf\n");
        printf("  -> \"%s\" (expect \"abc\")\n\n", dst);
    }

    /* --- 测试4: 空字符串 --- */
    {
        char dst[10] = "junk";
        my_strcpy(dst, "", sizeof(dst));
        printf("Test 4: copy \"\" into 10-byte buf\n");
        printf("  -> \"%s\" (expect \"\")\n\n", dst);
    }

    /* --- 测试5: size=1，只能放 '\0' --- */
    {
        char dst[1];
        my_strcpy(dst, "hello", sizeof(dst));
        printf("Test 5: copy \"hello\" into 1-byte buf\n");
        printf("  -> \"%s\" (expect \"\", empty)\n\n", dst);
    }

    /* --- 测试6: 验证结果确实是合法字符串（可用 strlen） --- */
    {
        char dst[6];
        my_strcpy(dst, "hello world", sizeof(dst));
        printf("Test 6: result is a true string?\n");
        printf("  -> \"%s\", strlen=%d (expect 5)\n",
               dst, (int)strlen(dst));
    }

    return 0;
}

/*
 * my_strcpy: 安全地复制字符串，绝不溢出目标缓冲区
 *
 * 参数:
 *   dst  — 目标缓冲区
 *   src  — 源字符串（只读）
 *   size — 目标缓冲区容量（字节数）
 *
 * 算法:
 *   最多复制 size-1 个字符，始终在末尾补 '\0'。
 *
 *   循环条件 size > 1 保证始终给 '\0' 留一个位置；
 *   *src != '\0' 保证复制到源串结尾就停。
 *
 * 和 strcpy 的区别:
 *   strcpy 不检查目标大小，源串过长会溢出；
 *   my_strcpy 用 size 限制复制长度，截断但安全。
 */
void my_strcpy(char *dst, char const *src, int size)
{
    /* 至少留 1 字节给 '\0'，所以最多复制 size-1 个字符 */
    while (size > 1 && *src != '\0') {
        *dst++ = *src++;   /* 复制一个字符，两指针前进 */
        size--;            /* 剩余空间减 1 */
    }

    *dst = '\0';           /* 关键: 无论是否截断，都补上结尾 '\0' */
}
