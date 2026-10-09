#include "queue.h"
#include <stdint.h>
#include <stdlib.h>

/* 节点定义隐藏在实现中，避免调用方修改链接或释放内部节点。 */
typedef struct QueueNode
{
    int value;              /* 当前节点的整数。 */
    struct QueueNode *next; /* 后继地址，最后节点为 NULL。 */
} QueueNode;

/* 空队列时两个指针同时为 NULL；非空时 rear->next 为 NULL。
 * size 是节点个数，不是字节数。模块仅供串行调用。 */
static QueueNode *front = NULL;
static QueueNode *rear = NULL;
static size_t size = 0;

#ifdef QUEUE_TEST
/* 测试构建注入失败并计数资源；正常构建直接使用标准分配函数。 */
void *test_malloc(size_t bytes);
void test_free(void *ptr);
#define NODE_ALLOC test_malloc
#define NODE_FREE test_free
#else
#define NODE_ALLOC malloc
#define NODE_FREE free
#endif

bool enqueue(int value)
{
    if (size == SIZE_MAX)
    {
        return false; /* 在增加数量之前检查，避免无符号回绕。 */
    }
    QueueNode *new_node = NODE_ALLOC(sizeof *new_node);
    if (new_node == NULL)
    {
        return false; /* 尚未修改旧链接，失败时原队列完整。 */
    }
    new_node->value = value;
    new_node->next = NULL; /* 先准备完整节点，再接入队尾。 */
    if (rear == NULL)
    {
        front = new_node; /* 第一个节点同时成为队首和队尾。 */
    }
    else
    {
        rear->next = new_node; /* 先连接，避免丢失原队尾。 */
    }
    rear = new_node;
    size++;
    return true;
}

bool dequeue(int *out_value)
{
    if (out_value == NULL || front == NULL)
    {
        return false;
    }
    QueueNode *old_front = front; /* 保存待释放节点。 */
    *out_value = old_front->value;
    front = old_front->next;      /* 释放前读取后继链接。 */
    if (front == NULL)
    {
        rear = NULL; /* 最后节点出队，不能留下悬空队尾。 */
    }
    size--;
    NODE_FREE(old_front); /* 节点已脱离队列，模块负责释放。 */
    return true;
}

bool queue_peek(int *out_value)
{
    if (out_value == NULL || front == NULL)
    {
        return false;
    }
    *out_value = front->value;
    return true;
}

bool queue_is_empty(void) { return front == NULL; }
size_t queue_size(void) { return size; }

void destroy_queue(void)
{
    while (front != NULL)
    {
        QueueNode *old_front = front;
        front = old_front->next; /* 先保存后继，再释放当前节点。 */
        NODE_FREE(old_front);
    }
    rear = NULL;
    size = 0;
}
