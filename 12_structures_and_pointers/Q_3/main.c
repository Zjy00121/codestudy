#include <stdio.h>
#include <stdlib.h>

struct NODE {
    int value;          // 当前节点保存的整数，可以重复或为负数
    struct NODE *next;  // 后继节点地址，尾节点为 NULL
};

struct NODE *sll_reverse (struct NODE *first);

int main (void) {
    /* 分别覆盖空链表、单节点、双节点和多个节点。 */
    const size_t lengths[] = {0, 1, 2, 5};
    const int values[] = {30, -5, 0, 30, 8}; // 含重复值，不能只靠值判断顺序
    size_t case_count = sizeof lengths / sizeof lengths[0];
    int failures = 0;

    for (size_t test = 0; test < case_count; ++test)
    {
        /*
         * 用局部数组提供节点存储，链接仍由 next 明确建立。
         * 节点在本轮测试期间有效，不使用 malloc，也不能调用 free。
         */
        struct NODE nodes[5];
        size_t length = lengths[test];
        struct NODE *head = NULL;

        for (size_t i = 0; i < length; ++i)
        {
            nodes[i].value = values[i];
            nodes[i].next = NULL;
            if (i + 1 < length)
            {
                nodes[i].next = &nodes[i + 1];
            }
        }

        if (length != 0)
        {
            head = &nodes[0];
        }

        /* 必须接收返回值，调用方的入口才会指向新的首节点。 */
        head = sll_reverse(head);
        struct NODE *current = head;
        int reversed_ok = 1;

        /*
         * 按节点地址验证反序，而不只是比较数据。
         * 最多检查 length 个节点，避免错误成环时测试遍历无限运行。
         */
        for (size_t i = 0; i < length; ++i)
        {
            size_t expected_index = length - 1 - i;
            if (current != &nodes[expected_index])
            {
                reversed_ok = 0;
                break; // 地址不符合预期时不继续解引用
            }
            current = current->next;
        }

        /* 同时验证尾节点指向 NULL；空表时验证返回值就是 NULL。 */
        if (current != NULL)
        {
            reversed_ok = 0;
        }

        /* 节点数据不应变化，反转只修改链接。 */
        for (size_t i = 0; i < length; ++i)
        {
            if (nodes[i].value != values[i])
            {
                reversed_ok = 0;
            }
        }

        if (!reversed_ok)
        {
            printf("Reverse %zu nodes: FAIL\n", length);
            failures++;
            continue; // 不把结构异常的链表再次交给反转函数
        }
        printf("Reverse %zu nodes: PASS\n", length);

        /* 再反转一次，应恢复原来的入口、节点顺序及数据。 */
        head = sll_reverse(head);
        current = head;
        int restored_ok = 1;

        for (size_t i = 0; i < length; ++i)
        {
            if (current != &nodes[i])
            {
                restored_ok = 0;
                break;
            }
            if (current->value != values[i])
            {
                restored_ok = 0;
            }
            current = current->next;
        }
        if (current != NULL)
        {
            restored_ok = 0;
        }

        if (restored_ok)
        {
            printf("Restore %zu nodes: PASS\n", length);
        }
        else
        {
            printf("Restore %zu nodes: FAIL\n", length);
            failures++;
        }
    }

    if (failures != 0)
    {
        printf("Failed checks: %d\n", failures);
        return EXIT_FAILURE;
    }
    printf("All tests passed.\n");
    return EXIT_SUCCESS;
}

/*
 * 原地反转有效、无环、以 NULL 结尾的单链表。
 * first：原首节点地址，允许为 NULL。
 * 返回新首节点地址，空表返回 NULL；调用方应接收返回值更新入口。
 * 只修改节点链接，不改变数据，不分配或释放节点，所有权保持不变。
 */
struct NODE *sll_reverse (struct NODE *first) {
    /* 必须先判断空表，才能访问 first->next。 */
    if (first == NULL) {
        return NULL;
    }

    struct NODE *current = first;    // 已反转部分的头节点
    struct NODE *next = first->next; // 尚未处理部分的首节点
    struct NODE *prev;               // 本轮接入节点时使用的前方连接
    current->next = NULL;            // 原首节点将成为尾节点，先断开原链接
    while (next != NULL) {
        prev = current;              // 保存已反转部分的入口
        current = next;              // 取出下一个待反转节点
        next = current->next;        // 修改链接前先保存原后继，防止丢失剩余链
        current->next = prev;        // 让当前节点指向已反转部分
    }
    first = current;                // 这里只修改形参副本，必须返回给调用方
    return first;
}
