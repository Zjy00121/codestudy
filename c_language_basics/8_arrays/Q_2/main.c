#include <stdio.h>

/*
 * 题目: 检测 10×10 矩阵是否为单位阵
 *
 * 单位阵定义: 主对角线元素全为 1，其余元素全为 0
 *   i == j → matrix[i][j] 必须 == 1
 *   i != j → matrix[i][j] 必须 == 0
 */

#define TRUE  1
#define FALSE 0

int identity_matrix(int matrix[10][10]);

int main(void)
{
    int mat[10][10];          /* 工作矩阵 */
    int i, j;

    /* --- 测试1: 标准单位阵 --- */
    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++)
            mat[i][j] = (i == j) ? 1 : 0;
    printf("Test 1: identity matrix        -> %s (expect true)\n",
           identity_matrix(mat) ? "true" : "false");

    /* --- 测试2: 对角线上有一个不是 1 --- */
    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++)
            mat[i][j] = (i == j) ? 1 : 0;
    mat[5][5] = 0;             /* 对角线上放一个 0 */
    printf("Test 2: mat[5][5] = 0 (not 1) -> %s (expect false)\n",
           identity_matrix(mat) ? "true" : "false");

    /* --- 测试3: 非对角线有一个不是 0 --- */
    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++)
            mat[i][j] = (i == j) ? 1 : 0;
    mat[3][7] = 5;             /* 非对角线上放一个非 0 */
    printf("Test 3: mat[3][7] = 5 (not 0) -> %s (expect false)\n",
           identity_matrix(mat) ? "true" : "false");

    /* --- 测试4: 全 0 矩阵 --- */
    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++)
            mat[i][j] = 0;
    printf("Test 4: all zeros               -> %s (expect false)\n",
           identity_matrix(mat) ? "true" : "false");

    /* --- 测试5: 全 1 矩阵 --- */
    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++)
            mat[i][j] = 1;
    printf("Test 5: all ones                -> %s (expect false)\n",
           identity_matrix(mat) ? "true" : "false");

    /* --- 测试6: 只有第一个对角是 0，其余对角正确 --- */
    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++)
            mat[i][j] = (i == j) ? 1 : 0;
    mat[0][0] = 0;
    printf("Test 6: mat[0][0] = 0           -> %s (expect false)\n",
           identity_matrix(mat) ? "true" : "false");

    return 0;
}

/*
 * identity_matrix: 检测 10×10 矩阵是否为单位阵
 *
 * 参数:
 *   matrix — 10×10 整型二维数组
 *
 * 返回:
 *   TRUE  — 是单位阵（对角线全 1，其余全 0）
 *   FALSE — 不是单位阵
 *
 * 算法:
 *   双重循环遍历 10×10 矩阵，
 *   对角线 (i==j) 检查是否 == 1，
 *   非对角线 (i!=j) 检查是否 == 0，
 *   任一不符立即返回 FALSE；全部通过返回 TRUE。
 */
int identity_matrix(int matrix[10][10])
{
    int i, j;

    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {

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

    return TRUE;   /* 所有 100 个元素都通过了检查 */
}
