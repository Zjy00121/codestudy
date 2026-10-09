#include "stack.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* 独立练习兼作自动验证：失败返回非零退出码，不依赖 assert/NDEBUG。 */
static int failures = 0;
static int checks = 0;

static void check(bool condition, const char *description)
{
    checks++;
    if (!condition)
    {
        failures++;
        fprintf(stderr, "FAIL: %s\n", description);
    }
}

#ifdef STACK_TEST
/* 只让下一次分配失败；其余调用转交真实 realloc。 */
static bool fail_next_allocation = false;
void *test_realloc(void *ptr, size_t bytes)
{
    if (fail_next_allocation)
    {
        fail_next_allocation = false;
        return NULL; /* 不触碰旧块，模拟非零 realloc 的失败约定。 */
    }
    return realloc(ptr, bytes);
}
#endif

int main(void)
{
    int value = -1; /* 接收数据，也用于检查失败时输出未被改动。 */
    check(stack_size() == 0 && stack_capacity() == 0, "initial state");
    check(resize_stack(0), "empty zero capacity");
    check(!push(1), "zero capacity rejects push");
    check(!pop(&value) && value == -1, "empty pop preserves output");
    check(!peek(&value) && value == -1, "empty peek preserves output");
    check(!pop(NULL) && !peek(NULL), "null output rejected");

    check(resize_stack(2), "allocate two elements");
    check(push(10) && push(20), "fill stack");
    check(!push(30), "full stack rejected");
    check(!resize_stack(1), "reject shrink below size");
    check(!resize_stack(0), "reject zero with live elements");
    check(stack_size() == 2 && stack_capacity() == 2 &&
          peek(&value) && value == 20, "rejected shrink preserves state");
    check(resize_stack(2), "same capacity succeeds");

#ifdef STACK_TEST
    fail_next_allocation = true;
    check(!resize_stack(4), "allocation failure reported");
    check(stack_size() == 2 && stack_capacity() == 2 &&
          peek(&value) && value == 20, "allocation failure preserves state");
#endif

    check(resize_stack(4), "grow capacity");
    check(stack_size() == 2 && stack_capacity() == 4, "grow keeps size");
    check(push(30) && push(40), "use added capacity");
    check(pop(&value) && value == 40, "LIFO after grow");
    check(resize_stack(3), "shrink exactly to size");
    check(stack_size() == 3 && stack_capacity() == 3, "shrink state");
    check(!push(50), "shrunk stack is full");

#ifdef STACK_TEST
    check(resize_stack(6), "prepare shrink failure");
    fail_next_allocation = true;
    check(!resize_stack(4), "shrink allocation failure reported");
    check(stack_size() == 3 && stack_capacity() == 6 &&
          peek(&value) && value == 30, "failed shrink preserves state");
#endif

    /* int 大于一字节时，这个请求必定导致乘法溢出，拒绝前不分配。 */
    if (sizeof(int) > 1)
    {
        size_t old_capacity = stack_capacity();
        check(!resize_stack(SIZE_MAX / sizeof(int) + 1), "overflow rejected");
        check(stack_capacity() == old_capacity && stack_size() == 3,
              "overflow preserves state");
    }

    check(pop(&value) && value == 30, "preserved third value");
    check(pop(&value) && value == 20, "preserved second value");
    check(pop(&value) && value == 10, "preserved first value");
    check(resize_stack(0), "release empty storage");
    check(stack_size() == 0 && stack_capacity() == 0, "released state");
    check(resize_stack(1) && push(99), "reuse after release");
    destroy_stack();
    destroy_stack();
    check(stack_size() == 0 && stack_capacity() == 0, "repeat destroy");
    printf("Checks: %d, failures: %d\n", checks, failures);
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
