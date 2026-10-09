#include "queue.h"
#include <stdint.h>
#include <stdlib.h>

/* 文件内状态：调用方不保存底层地址，也不负责释放。
 * front 指向队首，rear 指向下次写入位置，size 是有效元素数量。
 * 数量和容量均以“个”为单位。此模块不提供并发保证。 */
static int *data = NULL;
static size_t front = 0;
static size_t rear = 0;
static size_t size = 0;
static size_t capacity = 0;

#ifdef QUEUE_TEST
/* 仅测试构建使用可控分配器，正常构建仍然调用 malloc。 */
void *test_malloc(size_t bytes);
#define QUEUE_MALLOC test_malloc
#else
#define QUEUE_MALLOC malloc
#endif

/* 前提：capacity > 0 且 index < capacity；避免取模和加法溢出。 */
static size_t next_index(size_t index)
{
    if (index == capacity - 1)
    {
        return 0;
    }
    return index + 1;
}

bool resize_queue(size_t new_length)
{
    if (new_length < size)
    {
        return false; /* 不静默丢弃队列中的有效元素。 */
    }
    if (new_length > SIZE_MAX / sizeof *data)
    {
        return false; /* 转换为字节数前检查乘法溢出。 */
    }
    if (new_length == capacity)
    {
        return true;
    }
    if (new_length == 0)
    {
        destroy_queue(); /* size 已为零，显式释放，避免零字节请求。 */
        return true;
    }

    /* 不能仅 realloc 后改变容量：回绕后的物理顺序不等于 FIFO 顺序。
     * 先申请新数组，失败时旧数组和所有状态都不受影响。 */
    int *new_data = QUEUE_MALLOC(new_length * sizeof *new_data);
    if (new_data == NULL)
    {
        return false;
    }

    /* 从旧队首按旧容量回绕读取，连续写入新数组开头。
     * 空队列不进入循环，所以不会在 capacity 为零时调用 next_index。 */
    size_t old_index = front;
    for (size_t index = 0; index < size; index++)
    {
        new_data[index] = data[old_index];
        old_index = next_index(old_index);
    }

    /* 复制完成才提交新状态；释放旧数组的责任始终在模块内。 */
    free(data);
    data = new_data;
    capacity = new_length;
    front = 0;
    if (size == capacity)
    {
        rear = 0; /* 缩到恰好满时，下一位置回绕到开头。 */
    }
    else
    {
        rear = size;
    }
    return true;
}

bool enqueue(int value)
{
    if (size == capacity)
    {
        return false; /* 包括零容量状态，避免访问空数组。 */
    }
    data[rear] = value;
    rear = next_index(rear);
    size++;
    return true;
}

bool dequeue(int *out_value)
{
    if (out_value == NULL || size == 0)
    {
        return false;
    }
    *out_value = data[front]; /* 先取得原队首值，再移动队首。 */
    front = next_index(front);
    size--;
    return true;
}

bool queue_peek(int *out_value)
{
    if (out_value == NULL || size == 0)
    {
        return false;
    }
    *out_value = data[front];
    return true;
}

size_t queue_size(void) { return size; }
size_t queue_capacity(void) { return capacity; }

void destroy_queue(void)
{
    free(data);
    data = NULL;
    front = 0;
    rear = 0;
    size = 0;
    capacity = 0;
}
