#include <stdio.h>

/*
 * 题目: 矩阵乘法（通用维度，指针实现）
 *
 * C = A × B
 *   A: x 行 y 列  (m1)
 *   B: y 行 z 列  (m2)
 *   C: x 行 z 列  (r)
 *
 * 要求: 用指针而非下标遍历矩阵元素
 */

void matrix_multiply(int *m1, int *m2, int *r, int x, int y, int z);

int main(void)
{
    int c[12];                  /* 结果矩阵，3×3=9 足够 */
    int x, y, z;
    int i, j;

    /* --- 测试1: 1×2 乘 2×1 = 1×1 --- */
    {
        /* A = [1 2]   B = [3   C = [1*3 + 2*4] = [11]
         *                  4]  */
        int A[] = {1, 2};
        int B[] = {3, 4};
        x = 1; y = 2; z = 1;
        matrix_multiply(A, B, c, x, y, z);
        printf("Test 1: [1 2] * [3 4]^T = [%d] (expect 11)\n", c[0]);
    }

    /* --- 测试2: 2×3 乘 3×2 = 2×2 --- */
    {
        int A[] = {1, 2, 3,   4, 5, 6};           /* 2×3 */
        int B[] = {7, 8,   9, 10,   11, 12};       /* 3×2 */
        x = 2; y = 3; z = 2;
        matrix_multiply(A, B, c, x, y, z);
        /*
         * C[0][0] = 1*7 + 2*9 + 3*11 = 58
         * C[0][1] = 1*8 + 2*10 + 3*12 = 64
         * C[1][0] = 4*7 + 5*9 + 6*11 = 139
         * C[1][1] = 4*8 + 5*10 + 6*12 = 154
         */
        printf("Test 2: 2x3 * 3x2:\n");
        printf("  [ %3d %3d ]  (expect  58  64)\n", c[0], c[1]);
        printf("  [ %3d %3d ]  (expect 139 154)\n", c[2], c[3]);
    }

    /* --- 测试3: 2×2 乘 2×2 单位阵 = 原矩阵 --- */
    {
        int A[] = {3, 5,   7, 11};                  /* 2×2 */
        int B[] = {1, 0,   0, 1};                   /* 2×2 单位阵 */
        x = 2; y = 2; z = 2;
        matrix_multiply(A, B, c, x, y, z);
        printf("Test 3: A * I = A:\n");
        printf("  [ %2d %2d ]  (expect  3  5)\n", c[0], c[1]);
        printf("  [ %2d %2d ]  (expect  7 11)\n", c[2], c[3]);
    }

    /* --- 测试4: 2×2 乘 2×2 零矩阵 = 零矩阵 --- */
    {
        int A[] = {3, 5,   7, 11};
        int B[] = {0, 0,   0, 0};
        x = 2; y = 2; z = 2;
        matrix_multiply(A, B, c, x, y, z);
        printf("Test 4: A * 0 = 0:\n");
        printf("  [ %2d %2d ]  (expect  0  0)\n", c[0], c[1]);
        printf("  [ %2d %2d ]  (expect  0  0)\n", c[2], c[3]);
    }

    /* --- 测试5: 3×3 乘 3×3 = 3×3 --- */
    {
        int A[] = {1,2,3,  4,5,6,  7,8,9};          /* 3×3 */
        int B[] = {1,0,0,  0,1,0,  0,0,1};          /* 3×3 单位阵 */
        x = 3; y = 3; z = 3;
        matrix_multiply(A, B, c, x, y, z);
        printf("Test 5: 3x3 * I:\n");
        for (i = 0; i < 3; i++) {
            printf("  [");
            for (j = 0; j < 3; j++)
                printf(" %2d", c[i * 3 + j]);
            printf(" ]\n");
        }
        printf("  (expect rows: 1 2 3 / 4 5 6 / 7 8 9)\n");
    }

    return 0;
}

/*
 * matrix_multiply: 矩阵乘法 C = A × B
 *
 * 参数:
 *   m1 — 矩阵 A（x 行 y 列），按行存储在 1D 数组中
 *   m2 — 矩阵 B（y 行 z 列），按行存储在 1D 数组中
 *   r  — 结果矩阵 C（x 行 z 列），调用方负责分配
 *   x  — A 的行数
 *   y  — A 的列数 = B 的行数
 *   z  — B 的列数
 *
 * 算法:
 *   三重循环
 *     外层 i: A 的行
 *     中层 j: B 的列
 *     内层 k: 累加 A[i][k] * B[k][j]
 *
 * 指针运动:
 *   mp1 = m1 + i*y      → 指向 A 第 i 行开头
 *     mp1++             → 右移一列（A[i][k] → A[i][k+1]）
 *   mp2 = m2 + j        → 指向 B 第 0 行第 j 列
 *     mp2 += z          → 下移一行（B[k][j] → B[k+1][j]）
 */
void matrix_multiply(int *m1, int *m2, int *r, int x, int y, int z)
{
    int *mp1;          /* A 的游标 */
    int *mp2;          /* B 的游标 */
    int  i, j, k;      /* 循环变量 */

    for (i = 0; i < x; i++) {               /* A 的每一行 */
        for (j = 0; j < z; j++) {           /* B 的每一列 */

            /* 定位到 A[i][0] 和 B[0][j] */
            mp1 = m1 + i * y;               /* 指向 A 第 i 行开头 */
            mp2 = m2 + j;                   /* 指向 B 第 0 行第 j 列 */

            *r = 0;                         /* 清零累加器 */

            for (k = 0; k < y; k++) {       /* 内积求和 */
                *r += *mp1 * *mp2;
                mp1++;                       /* 右移: A[i][k] → A[i][k+1] */
                mp2 += z;                    /* 下移: B[k][j] → B[k+1][j] */
            }

            r++;                             /* C 的当前元素完成，指针前进 */
        }
    }
}
