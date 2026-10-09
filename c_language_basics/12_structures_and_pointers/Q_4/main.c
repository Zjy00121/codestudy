#include <stdio.h>
#include <stdlib.h>

struct NODE {
    int value;          // 当前节点保存的整数，可以重复或为负数
    struct NODE *next;  // 后继节点地址，尾节点为 NULL
};

int sll_remove (struct NODE **rootp, struct NODE *node);

int main (void) {
    /*
     * 每项独立建立链表，节点值为 10、20、10、30。
     * target_index >= 0 表示数组中的节点，-1 表示链表外节点，
     * -2 表示 NULL。链表外节点也保存 10，检查是否错误地按值删除。
     */
    const struct {
        const char *name; // 英文测试名称
        size_t length;    // 本次链表的节点数，不超过 4
        int target_index; // 目标节点选择方式，见上方说明
        int null_rootp;   // 非零时传入 NULL，测试无效入口参数
        int expected;     // 预期返回值：1 表示移除，0 表示未移除
    } cases[] = {
        {"Empty list", 0, -1, 0, 0},
        {"Remove only node", 1, 0, 0, 1},
        {"Single node missing", 1, -1, 0, 0},
        {"Remove head", 4, 0, 0, 1},
        {"Remove middle", 4, 1, 0, 1},
        {"Remove second duplicate", 4, 2, 0, 1},
        {"Remove tail", 4, 3, 0, 1},
        {"External node with same value", 4, -1, 0, 0},
        {"NULL target", 4, -2, 0, 0},
        {"NULL root pointer", 4, 1, 1, 0}
    };
    const int values[] = {10, 20, 10, 30};
    size_t case_count = sizeof cases / sizeof cases[0];
    int failures = 0;

    for (size_t test = 0; test < case_count; ++test)
    {
        /* 局部节点无需动态分配，移除后仍然有效，不能 free。 */
        struct NODE nodes[4];
        struct NODE outside = {10, NULL};
        struct NODE *head = NULL;
        size_t length = cases[test].length;

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

        struct NODE *target = NULL;
        if (cases[test].target_index >= 0)
        {
            target = &nodes[cases[test].target_index];
        }
        else if (cases[test].target_index == -1)
        {
            target = &outside;
        }

        /* 正常调用传 &head，使函数能修改 main 中的头指针。 */
        struct NODE **rootp = &head;
        if (cases[test].null_rootp)
        {
            rootp = NULL;
        }
        int actual = sll_remove(rootp, target);
        int passed = (actual == cases[test].expected);

        /* 按地址核对剩余节点顺序；固定次数检查，避免成环时无限遍历。 */
        struct NODE *current = head;
        for (size_t i = 0; i < length; ++i)
        {
            if (cases[test].expected && &nodes[i] == target)
            {
                continue; // 成功时只跳过指定的那个节点
            }
            if (current != &nodes[i])
            {
                passed = 0;
                break; // 未知地址不再解引用
            }
            current = current->next;
        }
        if (current != NULL)
        {
            passed = 0; // 最后一个保留节点必须以 NULL 结尾
        }

        /* 移除不应改变任何节点的数据，也不应修改链表外节点。 */
        for (size_t i = 0; i < length; ++i)
        {
            if (nodes[i].value != values[i])
            {
                passed = 0;
            }
        }
        if (outside.value != 10 || outside.next != NULL)
        {
            passed = 0;
        }

        if (cases[test].expected)
        {
            /* 本实现把已移除节点的 next 清空；再次移除应返回假。 */
            if (target->next != NULL || sll_remove(&head, target) != 0)
            {
                passed = 0;
            }
        }

        if (passed)
        {
            printf("%s: PASS\n", cases[test].name);
        }
        else
        {
            printf("%s: FAIL\n", cases[test].name);
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
 * 从有效、无环且以 NULL 结尾的链表中移除指定节点。
 * rootp：调用方头指针变量的地址；node：待移除节点的有效地址。
 * 返回 1 表示成功，返回 0 表示未找到或传入了 NULL 参数。
 * 比较节点地址，不比较数据。只摘链，不分配或释放内存；
 * 若节点来自动态分配，调用方仍负责决定何时释放或复用。
 */
int sll_remove (struct NODE **rootp, struct NODE *node) {
    if (rootp == NULL || node == NULL) {
        return 0; // 检查参数后才能读取 *rootp
    }

    struct NODE *current = *rootp; // 从首节点开始，空表时自然跳过循环
    struct NODE *prev = NULL;      // 首节点没有前驱
    while (current != NULL) {
        if (current == node) {
            if (prev == NULL) {
                *rootp = current->next;     // 删除首节点，更新调用方入口
            }
            else {
                prev->next = current->next; // 前驱跳过当前节点，也适用于尾节点
            }
            current->next = NULL; // 摘下后成为独立节点，必须在接好后继之后执行
            return 1;
        }
        prev = current;          // 先保留前驱，再前进到后继
        current = current->next;
    }
    return 0; // 已经到达 NULL，不能再访问 current->value
}
