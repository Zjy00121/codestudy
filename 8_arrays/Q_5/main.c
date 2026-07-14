#include <stdio.h>
#include <stdlib.h>    /* abs() */

/*
 * 题目: 八皇后问题
 *
 * 在 8×8 棋盘上放置 8 个皇后，要求任意两个皇后
 * 不在同一行、同一列、同一对角线上。
 * 找出并打印所有合法摆放方案。
 *
 * 数据表示:
 *   pos[col] = row  → 第 col 列的皇后放在第 row 行
 *   用一维数组，每列一个皇后，天然保证列不冲突。
 *
 * 冲突判断:
 *   同行:   row == pos[prev]
 *   对角线: abs(col - prev) == abs(row - pos[prev])
 *
 * 算法: 回溯法 (Backtracking)
 *   逐列放置皇后，若当前列找不到合法位置，
 *   则回溯到上一列更换行位置，继续尝试。
 */

#define N     8             /* 棋盘大小 */
#define TRUE  1
#define FALSE 0

int  pos[N];                /* 全局数组: pos[col] = row */
int  solution_count = 0;    /* 解计数器 */

/* 函数声明 */
int  is_safe(int row, int col);
void try_place(int col);
void print_solution(void);

int main(void)
{
    printf("Eight Queens Puzzle (%dx%d)\n", N, N);
    printf("Searching for all solutions...\n\n");

    try_place(0);           /* 从第 0 列开始放置 */

    printf("Total solutions: %d\n", solution_count);
    return 0;
}

/*
 * is_safe: 检查在 (row, col) 放置皇后是否安全
 *
 * 参数:
 *   row — 候选行号 (0 ~ N-1)
 *   col — 当前列号
 *
 * 返回:
 *   TRUE  — 不冲突，可以放置
 *   FALSE — 与前面某个皇后冲突
 *
 * 算法:
 *   遍历 0 到 col-1 列已放置的皇后，
 *   检查同行冲突和对角线冲突。
 *   col 后面的列还没放，不需要检查。
 */
int is_safe(int row, int col)
{
    int prev;

    for (prev = 0; prev < col; prev++) {

        /* 同行冲突: 之前有皇后在同一行 */
        if (pos[prev] == row)
            return FALSE;

        /* 对角线冲突: 列差 == 行差 */
        if (abs(col - prev) == abs(row - pos[prev]))
            return FALSE;
    }

    return TRUE;   /* 和前面所有皇后都不冲突 */
}

/*
 * try_place: 回溯主函数 — 尝试在第 col 列放置皇后
 *
 * 参数:
 *   col — 当前要放置的列号 (0 ~ N-1)
 *
 * 算法:
 *   递归基线: col == N → 所有列都已放好 → 打印解并返回
 *   否则: 在当前列尝试每一行 row = 0..N-1
 *     若 is_safe(row, col) 为真 → 放下皇后 → 递归到下一列
 *     若不行 → 循环换下一行（回溯）
 *
 *   注意: pos[col] 赋值后无需显式"撤销"——
 *   下一次循环 row++ 自然覆盖。
 */
void try_place(int col)
{
    int row;

    /* 基线: 8 列全部放置成功 → 找到一个解 */
    if (col == N) {
        solution_count++;
        print_solution();
        return;
    }

    /* 在当前列尝试每一行 */
    for (row = 0; row < N; row++) {

        if (is_safe(row, col)) {
            pos[col] = row;                /* 放下皇后 */
            try_place(col + 1);            /* 去下一列 */
            /* 回溯在这里自动发生: 下一轮循环 row++ 即换行 */
        }
    }
}

/*
 * print_solution: 打印当前棋盘布局
 *
 * 格式:
 *   每行对应棋盘的一行，
 *   Q 表示皇后，. 表示空格。
 */
void print_solution(void)
{
    int row, col;

    printf("Solution #%d:\n", solution_count);

    for (row = 0; row < N; row++) {

        printf("  ");                    /* 左侧缩进 */

        for (col = 0; col < N; col++) {

            if (pos[col] == row)
                printf(" Q");            /* 该位置有皇后 */
            else
                printf(" .");            /* 空位 */
        }

        printf("\n");
    }

    printf("\n");
}
