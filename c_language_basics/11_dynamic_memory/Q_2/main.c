#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>  // INT_MAX：array[0] 能保存的最大数量

int *readints(void);

int main(void)
{
    int *array = readints();
    if (array == NULL) {
        fprintf(stderr, "Input or allocation failed.\n");
        return EXIT_FAILURE;
    }

    // 首元素只统计输入的整数，不把计数元素本身算进去。
    printf("Count: %d\nValues:", array[0]);
    size_t count = (size_t)array[0];
    for (size_t i = 0; i < count; ++i) {
        printf(" %d", array[i + 1]);
    }
    putchar('\n');

    // readints 返回动态内存，调用者使用完后负责释放。
    free(array);
    return EXIT_SUCCESS;
}

/*
 * 从 stdin 读取以空白分隔的 int，正常 EOF 表示列表结束。
 * 输入数值须在 int 范围内；scanf("%d") 不是超范围整数校验器。
 * 返回布局：[数量][第一个整数][第二个整数]……。
 * 空输入也返回有效数组，array[0] 为 0。
 * 分配失败、容量超限、非法输入或输入流出错时，清理内存并返回 NULL。
 */
int *readints(void)
{
    // capacity 是已分配的 int 槽位总数，包含 array[0]。
    // count 只记录读入的数据个数，因此已使用槽位数为 count + 1。
    size_t capacity = 1;
    size_t count = 0;
    int *array = malloc(capacity * sizeof *array);

    // == 才是比较；= 会覆盖指针并丢失分配结果。
    if (array == NULL) {
        return NULL;
    }

    // 最大槽位数用于保证后面的“槽位数 * sizeof(int)”不溢出。
    size_t max_slots = SIZE_MAX / sizeof *array;
    int value;
    int result;

    while ((result = scanf("%d", &value)) == 1) {
        // 计数最终存入 int，先防止其超出 INT_MAX。
        if (count >= (size_t)INT_MAX) {
            free(array);
            return NULL;
        }

        // 在写入之前检查空间，不能先写后扩容。
        // 初始 count=0、capacity=1，仅有计数槽，所以第一项就需扩容。
        if (count == capacity - 1) {
            if (capacity >= max_slots) {
                free(array);
                return NULL;
            }

            // 通常容量翻倍，减少每读一个数就 realloc 的开销。
            // 先检查再乘，接近上限时增长到最大安全槽位数。
            size_t new_capacity;
            if (capacity > max_slots / 2) {
                new_capacity = max_slots;
            } else {
                new_capacity = capacity * 2;
            }

            // 临时指针接收结果：非零大小请求失败时，旧块仍然存在。
            int *tmp = realloc(array, new_capacity * sizeof *array);
            if (tmp == NULL) {
                free(array);  // 本函数选择失败返回，因此释放旧块
                return NULL;
            }

            // 成功后才更新地址和容量。
            array = tmp;
            capacity = new_capacity;
        }

        // 先增加数据数量，使第一项存到 array[1]，而不是计数位置。
        ++count;
        array[count] = value;
    }

    // scanf 返回 0 表示匹配失败，不代表 EOF。
    // 返回 EOF 也可能是读取错误，因此还需检查 ferror。
    if (result != EOF || ferror(stdin)) {
        free(array);
        return NULL;
    }

    // 前面保证了 count <= INT_MAX，这次转换不会丢失数量。
    array[0] = (int)count;
    return array;
}
