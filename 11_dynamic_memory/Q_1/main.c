#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  // SIZE_MAX：size_t 可表示的最大值
#include <assert.h>  // assert：测试条件不成立时终止程序

void *my_calloc (size_t num, size_t size);

int main(void)
{
    // 测试 1：整数数组应初始化为数值零，并且可以正常写入。
    int *values = my_calloc(5, sizeof *values);
    assert(values != NULL);
    for (size_t i = 0; i < 5; ++i) {
        assert(values[i] == 0);
        values[i] = 42;
        assert(values[i] == 42);
    }
    free(values);

    // 测试 2：每个元素为 7 字节，共 21 字节。
    // 逐字节检查，确保不是只清零 num 字节，或只处理整倍数的 int。
    size_t total = 3 * 7;
    unsigned char *bytes = my_calloc(3, 7);
    assert(bytes != NULL);
    for (size_t i = 0; i < total; ++i) {
        assert(bytes[i] == 0);
        bytes[i] = 0xA5;
    }
    // 最后一个字节也应在申请范围内并可写入。
    assert(bytes[total - 1] == 0xA5);
    free(bytes);

    // 测试 3：最小非零请求，仅申请并清零一个字节。
    bytes = my_calloc(1, 1);
    assert(bytes != NULL);
    assert(bytes[0] == 0);
    free(bytes);

    // 测试 4：本实现明确约定，任一参数为零都返回 NULL。
    assert(my_calloc(0, 7) == NULL);
    assert(my_calloc(3, 0) == NULL);
    assert(my_calloc(0, 0) == NULL);

    // 测试 5：乘积超过 SIZE_MAX 时，应在调用 malloc 之前拒绝。
    // 两种顺序都检查，避免只对某个参数生效。
    assert(my_calloc(SIZE_MAX, 2) == NULL);
    assert(my_calloc(2, SIZE_MAX) == NULL);
    assert(my_calloc(SIZE_MAX / 2 + 1, 2) == NULL);

    puts("All tests passed: zeroing, writable memory, zero size and overflow.");
    return 0;
}

/*
 * 功能：申请 num 个元素，每个元素 size 字节，并把所有字节清零。
 * 成功返回起始地址，调用者使用结束后负责 free。
 * 大小溢出或 malloc 失败返回 NULL。
 * 对零大小请求，本实现选择返回 NULL；不要求与系统 calloc 返回相同指针。
 */
void *my_calloc (size_t num, size_t size) {
    // 第一步：处理零大小，同时避免后面的除法出现除零。
    if (num == 0 || size == 0) {
        return NULL;
    }

    // 第二步：先检查乘法是否安全，不能先乘完再检查。
    // num > SIZE_MAX / size 意味着 num * size 超出 size_t 的范围。
    if (num > SIZE_MAX / size) {
        return NULL;
    }

    size_t total = num * size;

    // 第三步：使用 malloc 获取内存。
    // unsigned char 的大小为 1 字节，可以访问任意对象的字节表示。
    // int * 的下标每次跨过 sizeof(int) 字节，不适合通用的逐字节清零。
    unsigned char *p = malloc(total);

    // == 是比较；写成 p = NULL 会把地址覆盖掉，且条件结果为假。
    // 分配失败时不能继续写内存，由调用者决定是否输出错误信息。
    if (p == NULL) {
        return NULL;
    }

    // 第四步：清零全部 total 字节，不只是 num 个字节。
    // 这保证按位清零，不意味着所有类型的零值都具有全零位表示。
    for (size_t i = 0; i < total; ++i) {
        p[i] = 0;
    }

    // 对象指针在 C 中可以转换为 void *，无需显式强制转换。
    return p;
}
