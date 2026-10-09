#include <stdio.h>
#include <string.h>

/*
 * 题目: 编写 my_strcpy_end
 *
 * 用来取代 strcpy，区别在于返回值:
 *   strcpy        → 返回指向目标字符串起始位置的指针
 *   my_strcpy_end → 返回指向目标字符串末尾 '\0' 的指针
 *
 * 返回末尾指针的好处:
 *   连续拼接多个字符串时，不需要每次都从头扫描目标串，
 *   直接从上一次的末尾继续写，效率更高。
 *
 * 函数原型:
 *   char *my_strcpy_end(char *dst, char const *src);
 */

char *my_strcpy_end(char *dst, char const *src);

int main(void)
{
    char buf[100];
    char *end;

    /* --- 测试1: 基本复制，返回指向 '\0' --- */
    {
        end = my_strcpy_end(buf, "hello");
        printf("Test 1: copy \"hello\"\n");
        printf("  buf = \"%s\" (expect \"hello\")\n", buf);
        printf("  *end = %d (expect 0, 即 '\\0')\n\n", *end);
    }

    /* --- 测试2: 返回指针确实指向末尾 '\0' --- */
    {
        my_strcpy_end(buf, "abc");
        end = my_strcpy_end(buf, "abcdefg");   /* 重新覆盖 */
        printf("Test 2: copy \"abcdefg\"\n");
        printf("  buf = \"%s\"\n", buf);
        printf("  end - buf = %d (expect 7 = strlen)\n\n", (int)(end - buf));
    }

    /* --- 测试3: 空字符串 --- */
    {
        end = my_strcpy_end(buf, "");
        printf("Test 3: copy \"\"\n");
        printf("  buf = \"%s\" (expect \"\")\n", buf);
        printf("  *end = %d (expect 0)\n\n", *end);
    }

    /* --- 测试4: 连续拼接（返回末尾指针的核心用途） --- */
    {
        char *p = buf;
        p = my_strcpy_end(p, "Hello");
        p = my_strcpy_end(p, " ");
        p = my_strcpy_end(p, "World");
        p = my_strcpy_end(p, "!");
        printf("Test 4: chain \"Hello\" + \" \" + \"World\" + \"!\"\n");
        printf("  buf = \"%s\" (expect \"Hello World!\")\n", buf);
        printf("  p points to '\\0', *p = %d\n\n", *p);
    }

    /* --- 测试5: 对比 strlen 验证末尾位置 --- */
    {
        end = my_strcpy_end(buf, "test string");
        printf("Test 5: copy \"test string\"\n");
        printf("  strlen(buf) = %d, end-buf = %d (应相等)\n",
               (int)strlen(buf), (int)(end - buf));
    }

    return 0;
}

/*
 * my_strcpy_end: 复制字符串，返回指向末尾 '\0' 的指针
 *
 * 参数:
 *   dst — 目标缓冲区
 *   src — 源字符串（只读）
 *
 * 返回:
 *   指向 dst 字符串末尾 '\0' 的指针
 *
 * 算法:
 *   逐字符复制 src 到 dst（不含 '\0'），
 *   补上 '\0'，返回指向该 '\0' 的指针。
 *
 * 和 strcpy 的区别:
 *   strcpy 返回起始指针（调用方还得再 strlen 一次才能知道末尾在哪）；
 *   my_strcpy_end 直接返回末尾指针，方便连续拼接。
 */
char *my_strcpy_end(char *dst, char const *src)
{
    while (*src != '\0') {
        *dst++ = *src++;   /* 复制一个字符，两指针前进 */
    }

    *dst = '\0';           /* 补上结尾 NUL */
    return dst;            /* 返回指向该 NUL 的指针 */
}
