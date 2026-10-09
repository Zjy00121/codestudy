#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

/* 分类函数统一接收一个 int 字符值，返回零表示不属于该类，非零表示属于。 */
typedef int (*Classifier)(int);

/*
 * ctype.h 没有直接判断“不可打印”的函数，因此对 isprint 结果取反。
 * ch 必须是可表示为 unsigned char 的值或 EOF；本程序只传实际读到的字符。
 * 返回 1 表示不可打印，0 表示可打印。
 */
static int is_nonprinting(int ch)
{
    return !isprint(ch);
}

int main(void)
{
    /*
     * 用函数指针建立分类表，不用一串 if/else 判断字符属于哪一类。
     * 未调用 setlocale，采用程序启动时的 C locale；按输入字节分类，
     * 不把 UTF-8 多字节汉字当作一个 Unicode 字符处理。
     */
    const struct {
        const char *name; // 输出用的英文分类名称
        Classifier test;  // 对应的分类函数地址
    } categories[] = {
        {"Control", iscntrl},
        {"Whitespace", isspace},
        {"Digit", isdigit},
        {"Lowercase", islower},
        {"Uppercase", isupper},
        {"Punctuation", ispunct},
        {"Non-printable", is_nonprinting}
    };
    enum { CATEGORY_COUNT = sizeof categories / sizeof categories[0] };
    size_t counts[CATEGORY_COUNT] = {0}; // 各类计数，单位是读取到的字符数
    size_t total = 0;                    // 分母：成功读取的全部字符数

    /*
     * getchar 返回 unsigned char 转成 int 的值，或者 EOF。
     * 必须用 int 接收，不能用 char 截断后再判断 EOF。
     * 检查 EOF 后，ch 可以直接安全传给 ctype 分类函数。
     */
    int ch;
    while ((ch = getchar()) != EOF) {
        if (total == SIZE_MAX) {
            fprintf(stderr, "Input is too large to count.\n");
            return EXIT_FAILURE;
        }
        total++;

        for (size_t i = 0; i < CATEGORY_COUNT; ++i) {
            /*
             * 分类结果不保证是 1，先用 != 0 规范为 0 或 1 再累加。
             * 每个字符独立检查所有类别：换行同时属于控制、空白、不可打印。
             * 每类计数不超过 total，上面的总数检查也防止各类计数溢出。
             */
            counts[i] += (size_t)(categories[i].test(ch) != 0);
        }
    }

    /* EOF 也可能表示读取失败，发生错误时不把部分统计当作完整结果。 */
    if (ferror(stdin)) {
        fprintf(stderr, "Failed to read standard input.\n");
        return EXIT_FAILURE;
    }

    printf("Total characters: %zu\n", total);
    printf("Categories overlap; percentages may sum to more than 100%%.\n");
    for (size_t i = 0; i < CATEGORY_COUNT; ++i) {
        double percentage = 0.0; // 空输入时约定输出 0.00%，避免除以零
        if (total != 0) {
            /* 转成浮点后相除，避免整数除法截断，也避免整数乘 100 溢出。 */
            percentage = (double)counts[i] / (double)total * 100.0;
        }
        printf("%-14s %zu %.2f%%\n", categories[i].name, counts[i], percentage);
    }
    return EXIT_SUCCESS;
}
