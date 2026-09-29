#include <stdio.h>
#include <stdlib.h>

struct NODE {
    int value;          // 数据节点的整数；根节点的该成员不参与查找
    struct NODE *next;  // 数据节点的后继；根节点中保存首节点地址
    struct NODE *prev;  // 数据节点的前驱；根节点中保存尾节点地址
};

int dll_remove (struct NODE *rootp, struct NODE *node);

int main (void) {
    /*
     * 根节点只管理首尾，不参与数据链：首节点 prev、尾节点 next 为 NULL。
     * 空表时根节点的 next、prev 都为 NULL，不采用循环哨兵结构。
     */
    const struct {
        const char *name; // 测试名称
        size_t length;   // 本次数据节点数，最多 4
        int target;      // 非负为节点下标；-1 外部节点，-2 NULL，-3 根节点
        int null_root;   // 非零表示故意传入 NULL 根指针
        int expected;    // 预期返回值，1 成功，0 未移除
    } cases[] = {
        {"Empty list", 0, -1, 0, 0},
        {"Remove only node", 1, 0, 0, 1},
        {"Single node missing", 1, -1, 0, 0},
        {"Remove head", 4, 0, 0, 1},
        {"Remove middle", 4, 1, 0, 1},
        {"Remove second duplicate", 4, 2, 0, 1},
        {"Remove tail", 4, 3, 0, 1},
        {"Node from another list", 4, -1, 0, 0},
        {"NULL target", 4, -2, 0, 0},
        {"NULL root", 4, 1, 1, 0},
        {"Root is not a data node", 4, -3, 0, 0}
    };
    const int values[] = {10, 20, 10, 30};
    size_t case_count = sizeof cases / sizeof cases[0];
    int failures = 0;

    for (size_t test = 0; test < case_count; ++test)
    {
        /* 局部节点在本轮测试期间有效，不需要也不能调用 free。 */
        struct NODE nodes[4];
        struct NODE root = {0, NULL, NULL};
        struct NODE outside = {10, NULL, NULL};
        struct NODE other_root = {0, &outside, &outside};
        size_t length = cases[test].length;

        for (size_t i = 0; i < length; ++i)
        {
            nodes[i].value = values[i];
            nodes[i].next = NULL;
            nodes[i].prev = NULL;
            if (i + 1 < length) {
                nodes[i].next = &nodes[i + 1];
            }
            if (i != 0) {
                nodes[i].prev = &nodes[i - 1];
            }
        }
        if (length != 0) {
            root.next = &nodes[0];
            root.prev = &nodes[length - 1];
        }

        struct NODE *target = NULL;
        if (cases[test].target >= 0) {
            target = &nodes[cases[test].target];
        }
        else if (cases[test].target == -1) {
            target = &outside;
        }
        else if (cases[test].target == -3) {
            target = &root;
        }

        struct NODE *root_arg = &root;
        if (cases[test].null_root) {
            root_arg = NULL;
        }
        int actual = dll_remove(root_arg, target);
        int passed = (actual == cases[test].expected);

        /* 成功摘下后两侧链接清空，再次移除应返回 0。 */
        if (cases[test].expected) {
            if (target->next != NULL || target->prev != NULL) {
                passed = 0;
            }
            if (dll_remove(&root, target) != 0) {
                passed = 0;
            }
        }

        /* 独立建立预期节点地址序列，不依靠待测链表计算预期结果。 */
        struct NODE *expected_nodes[4];
        size_t remaining = 0;
        for (size_t i = 0; i < length; ++i) {
            if (!(cases[test].expected && &nodes[i] == target)) {
                expected_nodes[remaining] = &nodes[i];
                remaining++;
            }
            if (nodes[i].value != values[i]) {
                passed = 0; // 移除不应改变节点数据
            }
        }

        /* 限定遍历次数并先比较地址，避免错误链接导致测试无限遍历。 */
        struct NODE *current = root.next;
        for (size_t i = 0; i < remaining; ++i) {
            if (current != expected_nodes[i]) {
                passed = 0;
                break;
            }
            current = current->next;
        }
        if (current != NULL) {
            passed = 0;
        }

        /* 从根节点保存的尾指针反向核对，检查 prev 与尾部入口。 */
        current = root.prev;
        for (size_t i = remaining; i > 0; --i) {
            if (current != expected_nodes[i - 1]) {
                passed = 0;
                break;
            }
            current = current->prev;
        }
        if (current != NULL) {
            passed = 0;
        }

        /* 不属于本链表的节点及另一条链表必须保持原样。 */
        if (outside.value != 10 || outside.next != NULL || outside.prev != NULL ||
            other_root.next != &outside || other_root.prev != &outside ||
            root.value != 0) {
            passed = 0;
        }

        if (passed) {
            printf("%s: PASS\n", cases[test].name);
        }
        else {
            printf("%s: FAIL\n", cases[test].name);
            failures++;
        }
    }

    if (failures != 0) {
        printf("Failed checks: %d\n", failures);
        return EXIT_FAILURE;
    }
    printf("All tests passed.\n");
    return EXIT_SUCCESS;
}

/*
 * rootp：根节点地址，其 next/prev 保存数据链的首尾地址。
 * node：待移除节点地址；按地址匹配，不按 value 匹配。
 * 前提：数据链有效、无环、双向链接一致，首尾外侧链接为 NULL。
 * 返回 1 表示成功摘下，0 表示未找到或参数为空/目标为根节点。
 * 不分配、不释放内存，摘下节点的两个链接清空，释放责任仍由调用方承担。
 */
int dll_remove (struct NODE *rootp, struct NODE *node) {
    if (rootp == NULL || node == NULL || node == rootp) {
        return 0;
    }
    struct NODE *current = rootp->next; // 从首个数据节点开始确认归属
    while (current != NULL) {
        if (current == node) {
            /* 先更新前驱或根节点的首指针。 */
            if (current->prev == NULL) {
                rootp->next = current->next;
            }
            else {
                current->prev->next = current->next;
            }
            /* 再更新后继或根节点的尾指针，不能只维护一个方向。 */
            if (current->next == NULL) {
                rootp->prev = current->prev;
            }
            else {
                current->next->prev = current->prev;
            }

            /* 两侧重新连接后，再清空被摘下节点的链接。 */
            current->next = NULL;
            current->prev = NULL;
            return 1;
        }
        current = current->next;
    }
    return 0;
}
