#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * 功能：寻找年龄在某基数下表示不大于 29 的最小基数。
 * age：已经验证的非负十进制年龄。
 * 返回值：找到时返回 2～36；范围内无解时返回 0。
 */
static int find_minimum_base(long age)
{
    /* 从小到大逐个尝试，首次满足条件的基数就是最小值。 */
    for (int base = 2; base <= 36; ++base)
    {
        /*
         * 除法得到去掉最低位后的部分，余数得到最低位。
         * 余数 10～35 通常写成 A～Z，比较时直接使用数值。
         * 不做 age * 10 之类的乘法，避免引入整数溢出。
         */
        long quotient = age / base;
        long remainder = age % base;

        /* 商不小于基数意味着至少三位，必然大于两位的 29。 */
        if (quotient >= base)
        {
            continue;
        }

        /*
         * 单位数的商为 0，必然小于两位的 29。
         * 两位数的最高位为 1 时也小于 29；为 2 时最低位须 <= 9。
         * 例如 41 / 16 商为 2、余数为 9，表示为十六进制 29。
         */
        if (quotient < 2 || (quotient == 2 && remainder <= 9))
        {
            return base;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    /* argv[0] 是程序名；本题要求恰好一个年龄参数。 */
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <decimal-age>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /*
     * 本题接受一串十进制数字，允许前导零，年龄 0 也作为合法输入。
     * 先检查字符，拒绝空串、负号、空白、小数和其他文字。
     */
    const char *text = argv[1];
    if (*text == '\0')
    {
        fprintf(stderr, "Age must contain decimal digits.\n");
        return EXIT_FAILURE;
    }
    for (const char *cursor = text; *cursor != '\0'; ++cursor)
    {
        if (*cursor < '0' || *cursor > '9')
        {
            fprintf(stderr, "Age must contain decimal digits only.\n");
            return EXIT_FAILURE;
        }
    }

    /*
     * 清除旧 errno，再按十进制转换。
     * end 指向原参数字符串内部，不分配内存，也不需要释放。
     */
    char *end = NULL;
    errno = 0;
    long age = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0')
    {
        fprintf(stderr, "Age is outside the supported integer range.\n");
        return EXIT_FAILURE;
    }

    int base = find_minimum_base(age);
    if (base == 0)
    {
        fprintf(stderr, "No base from 2 to 36 represents this age as at most 29.\n");
        return EXIT_FAILURE;
    }

    /* 按题意输出找到的最小基数。 */
    printf("%d\n", base);
    return EXIT_SUCCESS;
}
