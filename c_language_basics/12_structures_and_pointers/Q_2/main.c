#include <stdio.h>
#include <stdlib.h>

struct NODE {
    int value;          // 当前节点保存的整数，可以重复或为负数
    struct NODE *next;  // 后继节点地址，尾节点为 NULL
};

struct NODE *search_NODE (struct NODE *head, int value);

int main (void) {
    /*
     * 构造无序链表：30 -> -5 -> 0 -> 30 -> 8 -> NULL。
     * 这些是局部节点，在 main 执行期间有效，不需要也不能 free。
     */
    struct NODE last = {8, NULL};
    struct NODE fourth = {30, &last};
    struct NODE third = {0, &fourth};
    struct NODE second = {-5, &third};
    struct NODE first = {30, &second};
    struct NODE *head = &first;

    /* 每项给出入口、目标值和预期地址，直接验证节点身份。 */
    struct {
        const char *name;       // 英文测试名称
        struct NODE *start;    // 查找起点，允许为 NULL
        int target;            // 要寻找的值
        struct NODE *expected; // 预期节点地址；未找到时为 NULL
    } cases[] = {
        {"Empty list", NULL, 30, NULL},
        {"Single node found", &last, 8, &last},
        {"Single node missing", &last, 9, NULL},
        {"Head and first duplicate", head, 30, &first},
        {"Negative value", head, -5, &second},
        {"Zero in middle", head, 0, &third},
        {"Tail node", head, 8, &last},
        {"Missing value", head, 99, NULL},
        {"Search from second node", &second, 30, &fourth}
    };

    /* 数组总字节数除以单项字节数，得到测试项数量。 */
    size_t case_count = sizeof cases / sizeof cases[0];
    int failures = 0;  // 累计失败数量，用于决定退出状态

    for (size_t i = 0; i < case_count; ++i)
    {
        struct NODE *actual = search_NODE(cases[i].start, cases[i].target);

        /* 不解引用返回值，NULL 结果也可以安全比较。 */
        if (actual == cases[i].expected)
        {
            printf("%s: PASS\n", cases[i].name);
        }
        else
        {
            printf("%s: FAIL\n", cases[i].name);
            failures++;
        }
    }

    /* 查找只读取节点，不应修改调用方的入口、链接或数据。 */
    if (head == &first &&
        first.next == &second && second.next == &third &&
        third.next == &fourth && fourth.next == &last && last.next == NULL &&
        first.value == 30 && second.value == -5 && third.value == 0 &&
        fourth.value == 30 && last.value == 8)
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
 * 在有效、无环且以 NULL 结尾的单链表中查找 value。
 * head：首节点地址，允许为 NULL；value：目标整数。
 * 返回首个匹配节点的地址，空表或未找到时返回 NULL。
 * 返回的是原节点的地址，不是副本；不分配、不释放、不修改节点。
 * 调用方访问结果前必须检查是否为空，并确保节点仍在有效期内。
 */
struct NODE *search_NODE (struct NODE *head, int value) {
    struct NODE *current = head; // 局部遍历指针，不改变调用方的 head
    while (current != NULL) {
        if (current->value == value) {
            /* 立即返回，因此重复值只返回沿链表遇到的第一个。 */
            return current;
        }

        /* 不匹配时必须移动，否则会一直检查同一节点而形成死循环。 */
        current = current->next;
    }
    return NULL; // 遍历到末尾仍未找到，包括空链表的情况
}
