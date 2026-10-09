/*
 * 把一个值存储到一个整数中指定的几个位，返回结果。
 *
 * original_value — 原始值（其中的目标位区域会被替换）
 * value_to_store  — 要存入的值（如果超出目标区域宽度，截取低位）
 * starting_bit    — 起始位（高位，编号从右至左）
 * ending_bit      — 结束位（低位）
 *
 * 例如：把 0x123 存到 0xFFFF 的位 15~4 → 返回 0x123F
 */

#include <stdio.h>

int store_bit_field(int original_value, int value_to_store,
    unsigned starting_bit, unsigned ending_bit);

int main(void) {
    int result;

    /* test 1: 原始 0x0, 存 0x1 到位 4~4, 期望 0x10 */
    result = store_bit_field(0x0, 0x1, 4, 4);
    printf("test 1: 0x%X (expect 0x10)\n", result);

    /* test 2: 原始 0xFFFF, 存 0x123 到位 15~4, 期望 0x123F */
    result = store_bit_field(0xFFFF, 0x123, 15, 4);
    printf("test 2: 0x%X (expect 0x123F)\n", result);

    /* test 3: 原始 0xFFFF, 存 0x123 到位 13~9, 期望 0xC7FF */
    result = store_bit_field(0xFFFF, 0x123, 13, 9);
    printf("test 3: 0x%X (expect 0xC7FF)\n", result);

    return 0;
}

int store_bit_field(int original_value, int value_to_store,
    unsigned starting_bit, unsigned ending_bit) {
    unsigned width;      /* 目标区域宽度（位个数） */
    unsigned mask;       /* 目标区域的掩码 */

    width = starting_bit - ending_bit + 1;

    /* 构建掩码：width 个 1 左移 ending_bit 位 */
    mask = 0;
    for (unsigned i = 0; i < width; i++) {
        mask <<= 1;          /* 先左移腾位       */
        mask |= 1;           /* 最低位置 1        */
    }
    for (unsigned i = 0; i < ending_bit; i++)
        mask <<= 1;          /* 移到目标位置      */

    /* 清除原始值中的目标区域 */
    original_value &= ~mask;

    /* 把要存的值移到目标位置，截去多余位 */
    value_to_store <<= ending_bit;
    value_to_store &= mask;

    /* 合并 */
    return value_to_store | original_value;
}