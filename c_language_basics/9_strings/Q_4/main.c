#include <stdio.h>
#include <string.h>

/*
 * 题目: 编写 my_strcat
 *
 * 类似 strcat，但不会溢出目标数组，且结果一定是真正的字符串
 * （一定以 '\0' 结尾）。
 *
 * 函数原型:
 *   void my_strcat(char *dst, char const *src, int size);
 *
 * 参数:
 *   dst  — 目标缓冲区（必须已经是合法字符串）
 *   src  — 要追加的字符串
 *   size — 目标缓冲区的总容量（字节数）
 *
 * 行为:
 *   在 dst 末尾追加 src，最多填满缓冲区（留 1 字节给 '\0'），
 *   源串过长时截断，但结果始终以 '\0' 结尾，绝不溢出。
 */

void my_strcat(char *dst, char const *src, int size);

int main(void)
{
    /* --- 测试1: 正常追加 --- */
    {
        char dst[20] = "hello";
        my_strcat(dst, " world", sizeof(dst));
        printf("Test 1: \"hello\" + \" world\"\n");
        printf("  -> \"%s\" (expect \"hello world\")\n\n", dst);
    }

    /* --- 测试2: 追加后溢出，截断 --- */
    {
        char dst[8] = "hello";                     /* 剩 3 字节：2字符+'\0' */
        my_strcat(dst, " world!", sizeof(dst));
        printf("Test 2: \"hello\" + \" world!\" into 8-byte buf\n");
        printf("  -> \"%s\" (expect \"hello w\", truncated)\n\n", dst);
    }

    /* --- 测试3: 追加到空串 --- */
    {
        char dst[10] = "";
        my_strcat(dst, "hi", sizeof(dst));
        printf("Test 3: \"\" + \"hi\"\n");
        printf("  -> \"%s\" (expect \"hi\")\n\n", dst);
    }

    /* --- 测试4: 追加空串，原串不变 --- */
    {
        char dst[10] = "abc";
        my_strcat(dst, "", sizeof(dst));
        printf("Test 4: \"abc\" + \"\"\n");
        printf("  -> \"%s\" (expect \"abc\")\n\n", dst);
    }

    /* --- 测试5: dst 已满，追加不进 --- */
    {
        char dst[4] = "abc";                       /* 只剩 '\0' 的位置 */
        my_strcat(dst, "xyz", sizeof(dst));
        printf("Test 5: \"abc\" + \"xyz\" into 4-byte buf\n");
        printf("  -> \"%s\" (expect \"abc\", no room)\n\n", dst);
    }

    /* --- 测试6: 恰好放下 --- */
    {
        char dst[10] = "ab";
        my_strcat(dst, "cd", sizeof(dst));
        printf("Test 6: \"ab\" + \"cd\"\n");
        printf("  -> \"%s\" (expect \"abcd\")\n\n", dst);
    }

    return 0;
}

/*
 * my_strcat: 安全地追加字符串，绝不溢出目标缓冲区
 *
 * 参数:
 *   dst  — 目标缓冲区（必须已是合法字符串）
 *   src  — 要追加的字符串（只读）
 *   size — 目标缓冲区总容量（字节数）
 *
 * 算法:
 *   1. 找到 dst 末尾的 '\0'，同时扣减已占用的空间
 *   2. 从该位置开始追加 src，最多加到剩 1 字节给 '\0'
 *   3. 补上结尾 '\0'
 *
 * 和 strcat 的区别:
 *   strcat 不检查目标大小，追加过长会溢出；
 *   my_strcat 用 size 限制，截断但安全。
 */
void my_strcat(char *dst, char const *src, int size)
{
    /* 第1步: 走到 dst 末尾的 '\0'，每走一步扣减剩余空间 */
    while (size > 0 && *dst != '\0') {
        dst++;      /* 停在 '\0' 本身，而不是 '\0' 之后 */
        size--;     /* 已占用的字节不再属于可用空间 */
    }

    /* 第2步: 追加 src，size > 1 保证始终给 '\0' 留位置 */
    while (size > 1 && *src != '\0') {
        *dst++ = *src++;   /* 这里是赋值 =，不是比较 == */
        size--;
    }

    /* 第3步: 补上结尾 '\0' */
    *dst = '\0';
}
