#include <stdio.h>
#include <stdlib.h>    /* abs() */

/*
 * 题目: 求最大公约数 (GCD — Greatest Common Divisor)
 *
 * 算法: 欧几里德辗转相除法 (递归版)
 *   gcd(m, n) = n                    (当 m % n == 0)
 *   gcd(m, n) = gcd(n, m % n)       (否则)
 */

int gcd(int m, int n);

int main(void)
{
    /* --- 正常用例 --- */
    printf("Test  1: gcd(12, 8)  = %d (expect 4)\n",  gcd(12, 8));
    printf("Test  2: gcd(48, 18) = %d (expect 6)\n",  gcd(48, 18));
    printf("Test  3: gcd(100, 25)= %d (expect 25)\n", gcd(100, 25));

    /* --- 互质（结果为 1） --- */
    printf("Test  4: gcd(7, 3)   = %d (expect 1)\n",  gcd(7, 3));
    printf("Test  5: gcd(17, 13) = %d (expect 1)\n",  gcd(17, 13));

    /* --- m < n 时自动交换 --- */
    printf("Test  6: gcd(8, 12)  = %d (expect 4)\n",  gcd(8, 12));

    /* --- 大数 --- */
    printf("Test  7: gcd(1071, 462) = %d (expect 21)\n", gcd(1071, 462));

    /* --- 两个相等 --- */
    printf("Test  8: gcd(7, 7)   = %d (expect 7)\n",  gcd(7, 7));

    /* --- 负数的处理 --- */
    printf("Test  9: gcd(-12, 8) = %d (expect 4)\n",  gcd(-12, 8));
    printf("Test 10: gcd(12, -8) = %d (expect 4)\n",  gcd(12, -8));

    /* --- n=0 除零保护 --- */
    printf("Test 11: gcd(5, 0)   = %d (expect 5)\n",  gcd(5, 0));
    printf("Test 12: gcd(0, 5)   = %d (expect 5)\n",  gcd(0, 5));

    return 0;
}

/*
 * gcd: 递归计算 m 和 n 的最大公约数
 *
 * 参数:
 *   m, n — 两个整数（支持负数，函数内部取绝对值处理）
 *
 * 返回:
 *   m 和 n 的最大公约数（正数）
 *
 * 算法:
 *   gcd(m, 0) = |m|              (0 可以被任何非零数整除，公约数就是 |m|)
 *   gcd(m, n) = gcd(n, m % n)    (辗转相除)
 *
 * 要点:
 *   1. 对 n=0 做了保护，返回 |m|，避免除零崩溃
 *   2. 函数入口统一取绝对值，确保递归过程中参数非负
 *   3. 原代码的 int r 可以省略 — 直接 return gcd(n, m % n) 即可
 */
int gcd(int m, int n)
{
    /* 统一取绝对值，处理负数输入 */
    m = abs(m);
    n = abs(n);

    /* 除零保护: gcd(m, 0) = m */
    if (n == 0)
        return m;

    /* 基线: 整除时除数就是 GCD */
    if (m % n == 0)
        return n;

    /* 递归: gcd(m, n) = gcd(n, m % n) */
    return gcd(n, m % n);
}
