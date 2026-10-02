#include <stdio.h>
#include <stdlib.h>

struct NODE {
    int value;         // 当前节点的数据，回调可以读取或修改
    struct NODE *next; // 后继节点地址，尾节点为 NULL
};

void sll_general (struct NODE *head, void (*function)(struct NODE *node));

/*
 * 以下状态仅供测试回调记录结果：题目限定回调只有一个节点参数，
 * 因此用文件内静态变量保存记录。每组测试开始前重置调用次数。
 */
static struct NODE *visited[4]; // 按调用先后保存节点地址，不复制或拥有节点
static size_t visit_count;      // 本轮回调被调用的次数

/* node 是遍历函数传来的有效节点；仅记录访问，不修改链表。 */
static void record_node(struct NODE *node)
{
    if (visit_count < sizeof visited / sizeof visited[0]) {
        visited[visit_count] = node; // 有界写入，额外调用仍由计数检测
    }
    visit_count++;
}

/* 把当前节点的数据清零，不改变 next，也不释放节点。 */
static void clear_value(struct NODE *node)
{
    node->value = 0;
}

int main (void) {
    /*
     * 链表顺序有意不同于数组顺序，验证遍历确实沿 next 行走。
     * 数据含重复值，必须比较节点地址才能区分节点。
     * 节点都是局部对象，本函数执行期间有效，不需要也不能 free。
     */
    struct NODE nodes[4] = {{10, NULL}, {-5, NULL}, {10, NULL}, {30, NULL}};
    nodes[0].next = &nodes[2];
    nodes[2].next = &nodes[1];
    nodes[1].next = &nodes[3];
    struct NODE *head = &nodes[0];

    const struct {
        const char *name;         // 英文测试名称
        struct NODE *start;      // 本次遍历的起点
        size_t expected_count;   // 预期回调次数
        struct NODE *expected[4]; // 预期节点地址顺序
    } cases[] = {
        {"Empty list", NULL, 0, {NULL}},
        {"Single node", &nodes[3], 1, {&nodes[3]}},
        {"Four nodes", head, 4, {&nodes[0], &nodes[2], &nodes[1], &nodes[3]}},
        {"Start in middle", &nodes[1], 2, {&nodes[1], &nodes[3]}},
        {"Repeat traversal", head, 4, {&nodes[0], &nodes[2], &nodes[1], &nodes[3]}}
    };
    size_t case_count = sizeof cases / sizeof cases[0];
    int failures = 0;

    for (size_t test = 0; test < case_count; ++test) {
        visit_count = 0;
        sll_general(cases[test].start, record_node);
        int passed = (visit_count == cases[test].expected_count);

        /* 只有次数正确才逐项核对，避免读取本轮未写入的记录。 */
        if (passed) {
            for (size_t i = 0; i < visit_count; ++i) {
                if (visited[i] != cases[test].expected[i]) {
                    passed = 0;
                }
            }
        }
        if (passed) {
            printf("%s: PASS\n", cases[test].name);
        }
        else {
            printf("%s: FAIL\n", cases[test].name);
            failures++;
        }
    }

    /* 本实现约定空回调直接返回，空表与非空表都可以安全调用。 */
    visit_count = 0;
    sll_general(head, NULL);
    sll_general(NULL, NULL);
    if (visit_count == 0 && nodes[0].value == 10 && nodes[1].value == -5 &&
        nodes[2].value == 10 && nodes[3].value == 30) {
        printf("Read-only traversal and NULL callback: PASS\n");
    }
    else {
        printf("Read-only traversal and NULL callback: FAIL\n");
        failures++;
    }

    /* 换一个回调，确认传入的确实是原节点地址，能够修改节点数据。 */
    sll_general(head, clear_value);
    if (nodes[0].value == 0 && nodes[1].value == 0 &&
        nodes[2].value == 0 && nodes[3].value == 0 &&
        head == &nodes[0] && nodes[0].next == &nodes[2] &&
        nodes[2].next == &nodes[1] && nodes[1].next == &nodes[3] &&
        nodes[3].next == NULL) {
        printf("Modify values and preserve links: PASS\n");
    }
    else {
        printf("Modify values and preserve links: FAIL\n");
        failures++;
    }

    if (failures != 0) {
        printf("Failed checks: %d\n", failures);
        return EXIT_FAILURE;
    }
    printf("All tests passed.\n");
    return EXIT_SUCCESS;
}

/*
 * head：有效、无环且以 NULL 结尾的单链表入口，允许为空。
 * function：接收一个节点地址且无返回值的回调；为空时直接返回。
 * 沿 next 顺序对每个节点调用一次回调，本函数无返回值。
 * 回调可以修改 value，但不得修改链表链接、释放链表节点或使其失效。
 * 本函数不分配、不释放节点，不改变调用方的头指针。
 */
void sll_general (struct NODE *head, void (*function)(struct NODE *node)) {
    if (function == NULL) {
        return; // 防止调用空函数指针
    }
    struct NODE *current = head; // 用局部指针遍历，保留调用方的入口
    while (current != NULL) {
        function (current);      // 传递节点地址，不是只传 value 的副本
        current = current->next; // 当前节点处理完，再沿链接访问后继
    }
}
