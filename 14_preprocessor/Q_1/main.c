#include <stdio.h>
#include <limits.h>

void print_ledger (int x);
void print_ledger_long(int x);
void print_ledger_detailed(int x);
void print_ledger_default(int x);

int main(void)
{
    /*
     * 题目未限制 int 参数的含义，测试正数、零、负数和 int 两端极值。
     * 这些调用用于检查参数是否原样传给被选中的打印函数。
     * 具体选中哪些函数由编译选项决定，不由 x 决定。
     */
    const int values[] = {42, 0, -7, INT_MIN, INT_MAX};
    size_t count = sizeof values / sizeof values[0];
    for (size_t i = 0; i < count; ++i) {
        /* 输出输入值，便于外部测试逐项核对后续调用及顺序。 */
        printf("Input: %d\n", values[i]);
        print_ledger(values[i]);
    }
    return 0;
}

/*
 * x：原样传给选中打印函数的整数；无返回值。
 * 选项按“是否定义”判断，即使定义为 0 也启用。
 * 两个选项都定义时，先调用 long，再调用 detailed，各调用一次。
 * 两个选项都未定义时才调用 default。
 */
void print_ledger (int x) {
#ifdef OPTION_LONG
    print_ledger_long(x);
#endif

#ifdef OPTION_DETAILED
    print_ledger_detailed(x);
#endif

#if !defined(OPTION_LONG) && !defined(OPTION_DETAILED)
    print_ledger_default(x);
#endif
}

/*
 * 以下三个函数是本练习的测试替身：输出函数标识与收到的参数。
 * 题目没有提供真实报表格式，因此不实现额外的金融业务逻辑。
 * 实际项目可用同名、同接口的报表实现替换它们。
 */
void print_ledger_long(int x)
{
    printf("Long: %d\n", x);
}

void print_ledger_detailed(int x)
{
    printf("Detailed: %d\n", x);
}

void print_ledger_default(int x)
{
    printf("Default: %d\n", x);
}
