#include <stdio.h>

/*
 * 题目: 检测任意 N×N 矩阵是否为单位阵（通用版）
 *
 * 和 Q_2 的区别:
 *   Q_2: 函数固定处理 10×10 矩阵
 *   Q_3: 函数接受维度参数 n，可处理任意大小的方阵
 *
 * VLA (Variable-Length Array) 写法:
 *   identity_matrix(int n, int matrix[n][n])
 *
 *   n 必须在 matrix 之前声明，编译器才知道第二维的大小。
 */

#define TRUE  1
#define FALSE 0

int identity_matrix(int n, int matrix[n][n]);

int main(void)
{
    int i, j, n;

    /* --- 测试1: 3×3 单位阵 --- */
    {
        int mat[3][3];
        n = 3;
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                mat[i][j] = (i == j) ? 1 : 0;
        printf("Test 1: 3x3 identity        -> %s (expect true)\n",
               identity_matrix(n, mat) ? "true" : "false");
    }

    /* --- 测试2: 3×3 非单位阵 (对角线有 0) --- */
    {
        int mat[3][3];
        n = 3;
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                mat[i][j] = (i == j) ? 1 : 0;
        mat[1][1] = 0;                          /* 对角线上不是 1 */
        printf("Test 2: 3x3 diag has 0     -> %s (expect false)\n",
               identity_matrix(n, mat) ? "true" : "false");
    }

    /* --- 测试3: 3×3 非单位阵 (非对角有 1) --- */
    {
        int mat[3][3];
        n = 3;
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                mat[i][j] = (i == j) ? 1 : 0;
        mat[0][2] = 1;                          /* 非对角线上有 1 */
        printf("Test 3: 3x3 off-diag has 1 -> %s (expect false)\n",
               identity_matrix(n, mat) ? "true" : "false");
    }

    /* --- 测试4: 5×5 单位阵 --- */
    {
        int mat[5][5];
        n = 5;
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                mat[i][j] = (i == j) ? 1 : 0;
        printf("Test 4: 5x5 identity        -> %s (expect true)\n",
               identity_matrix(n, mat) ? "true" : "false");
    }

    /* --- 测试5: 1×1 单位阵 --- */
    {
        int mat[1][1];
        mat[0][0] = 1;
        printf("Test 5: 1x1 [1]             -> %s (expect true)\n",
               identity_matrix(1, mat) ? "true" : "false");
    }

    /* --- 测试6: 1×1 非单位阵 --- */
    {
        int mat[1][1];
        mat[0][0] = 0;
        printf("Test 6: 1x1 [0]             -> %s (expect false)\n",
               identity_matrix(1, mat) ? "true" : "false");
    }

    /* --- 测试7: 10×10 全零矩阵 --- */
    {
        int mat[10][10];
        n = 10;
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                mat[i][j] = 0;
        printf("Test 7: 10x10 all zeros     -> %s (expect false)\n",
               identity_matrix(n, mat) ? "true" : "false");
    }

    return 0;
}

/*
 * identity_matrix: 检测 N×N 方阵是否为单位阵
 *
 * 参数:
 *   n      — 矩阵的维度（n 行 n 列）
 *   matrix — n×n 整型二维数组 (VLA)
 *
 * 返回:
 *   TRUE  — 是单位阵
 *   FALSE — 不是单位阵
 *
 * 设计要点:
 *   参数表中 n 在 matrix 之前声明，
 *   这样 matrix[n][n] 中的 n 才在编译器的作用域内。
 *
 *   调用示例:
 *     int a[5][5];
 *     identity_matrix(5, a);    // n=5, 自动适配
 *
 *   VLA 自 C99 起支持，GCC/MinGW 均可用。
 */
int identity_matrix(int n, int matrix[n][n])
{
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {

            if (i == j) {
                /* 对角线元素必须为 1 */
                if (matrix[i][j] != 1)
                    return FALSE;
            }
            else {
                /* 非对角线元素必须为 0 */
                if (matrix[i][j] != 0)
                    return FALSE;
            }
        }
    }

    return TRUE;
}
