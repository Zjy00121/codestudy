#include <stdio.h>
#include <ctype.h>

/*
 * 题目: 统计各类字符所占百分比
 *
 * 从标准输入读取字符（直到 EOF），统计以下各类字符的数量和百分比:
 *   控制字符   (iscntrl)
 *   空白字符   (isspace)
 *   数字       (isdigit)
 *   小写字母   (islower)
 *   大写字母   (isupper)
 *   标点符号   (ispunct)
 *
 * 使用 <ctype.h> 中的字符分类函数判断不可打印字符。
 *
 * 运行方式:
 *   1. 终端输入内容，结束按 Ctrl+Z (Windows) / Ctrl+D (Linux) 表示 EOF
 *   2. 或从文件重定向:  main.exe < input.txt
 */

int main(void)
{
    int c;                    /* 读入的字符（int 类型，为容纳 EOF） */

    int total   = 0;          /* 字符总数 */
    int cntrl   = 0;          /* 控制字符 */
    int space   = 0;          /* 空白字符 */
    int digit   = 0;          /* 数字 */
    int lower   = 0;          /* 小写字母 */
    int upper   = 0;          /* 大写字母 */
    int punct   = 0;          /* 标点符号 */

    /*
     * 循环读取直到 EOF。
     * 分类用 if-else 链，保证每个字符只归入一类。
     *
     * 顺序说明:
     *   isspace 先判断 — 空格/制表符/换行等空白符都归入"空白"。
     *   制表符、换行既是空白也是控制字符，这里优先归入空白。
     *   剩下的非空白控制字符（如 SOH、DEL）才归入"控制"。
     */
    while ((c = getchar()) != EOF) {
        total++;

        if (isspace(c)) {
            space++;
        }
        else if (iscntrl(c)) {
            cntrl++;
        }
        else if (isdigit(c)) {
            digit++;
        }
        else if (islower(c)) {
            lower++;
        }
        else if (isupper(c)) {
            upper++;
        }
        else if (ispunct(c)) {
            punct++;
        }
        /* 其余字符（如扩展 ASCII 高位字节）不归入上述任何一类 */
    }

    /* 输出统计结果 */
    printf("\nCharacter statistics:\n");
    printf("  Total characters : %d\n", total);

    /* 空输入保护，避免除零 */
    if (total == 0) {
        printf("  (no input)\n");
        return 0;
    }

    printf("\n  Category        Count   Percent\n");
    printf("  --------------  -----   -------\n");
    printf("  Control chars   %5d   %6.2f%%\n", cntrl, cntrl * 100.0 / total);
    printf("  Whitespace      %5d   %6.2f%%\n", space, space * 100.0 / total);
    printf("  Digits          %5d   %6.2f%%\n", digit, digit * 100.0 / total);
    printf("  Lowercase       %5d   %6.2f%%\n", lower, lower * 100.0 / total);
    printf("  Uppercase       %5d   %6.2f%%\n", upper, upper * 100.0 / total);
    printf("  Punctuation     %5d   %6.2f%%\n", punct, punct * 100.0 / total);

    return 0;
}
