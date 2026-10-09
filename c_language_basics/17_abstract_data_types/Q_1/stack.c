#include "stack.h"
#include <stdint.h>
#include <stdlib.h>

/* 文件内状态隐藏实现，调用方不接触底层地址或负责释放它。
 * size 是有效数量，capacity 是容量，两者单位均为元素个数。
 * 此单栈模块不提供并发保证。 */
static int *data = NULL;
static size_t size = 0;
static size_t capacity = 0;

#ifdef STACK_TEST
/* 仅测试构建替换分配接口，确定性验证分配失败；正常构建用 realloc。 */
void *test_realloc(void *ptr, size_t bytes);
#define STACK_REALLOC test_realloc
#else
#define STACK_REALLOC realloc
#endif

bool resize_stack(size_t new_length)
{
    /* 不静默丢弃栈顶元素，失败时原状态保持不变。 */
    if (new_length < size)
    {
        return false;
    }

    /* malloc/realloc 使用字节数，先检查乘法是否溢出。 */
    if (new_length > SIZE_MAX / sizeof *data)
    {
        return false;
    }

    if (new_length == capacity)
    {
        return true; /* 同容量无需重新分配。 */
    }

    if (new_length == 0)
    {
        /* 已确认 size 为零，显式释放，避免依赖 realloc(ptr, 0)。 */
        destroy_stack();
        return true;
    }

    /* 临时指针保留失败时的旧地址；非零大小失败不释放原块。 */
    int *new_data = STACK_REALLOC(data, new_length * sizeof *data);
    if (new_data == NULL)
    {
        return false;
    }

    /* 成功后旧指针不再使用；保留的有效元素数量无需改变。 */
    data = new_data;
    capacity = new_length;
    return true;
}

bool push(int value)
{
    if (size == capacity)
    {
        return false;
    }
    data[size] = value; /* 先写入，再增加有效数量。 */
    size++;
    return true;
}

bool pop(int *out_value)
{
    if (out_value == NULL || size == 0)
    {
        return false;
    }
    size--;
    *out_value = data[size]; /* 新 size 就是原栈顶下标。 */
    return true;
}

bool peek(int *out_value)
{
    if (out_value == NULL || size == 0)
    {
        return false;
    }
    *out_value = data[size - 1];
    return true;
}

size_t stack_size(void)
{
    return size;
}

size_t stack_capacity(void)
{
    return capacity;
}

void destroy_stack(void)
{
    free(data); /* 模块是唯一所有者，free(NULL) 也合法。 */
    data = NULL;
    size = 0;
    capacity = 0;
}
