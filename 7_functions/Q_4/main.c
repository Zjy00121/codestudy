/*
 * 题目: 可变参数列表 — 找最大值
 *
 * max_list: 从可变参数列表中找出最大值
 *   n_1 — 最后一个固定参数（va_start 的参照点，其值不参与比较）
 *   ... — 可变参数，以负数（-1）作为结束哨兵
 *   sentinel: -1 标记参数列表结束
 */

#include <stdio.h>
#include <stdarg.h>

int max_list(int n_1, ...);

int main(void)
{
    /* 测试: 可变参数序列 1, 22, 44, 22, 100, -1
     * 注意: n_1(=1) 仅作为 va_start 定位参照，它的值不会参与比较
     * 实际参与比较的是 n_1 之后、-1 之前的所有参数
     * 序列 22, 44, 22, 100 中最大值 = 100 */
    int max = max_list(1, 22, 44, 22, 100, -1);
    printf("%d\n", max);          /* 期望输出 100 */
    return 0;
}

/*
 * max_list: 从以 -1 结尾的可变参数列表中找到最大值
 *
 * 参数:
 *   n_1  — 最后一个命名的固定参数，也是 va_start 的定位参照点
 *           其值仅用于 va_start，不参与最大值的比较
 *   ...  — 可变参数列表，必须是 int 类型，以负数（如 -1）结束
 *
 * 返回:
 *   列表中的最大值
 *   0 — 如果参数以负数开头（n_1 < 0），认为列表为空
 *
 * 注意:
 *   - 使用哨兵模式（sentinel = -1）标记参数结束
 *   - 必须确保可变参数列表末尾有负数，否则会越界读取
 */
int max_list(int n_1, ...)
{
    va_list var_arg;     /* 可变参数游标 */
    int max;             /* 当前找到的最大值 */
    int num;             /* 当前读取到的参数 */

    /* 若第一个参数就是负数，认为列表为空 */
    if (n_1 < 0) {
        return 0;
    }

    /* va_start: 定位到最后一个固定参数 n_1 之后（即可变参数的起点） */
    va_start(var_arg, n_1);

    /* 取出第一个可变参数，作为 max 的初始值 */
    num = va_arg(var_arg, int);
    max = num;

    /* 遍历后续参数，直到遇到负数哨兵 */
    while (num >= 0) {
        num = va_arg(var_arg, int);   /* 取下一个参数 */
        if (num > max) {
            max = num;                /* 更新最大值 */
        }
    }

    /* 清理 va_list */
    va_end(var_arg);

    return max;
}