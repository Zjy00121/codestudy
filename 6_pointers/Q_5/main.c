#include <stdio.h>

/*
 * 题目: Eratosthenes 筛选法求质数 (基础版)
 *
 * 算法:
 *   1. 创建数组 sieve[0..SIZE-1]，全部初始化为 TRUE
 *   2. 0 和 1 不是质数，标记为 FALSE
 *   3. 找到第一个 TRUE 的数 p（从 2 开始），p 是质数
 *   4. 把 p 的所有倍数 (2p, 3p, 4p...) 标记为 FALSE
 *   5. 下一个 TRUE 的数就是下一个质数，重复步骤 4
 *   6. 当 p*p >= SIZE 时停止
 *   7. 遍历数组，sieve[i]==TRUE 的 i 就是质数
 *
 * 要求:
 *   a. 用 char 数组节省空间 (1 字节 vs int 的 4 字节)
 *   b. 用指针访问数组，不使用下标 []
 *   c. 一开始用 1000 个元素的数组
 */

#define SIZE   1000
#define TRUE   1
#define FALSE  0

int main(void)
{
    char sieve[SIZE];      /* 标记数组: TRUE=质数, FALSE=非质数 */
    char *p;               /* 工作指针 */
    /* 剔除倍数直接通过 step 计算偏移，无需额外指针 */
    int   step;            /* 当前质数 p，用于计算倍数偏移 */
    int   count;           /* 统计质数个数 */
    int   per_line;        /* 输出换行控制 */

    /* ===== 第1步: 全部初始化为 TRUE ===== */
    p = sieve;                      /* 指向 sieve[0] */
    while (p < sieve + SIZE) {
        *p++ = TRUE;                /* 赋 TRUE，指针后移 */
    }

    /* ===== 第2步: 0 和 1 不是质数 ===== */
    *sieve       = FALSE;           /* sieve[0] → 0 不是质数 */
    *(sieve + 1) = FALSE;           /* sieve[1] → 1 不是质数 */

    /* ===== 第3~5步: 筛选 ===== */
    step = 2;                       /* 从第一个质数 2 开始 */
    while (step * step < SIZE) {

        /* 找到下一个 TRUE 的位置 = 下一个质数 */
        /* 从这里开始，用 (sieve + step) 即 &sieve[step] 来检查 */
        while (step < SIZE && *(sieve + step) == FALSE)
            step++;

        if (step * step >= SIZE)    /* 可以提前结束 */
            break;

        /* 剔除 step 的所有倍数: 从 step*2 开始，每次加 step */
        {
            int multiple = step * 2;           /* 倍数从 2*step 开始 */
            while (multiple < SIZE) {
                *(sieve + multiple) = FALSE;   /* 标记为非质数 */
                multiple += step;              /* 下一个倍数 */
            }
        }

        step++;                     /* 检查下一个数 */
    }

    /* ===== 第6步: 输出所有质数 ===== */
    printf("Primes up to %d (basic sieve):\n", SIZE - 1);

    count    = 0;
    per_line = 0;
    p        = sieve + 2;           /* 从 2 开始遍历 */

    while (p < sieve + SIZE) {
        if (*p == TRUE) {
            /* 用 p - sieve 还原出下标，即质数本身 */
            printf("%3d ", (int)(p - sieve));
            count++;
            per_line++;
            if (per_line == 10) {    /* 每行 10 个 */
                printf("\n");
                per_line = 0;
            }
        }
        p++;
    }

    if (per_line != 0)
        printf("\n");
    printf("Total: %d primes\n", count);

    return 0;
}
