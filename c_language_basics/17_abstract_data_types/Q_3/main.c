#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

static int checks = 0;   /* 已执行的检查数量。 */
static int failures = 0; /* 失败数量，决定程序退出码。 */
static void check(bool condition, const char *description)
{
    checks++;
    if (!condition)
    {
        failures++;
        fprintf(stderr, "FAIL: %s\n", description);
    }
}

#ifdef QUEUE_TEST
static bool fail_next_allocation = false; /* 下一次节点分配返回 NULL。 */
static size_t live_nodes = 0;             /* 当前未释放的分配数量。 */
void *test_malloc(size_t bytes)
{
    if (fail_next_allocation)
    {
        fail_next_allocation = false;
        return NULL;
    }
    void *ptr = malloc(bytes);
    if (ptr != NULL)
    {
        live_nodes++;
    }
    return ptr;
}
void test_free(void *ptr)
{
    if (ptr != NULL)
    {
        check(live_nodes > 0, "free has matching allocation count");
        if (live_nodes > 0)
        {
            live_nodes--;
        }
    }
    free(ptr);
}
#endif

/* 每次真正取出一个元素，检查完整 FIFO 顺序。 */
static void expect_value(int expected)
{
    int value = 12345;
    check(dequeue(&value) && value == expected, "FIFO value");
}

int main(void)
{
    int value = 12345;
    check(queue_is_empty() && queue_size() == 0, "initial empty state");
    check(!dequeue(&value) && value == 12345, "empty dequeue output unchanged");
    check(!queue_peek(&value) && value == 12345, "empty peek output unchanged");
    check(!dequeue(NULL) && !queue_peek(NULL), "null output on empty queue");
    destroy_queue();
    destroy_queue();
    check(queue_is_empty() && queue_size() == 0, "repeat empty destroy");
#ifdef QUEUE_TEST
    fail_next_allocation = true;
    check(!enqueue(10), "first allocation failure");
    check(queue_is_empty() && queue_size() == 0 && live_nodes == 0,
          "failed first enqueue preserves empty state");
#endif
    check(enqueue(10), "first enqueue");
    check(!queue_is_empty() && queue_size() == 1, "single node state");
    check(queue_peek(&value) && value == 10 && queue_size() == 1,
          "peek does not remove");
    check(!dequeue(NULL) && !queue_peek(NULL) && queue_size() == 1,
          "null output preserves nonempty queue");
    expect_value(10);
    check(queue_is_empty() && queue_size() == 0, "last dequeue clears queue");
    check(enqueue(20) && enqueue(30), "enqueue after last dequeue");
#ifdef QUEUE_TEST
    fail_next_allocation = true;
    check(!enqueue(999), "nonempty allocation failure");
    check(queue_size() == 2 && live_nodes == 2 &&
          queue_peek(&value) && value == 20, "failed enqueue preserves state");
#endif
    check(enqueue(40), "enqueue after failed operation");
    expect_value(20);
    check(enqueue(50), "interleaved enqueue");
    expect_value(30);
    expect_value(40);
    expect_value(50);
    check(queue_is_empty(), "interleaved queue drained");
    check(enqueue(0) && enqueue(-1) && enqueue(-1), "zero negative duplicate values");
    expect_value(0);
    expect_value(-1);
    expect_value(-1);

    /* 批量数据验证 FIFO 和计数；分配不足时仍能清理已经接入的节点。 */
    for (int number = 0; number < 1000; number++)
    {
        if (!enqueue(number))
        {
            check(false, "batch enqueue");
            destroy_queue();
            return EXIT_FAILURE;
        }
    }
    check(queue_size() == 1000, "batch size");
    for (int number = 0; number < 1000; number++)
    {
        expect_value(number);
    }
    check(queue_is_empty() && queue_size() == 0, "batch drained");
#ifdef QUEUE_TEST
    check(live_nodes == 0, "dequeue releases all nodes");
#endif
    check(enqueue(1) && enqueue(2) && enqueue(3), "prepare nonempty destroy");
    destroy_queue();
    check(queue_is_empty() && queue_size() == 0, "destroy clears state");
#ifdef QUEUE_TEST
    check(live_nodes == 0, "destroy releases all nodes");
#endif
    destroy_queue();
    check(enqueue(99), "reuse after destroy");
    expect_value(99);
    destroy_queue();
#ifdef QUEUE_TEST
    check(live_nodes == 0, "final allocation balance");
#endif
    printf("Checks: %d, failures: %d\n", checks, failures);
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
