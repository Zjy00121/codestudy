#include <stdio.h>

/*
 * 题目: 计算厄密多项式 (Hermite Polynomials)
 *
 * 定义:
 *   H_0(x) = 1
 *   H_1(x) = 2x
 *   H_n(x) = 2x * H_{n-1}(x) - 2(n-1) * H_{n-2}(x)   (n >= 2)
 *
 * 要求:
 *   用递归实现，函数原型 int hermite(int n, int x);
 */

int hermite(int n, int x);

int main(void)
{
    /* 测试1: 题目给的用例 n=3, x=2 */
    printf("Test 1: H_3(2) = %d (expect 40)\n", hermite(3, 2));

    /* 测试2: n=0 基线 */
    printf("Test 2: H_0(5) = %d (expect 1)\n", hermite(0, 5));

    /* 测试3: n=1 基线 */
    printf("Test 3: H_1(3) = %d (expect 6)\n", hermite(1, 3));

    /* 测试4: n=2 */
    printf("Test 4: H_2(1) = %d (expect 2)\n", hermite(2, 1));

    /* 测试5: n=4 多层递归 */
    printf("Test 5: H_4(1) = %d (expect -20)\n", hermite(4, 1));

    /* 测试6: n=5, x=1 */
    printf("Test 6: H_5(1) = %d (expect -8)\n", hermite(5, 1));

    /* 测试7: x=0 时的行为 */
    printf("Test 7: H_4(0) = %d (expect 12)\n", hermite(4, 0));

    /* 测试8: 大 n 测试递归深度 */
    printf("Test 8: H_10(1) = %d\n", hermite(10, 1));

    return 0;
}

/*
 * hermite: 递归计算第 n 阶厄密多项式在 x 处的值
 *
 * 参数:
 *   n — 多项式阶数 (n >= 0)
 *   x — 自变量
 *
 * 返回:
 *   H_n(x) 的值
 *
 * 递推公式:
 *   H_0(x) = 1
 *   H_1(x) = 2x
 *   H_n(x) = 2x * H_{n-1}(x) - 2(n-1) * H_{n-2}(x)
 *
 * 注意:
 *   - n < 0 也返回 1（防御性处理）
 *   - 使用 else if 链，避免最后一个 if 后编译器误报 "无返回值"
 */
int hermite(int n, int x)
{
    /* 基线条件1: H_0(x) = 1，负 n 也归到此处作为保护 */
    if (n <= 0) {
        return 1;
    }
    /* 基线条件2: H_1(x) = 2x */
    else if (n == 1) {
        return 2 * x;
    }
    /* 递归条件: n >= 2 */
    else {
        return 2 * x * hermite(n - 1, x)
             - 2 * (n - 1) * hermite(n - 2, x);
    }
}
