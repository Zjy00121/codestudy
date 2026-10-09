#include <stdio.h>

/*
 * 题目: 美国联邦政府 1995 年个人所得税计算（单身纳税人）
 *
 * 税率分级表:
 *   收入下限      收入上限      基础税额     超额部分税率    超额起征基数
 *   $0            $23,350      $0           15%           $0
 *   $23,350       $56,550      $3,502.50    28%           $23,350
 *   $56,550       $117,950     $12,798.50   31%           $56,550
 *   $117,950      $256,500     $31,832.50   36%           $117,950
 *   > $256,500    —            $81,710.50   39.6%         $256,500
 *
 * 计算公式:
 *   税额 = 该档基础固定税额 + (收入 - 该档超额起征基数) × 对应税率
 *
 * 函数原型:
 *   float single_tax(float income);
 */

float single_tax(float income);

int main(void)
{
    /* 测试各档边界值 */
    printf("Test  1: income=  0.00   tax=$%.2f (expect 0.00)\n",
           single_tax(0.0f));
    printf("Test  2: income=10000.00  tax=$%.2f (expect 1500.00)\n",
           single_tax(10000.0f));
    printf("Test  3: income=23350.00  tax=$%.2f (expect 3502.50)\n",
           single_tax(23350.0f));

    /* 第二档 */
    printf("Test  4: income=40000.00  tax=$%.2f (expect 8164.50)\n",
           single_tax(40000.0f));
    printf("Test  5: income=56550.00  tax=$%.2f (expect 12798.50)\n",
           single_tax(56550.0f));

    /* 第三档 */
    printf("Test  6: income=80000.00  tax=$%.2f (expect 20068.00)\n",
           single_tax(80000.0f));

    /* 第四档 */
    printf("Test  7: income=150000.00 tax=$%.2f (expect 43370.50)\n",
           single_tax(150000.0f));

    /* 第五档 */
    printf("Test  8: income=300000.00 tax=$%.2f (expect 98936.50)\n",
           single_tax(300000.0f));

    /* 边界测试 */
    printf("Test  9: income=117950.00 tax=$%.2f (expect 31832.50)\n",
           single_tax(117950.0f));
    printf("Test 10: income=256500.00 tax=$%.2f (expect 81710.50)\n",
           single_tax(256500.0f));

    return 0;
}

/*
 * single_tax: 根据 1995 年美国单身纳税人税率表计算应缴税额
 *
 * 参数:
 *   income — 应征税的个人收入（美元），负数视为 0
 *
 * 返回:
 *   需要缴纳的总税额（美元），保留两位小数
 *
 * 算法:
 *   遍历税率表数组，找到 income 对应的档次，
 *   代入公式: tax = base + (income - excess) * rate
 *
 * 设计:
 *   用四个并列数组分别存储每档的 收入上限 / 基础税额 / 税率 / 起征基数。
 *   这是数组章节的典型应用——用数组代替大量的 if-else 链。
 */
float single_tax(float income)
{
    /*
     * 税率表 — 5 个档次
     *
     * 前 4 档有明确上限，存在 in ceiling[] 中用于 if (income <= ceiling[i])。
     * 最后一档无上限（> $256,500），不在 ceiling 数组里——循环跑完自然落入。
     */
    float ceiling[] = {23350.0f, 56550.0f, 117950.0f, 256500.0f};  /* 只 4 个 */
    float base_tax[] = {0.0f, 3502.50f, 12798.50f, 31832.50f, 81710.50f};
    float rate[]     = {0.15f,  0.28f,    0.31f,    0.36f,    0.396f};
    float excess[]   = {0.0f, 23350.0f, 56550.0f, 117950.0f, 256500.0f};

    int bracket_count = sizeof(ceiling) / sizeof(ceiling[0]);
    int i;

    /* 负收入或零收入 → 无需缴税 */
    if (income <= 0.0f)
        return 0.0f;

    /* 遍历前 4 档：有明确上限，用 <= ceiling 判断 */
    for (i = 0; i < bracket_count; i++) {
        if (income <= ceiling[i]) {
            return base_tax[i] + (income - excess[i]) * rate[i];
        }
    }

    /*
     * 循环跑完都没匹配 → income 超过所有 ceiling，
     * 自然落入最后一档（index = 4，即第 5 档），该档无上限。
     */
    return base_tax[4] + (income - excess[4]) * rate[4];
}
