#include <stdio.h>
#include <stdarg.h>

/*
 * 题目: 实现简化版 printf
 *
 * 支持的格式码: %d, %f, %s, %c
 * 其他格式码行为未定义（符合 ANSI 标准精神）
 */

/* 前置声明 */
void my_printf(char *format, ...);
void print_integer(int n);
void print_float(double f);

int main(void)
{
    /* 测试 %d */
    my_printf("--- Test %%d ---\n");
    my_printf("positive: %d\n", 42);
    my_printf("negative: %d\n", -99);
    my_printf("zero:     %d\n", 0);

    /* 测试 %f */
    my_printf("\n--- Test %%f ---\n");
    my_printf("pi:       %f\n", 3.141593);
    my_printf("negative: %f\n", -2.500000);
    my_printf("zero:     %f\n", 0.0);

    /* 测试 %s */
    my_printf("\n--- Test %%s ---\n");
    my_printf("hello:    %s\n", "Hello, World!");
    my_printf("empty:    %s\n", "");

    /* 测试 %c */
    my_printf("\n--- Test %%c ---\n");
    my_printf("char:     %c\n", 'A');
    my_printf("digit:    %c\n", '7');

    /* 混合测试 */
    my_printf("\n--- Mixed ---\n");
    my_printf("int=%d, float=%f, str=%s, char=%c\n", 10, 1.23, "hi", 'X');

    /* %% 测试 (两个%输出一个%) */
    my_printf("\n--- Percent sign ---\n");
    my_printf("99%% done\n");

    return 0;
}

/*
 * my_printf: 简化版 printf，支持 %d %f %s %c
 *
 * 参数:
 *   format — 格式字符串
 *   ...    — 可变参数，类型需与格式码匹配
 *
 * 算法:
 *   遍历格式字符串:
 *     普通字符 → 直接 putchar
 *     %d → 取 int     → print_integer
 *     %f → 取 double  → print_float
 *     %s → 取 char *  → 逐字符 putchar
 *     %c → 取 int     → putchar (char 会被提升为 int)
 *     %% → 输出 '%'
 *     其他 %X → 未定义行为，这里选择直接输出字符
 */
void my_printf(char *format, ...)
{
    va_list args;                /* 可变参数游标 */
    char   *ptr;                 /* 遍历格式字符串的工作指针 */
    char   *str;                 /* %s 对应的字符串 */

    va_start(args, format);

    for (ptr = format; *ptr != '\0'; ptr++) {

        /* 非 % 普通字符 → 直接输出 */
        if (*ptr != '%') {
            putchar(*ptr);
            continue;
        }

        /* 当前字符是 '%'，看下一个字符决定动作 */
        ptr++;                    /* 跳过 '%'，指向格式码 */

        switch (*ptr) {

        case 'd':                 /* 十进制整数 */
            print_integer(va_arg(args, int));
            break;

        case 'f':                 /* 浮点数 (float 提升为 double) */
            print_float(va_arg(args, double));
            break;

        case 's':                 /* 字符串 */
            str = va_arg(args, char *);
            while (*str != '\0') {
                putchar(*str++);
            }
            break;

        case 'c':                 /* 字符 (char 提升为 int) */
            putchar((char)va_arg(args, int));
            break;

        case '%':                 /* 转义的 % */
            putchar('%');
            break;

        case '\0':                /* 格式串在 % 后结束 → 直接返回 */
            va_end(args);
            return;

        default:                  /* 未知格式码 → 未定义行为，原样输出 */
            putchar('%');
            putchar(*ptr);
            break;
        }
    }

    va_end(args);
}

/*
 * print_integer: 用 putchar 打印整数
 *
 * 参数: n — 待打印的整数
 *
 * 算法:
 *   - 负数 → 先输出 '-', 转正处理
 *   - 递归/逆序提取每一位数字，转为字符后 putchar
 */
void print_integer(int n)
{
    /* 临时数组: 存放逆序的数字字符（足够放下最小的 int） */
    char buf[12];                /* int 最多 11 位（含符号），+1 放 '\0' */
    int  i = 0;
    int  is_negative = 0;

    /* 处理 0 的特殊情况 */
    if (n == 0) {
        putchar('0');
        return;
    }

    /* 处理负数 */
    if (n < 0) {
        is_negative = 1;
        n = -n;                  /* 转为正数 */
    }

    /* 从低位到高位逐位提取，存入 buf（逆序） */
    while (n > 0) {
        buf[i++] = (char)('0' + (n % 10));   /* 取个位 → 字符 */
        n /= 10;                              /* 去掉个位 */
    }

    /* 输出负号 */
    if (is_negative)
        putchar('-');

    /* 逆序输出 buf 中的数字（即恢复为高位在前） */
    while (--i >= 0)
        putchar(buf[i]);
}

/*
 * print_float: 用 putchar 打印浮点数（固定 6 位小数）
 *
 * 参数: f — 待打印的浮点数
 *
 * 算法:
 *   1. 打印整数部分（复用 print_integer）
 *   2. 打印小数点
 *   3. 取小数部分，逐位打印（固定 6 位）
 */
void print_float(double f)
{
    int  int_part;               /* 整数部分 */
    double frac_part;            /* 小数部分 */
    int  digit;

    /* 处理负数 */
    if (f < 0) {
        putchar('-');
        f = -f;
    }

    /* 分离整数和小数部分 */
    int_part  = (int)f;
    frac_part = f - int_part;

    /* 打印整数部分 */
    print_integer(int_part);

    /* 打印小数点 */
    putchar('.');

    /* 打印小数部分（固定 6 位） */
    for (int i = 0; i < 6; i++) {
        frac_part *= 10;
        digit = (int)frac_part;              /* 取出最高位 */
        putchar((char)('0' + digit));
        frac_part -= digit;                  /* 去掉已输出的位 */
    }
}
