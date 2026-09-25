#include <stdio.h>
#include <string.h>

/*
 * 题目: 编写 my_strrchr
 *
 * 类似 strchr，但返回 ch 在字符串中最后一次出现（最右边）的位置。
 *
 * 函数原型:
 *   char *my_strrchr(char const *str, int ch);
 *
 * 返回:
 *   指向 ch 最后一次出现位置的指针
 *   找不到时返回 NULL
 */

char *my_strrchr(char const *str, int ch);

int main(void)
{
    char *p;

    /* --- 测试1: 多个相同字符，返回最右边 --- */
    {
        p = my_strrchr("hello", 'l');
        printf("Test 1: my_strrchr(\"hello\", 'l')\n");
        if (p) printf("  -> \"%s\" (expect \"lo\")\n\n", p);
        else   printf("  -> NULL\n\n");
    }

    /* --- 测试2: 单次出现 --- */
    {
        p = my_strrchr("hello", 'h');
        printf("Test 2: my_strrchr(\"hello\", 'h')\n");
        if (p) printf("  -> \"%s\" (expect \"hello\")\n\n", p);
        else   printf("  -> NULL\n\n");
    }

    /* --- 测试3: 找不到 → NULL（原 bug 在此暴露） --- */
    {
        p = my_strrchr("hello", 'z');
        printf("Test 3: my_strrchr(\"hello\", 'z')\n");
        if (p) printf("  -> \"%s\"\n\n", p);
        else   printf("  -> NULL (expect NULL)\n\n");
    }

    /* --- 测试4: 目标是最后一个字符 --- */
    {
        p = my_strrchr("hello", 'o');
        printf("Test 4: my_strrchr(\"hello\", 'o')\n");
        if (p) printf("  -> \"%s\" (expect \"o\")\n\n", p);
        else   printf("  -> NULL\n\n");
    }

    /* --- 测试5: 空字符串 --- */
    {
        p = my_strrchr("", 'a');
        printf("Test 5: my_strrchr(\"\", 'a')\n");
        if (p) printf("  -> \"%s\"\n\n", p);
        else   printf("  -> NULL (expect NULL)\n\n");
    }

    /* --- 测试6: 句子中的空格 --- */
    {
        p = my_strrchr("one two three", ' ');
        printf("Test 6: my_strrchr(\"one two three\", ' ')\n");
        if (p) printf("  -> \"%s\" (expect \" three\")\n\n", p);
        else   printf("  -> NULL\n\n");
    }

    return 0;
}

/*
 * my_strrchr: 查找字符 ch 最后一次出现的位置
 *
 * 参数:
 *   str — 被搜索的字符串（只读）
 *   ch  — 要查找的字符
 *
 * 返回:
 *   指向 ch 最后一次出现位置的指针；未找到返回 NULL
 *
 * 算法:
 *   从左到右扫描整个字符串，
 *   每次匹配到 ch 就更新 ch_end 记录当前位置，
 *   扫描结束后 ch_end 保存的就是最后一次匹配的位置。
 *
 *   关键: ch_end 必须初始化为 NULL，
 *         否则 ch 不存在时返回的是未初始化的垃圾指针。
 */
char *my_strrchr(char const *str, int ch)
{
    char const *ch_end = NULL;   /* 初始化为 NULL，未找到时返回 NULL */

    while (*str != '\0') {
        if (*str == ch)
            ch_end = str;        /* 每次匹配都更新，最后留下的是最后一次 */
        str++;
    }

    return (char *)ch_end;       /* 强转去掉 const，匹配接口约定 */
}
