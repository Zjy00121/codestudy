#include <stdio.h>
#include <stdlib.h>

struct NODE {
    int value;          // 节点数据；计数时不依赖数据的值
    struct NODE *next;  // 后继节点地址；尾节点为 NULL
};

int count_NODE (struct NODE *head);

int main(void)
{
    /*
     * 使用局部节点构造链表，不需要 malloc，也不能对它们调用 free。
     * 所有节点在 main 执行期间有效；连接顺序由 next 决定。
     */
    struct NODE last = {20, NULL};
    struct NODE third = {20, &last};
    struct NODE second = {-5, &third};
    struct NODE first = {0, &second};
    struct NODE *head = &first;

    /* 每个测试项保存名称、传入的起点和预期节点数。 */
    struct {
        const char *name;   // 英文测试名称，便于定位失败用例
        struct NODE *start; // 传给计数函数的唯一参数
        int expected;       // 从该起点沿 next 能访问到的节点数
    } cases[] = {
        {"Empty list", NULL, 0},
        {"Single node", &last, 1},
        {"Four nodes", head, 4},
        {"Start at second node", &second, 3},
        {"Count the same list again", head, 4}
    };

    /* sizeof 的字节数相除，得到测试项数量。 */
    size_t case_count = sizeof cases / sizeof cases[0];
    int failures = 0;  // 累计失败数，决定程序的退出状态

    for (size_t i = 0; i < case_count; ++i)
    {
        int actual = count_NODE(cases[i].start);

        printf("%s: expected=%d, actual=%d - ",
               cases[i].name, cases[i].expected, actual);

        if (actual == cases[i].expected)
        {
            printf("PASS\n");
        }
        else
        {
            printf("FAIL\n");
            failures++;
        }
    }

    /* 计数只应读取链表：核对入口、链接和节点数据均未改变。 */
    if (head == &first &&
        first.next == &second && second.next == &third &&
        third.next == &last && last.next == NULL &&
        first.value == 0 && second.value == -5 &&
        third.value == 20 && last.value == 20)
    {
        printf("List unchanged: PASS\n");
    }
    else
    {
        printf("List unchanged: FAIL\n");
        failures++;
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
 * 统计从 head 开始的单链表节点数；head 为 NULL 时返回 0。
 * 前提：链表有效、无环、以 NULL 结尾，节点数不超过 INT_MAX。
 * 保留本题的 int 返回类型；更通用的计数接口可考虑 size_t。
 * 函数不分配或释放内存，也不修改节点和调用方的头指针。
 */
int count_NODE (struct NODE *head) {
    struct NODE * current = head; // 局部遍历指针，保存当前节点地址
    int count = 0;                // 已访问的节点数
    while (current != NULL) {
        count ++;                // 当前指针有效，计入这个节点
        current = current->next; // 沿后继地址移动，直到链表末尾
    }
    return count;                // 返回累计结果，而不是固定返回 0
}
