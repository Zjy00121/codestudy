#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* 明确报告检查失败并返回非零，不依赖 assert 是否启用。 */
static int checks = 0;
static int failures = 0;
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
/* 只让下一次分配失败，避免用巨大分配请求耗尽系统资源。 */
static bool fail_next_allocation = false;
void *test_malloc(size_t bytes)
{
    if (fail_next_allocation)
    {
        fail_next_allocation = false;
        return NULL;
    }
    return malloc(bytes);
}
#endif

/* 每次验证真正出队的数据，检查完整 FIFO 顺序而不仅是队首。 */
static void expect_value(int expected)
{
    int value = -1;
    check(dequeue(&value) && value == expected, "expected FIFO value");
}

int main(void)
{
    int value = -1;
    check(queue_size() == 0 && queue_capacity() == 0, "initial state");
    check(resize_queue(0), "initial zero resize");
    check(!enqueue(1), "zero capacity rejects enqueue");
    check(!dequeue(&value) && value == -1, "empty dequeue preserves output");
    check(!queue_peek(&value) && value == -1, "empty peek preserves output");
    check(!dequeue(NULL) && !queue_peek(NULL), "null output rejected");
#ifdef QUEUE_TEST
    fail_next_allocation = true;
    check(!resize_queue(5), "initial allocation failure");
    check(queue_size() == 0 && queue_capacity() == 0, "initial failure state");
#endif
    check(resize_queue(5), "allocate capacity five");
    for (int number = 10; number <= 50; number += 10)
    {
        check(enqueue(number), "fill original queue");
    }
    check(!enqueue(60), "full queue rejected");
    expect_value(10);
    expect_value(20);
    check(enqueue(60) && enqueue(70), "wrap rear at array end");
    check(queue_peek(&value) && value == 30 && queue_size() == 5,
          "peek does not remove");
    check(resize_queue(5), "same capacity with wrapped queue");
    check(!resize_queue(4) && !resize_queue(0), "reject destructive shrink");
    check(queue_size() == 5 && queue_capacity() == 5, "rejected shrink state");
#ifdef QUEUE_TEST
    fail_next_allocation = true;
    check(!resize_queue(8), "wrapped growth allocation failure");
    check(queue_size() == 5 && queue_capacity() == 5 &&
          queue_peek(&value) && value == 30, "failed growth preserves state");
#endif
    check(resize_queue(8), "grow wrapped full queue");
    check(queue_size() == 5 && queue_capacity() == 8, "growth state");
    check(enqueue(80), "use grown capacity");
    for (int number = 30; number <= 80; number += 10)
    {
        expect_value(number);
    }
    check(queue_size() == 0, "drain grown queue");

    /* 移动队首队尾，使有效内容再次跨越数组尾部。 */
    for (int number = 1; number <= 6; number++)
    {
        check(enqueue(number), "prepare wrapped shrink");
    }
    expect_value(1);
    expect_value(2);
    check(enqueue(7) && enqueue(8), "append after partial dequeue");
#ifdef QUEUE_TEST
    fail_next_allocation = true;
    check(!resize_queue(6), "wrapped shrink allocation failure");
    check(queue_size() == 6 && queue_capacity() == 8 &&
          queue_peek(&value) && value == 3, "failed shrink preserves state");
#endif
    check(resize_queue(6), "shrink wrapped queue exactly to size");
    check(queue_size() == 6 && queue_capacity() == 6 && !enqueue(9),
          "exact shrink full state");
    expect_value(3);
    check(enqueue(9), "enqueue after exact shrink and dequeue");
    if (sizeof(int) > 1)
    {
        check(!resize_queue(SIZE_MAX / sizeof(int) + 1), "overflow rejected");
        check(queue_size() == 6 && queue_capacity() == 6, "overflow state");
    }
    for (int number = 4; number <= 9; number++)
    {
        expect_value(number);
    }
    check(resize_queue(2), "empty queue shrink to positive capacity");
    check(enqueue(100) && enqueue(200), "use empty resized queue");
    expect_value(100);
    expect_value(200);
    check(resize_queue(0) && queue_capacity() == 0, "release empty queue");
    check(resize_queue(1) && enqueue(99), "single capacity reuse");
    check(!enqueue(100), "single capacity full");
    expect_value(99);
    check(enqueue(101), "single capacity wraps");
    expect_value(101);
    destroy_queue();
    destroy_queue();
    check(queue_size() == 0 && queue_capacity() == 0, "repeat destroy");
    printf("Checks: %d, failures: %d\n", checks, failures);
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
