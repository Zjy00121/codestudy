/*
 * 把 value 的二进制位从左到右翻转一下后返回。
 * 例如在 32 位机器上，25（00000000 00000000 00000000 00011001）
 * 翻转后为 2550136832（10011000 00000000 00000000 00000000）。
 *
 * 该函数不依赖机器上整型值的长度——用循环条件 i != 0
 * 自动适配任意位宽（16/32/64 位均可）。
 */

#include <stdio.h>

unsigned int reverse_bits(unsigned int value);

int main(void) {
    unsigned int value = 25;            /* 二进制: 000...000 11001 */
    unsigned int answer;

    answer = reverse_bits(value);
    printf("original: %u (0x%08X)\n", value, value);
    printf("reversed: %u (0x%08X)\n", answer, answer);

    return 0;
}

unsigned int reverse_bits(unsigned int value) {
    unsigned int i;
    unsigned int answer = 0;

    /*
     * i 从 1 开始左移，经过每一位后最终溢出为 0 循环结束。
     * 循环次数 = unsigned int 的位数，不硬编码 32 或 64。
     */
    for (i = 1; i != 0; i <<= 1) {
        answer <<= 1;          /* answer 左移一位，为下个比特腾出位置 */
        if (value & 1)         /* 取 value 当前最低位 */
            answer |= 1;       /* 若为 1，则置 answer 最低位为 1 */
        value >>= 1;           /* value 右移，下轮取新的最低位 */
    }

    return answer;
}