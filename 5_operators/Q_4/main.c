/*
 * 位数组操作函数。
 *
 * bit_array[] — 字符数组，每个字节存储 8 个位。
 * bit_number   — 要访问的位的编号（从 0 开始）。
 *
 * 例如 bit_number = 13：位于 bit_array[1] 的第 5 位 (13/8=1, 13%8=5)
 *
 *  set_bit:  把指定位设为 1
 *  clear_bit: 把指定位清为 0
 *  assign_bit: value=0 则清零，否则置 1
 *  test_bit:  指定位为 1 返回非 0，否则返回 0
 */

#include <stdio.h>

void set_bit(char bit_array[], unsigned bit_number);
void clear_bit(char bit_array[], unsigned bit_number);
void assign_bit(char bit_array[], unsigned bit_number, int value);
int test_bit(char bit_array[], unsigned bit_number);

int main(void) {
    /* 两个字节的位数组，全部初始化为 0 */
    char bit_array[2] = {0};
    int result;

    /* 测试 set_bit：把位 13（第 2 字节第 5 位）置 1 */
    printf("--- set_bit(13) ---\n");
    set_bit(bit_array, 13);
    result = test_bit(bit_array, 13);
    printf("bit 13 = %d (expect 1)\n", result);

    /* 测试 clear_bit */
    printf("--- clear_bit(13) ---\n");
    clear_bit(bit_array, 13);
    result = test_bit(bit_array, 13);
    printf("bit 13 = %d (expect 0)\n", result);

    /* 测试 assign_bit */
    printf("--- assign_bit(5, value=1) ---\n");
    assign_bit(bit_array, 5, 1);
    printf("bit 5 = %d (expect 1)\n", test_bit(bit_array, 5));

    printf("--- assign_bit(5, value=0) ---\n");
    assign_bit(bit_array, 5, 0);
    printf("bit 5 = %d (expect 0)\n", test_bit(bit_array, 5));

    /* 测试多个位同时存在 */
    printf("--- multiple bits ---\n");
    set_bit(bit_array, 0);
    set_bit(bit_array, 7);
    set_bit(bit_array, 8);
    printf("bit 0 = %d, bit 7 = %d, bit 8 = %d\n",
           test_bit(bit_array, 0),
           test_bit(bit_array, 7),
           test_bit(bit_array, 8));

    return 0;
}

void set_bit(char bit_array[], unsigned bit_number) {
    unsigned byte = bit_number / 8;     /* 落在哪个字节 */
    unsigned bit  = bit_number % 8;     /* 字节内第几位（0~7） */
    bit_array[byte] |= (1 << bit);      /* 该位置 1 */
}

void clear_bit(char bit_array[], unsigned bit_number) {
    unsigned byte = bit_number / 8;
    unsigned bit  = bit_number % 8;
    bit_array[byte] &= ~(1 << bit);     /* 该位清零 */
}

void assign_bit(char bit_array[], unsigned bit_number, int value) {
    unsigned byte = bit_number / 8;
    unsigned bit  = bit_number % 8;
    if (value == 0)
        bit_array[byte] &= ~(1 << bit); /* 清零 */
    else
        bit_array[byte] |= (1 << bit);  /* 置 1 */
}

int test_bit(char bit_array[], unsigned bit_number) {
    unsigned byte = bit_number / 8;
    unsigned bit  = bit_number % 8;
    return (bit_array[byte] & (1 << bit)) != 0;  /* 该位是否为 1 */
}