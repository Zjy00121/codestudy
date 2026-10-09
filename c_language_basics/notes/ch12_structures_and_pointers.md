# 第十二章：使用结构和指针 — 知识点总结

> 根据本次对话中关于链表、单链表及双链表的讲解整理。学习进度统一见 [学习计划](../STUDY_PLAN.md)。
> 文中的概念片段不是一份可以直接拼接编译的完整程序，未单独编译运行；Q_1～Q_5 的独立练习已经编译测试，记录见文末。

## 12.1 链表

### 基本结构与数组的区别

链表用节点保存数据，用指针连接节点。数组元素连续存放；链表节点不要求连续，通过保存的地址找到后继。

```text
单链表：head → [10 | next] → [20 | next] → [30 | NULL]
双链表：head → [10] ⇄ [20] ⇄ [30] ← tail
```

- 数据域：保存业务数据，可以是整数，也可以是传感器记录等结构体。
- 指针域：保存其他节点的地址。
- 头指针：保存链表入口地址的指针变量，不是节点本身。
- 首节点：第一个数据节点。本文示例不带额外的哨兵头节点。
- 空链表：单链表中用 `head == NULL` 表示。
- 哨兵头节点：某些设计额外设置的辅助节点，不能与头指针混为一谈。

链表不要求使用 `malloc`。生命周期足够长的局部节点、静态节点或预分配节点池都可以建立链接；只有动态分配的节点才能按相应分配规则释放，不能对普通局部节点调用 `free`。

### 地址与对象的区别

| 表达式 | 含义 |
|------|------|
| `head` | 首节点地址，或空指针 |
| `*head` | 首节点这个结构体对象 |
| `head->value` | 首节点的数据 |
| `head->next` | 首节点保存的后继地址 |
| `head->next->value` | 第二个节点的数据，要求前两个节点均有效 |

`head = node` 只复制地址，不复制节点。指针变量和所指节点是不同对象，有各自的生命周期。

## 12.2 单链表

### 节点定义与自引用

```c
struct Node
{
    int value;          // 当前节点保存的数据
    struct Node *next;  // 后继节点地址；尾节点的 next 为 NULL
};
```

结构体可以包含指向同类对象的指针，但不能直接包含一个完整的自身对象。指针成员的大小可以确定；直接嵌套自身则无法形成有限大小的对象。

### 创建与分配责任

以下函数需要 `<stdlib.h>`：

```c
/*
 * 创建独立节点。
 * value：要保存的整数。
 * 返回值：成功返回节点地址，失败返回 NULL。
 * 调用方负责将节点交给链表管理，或在不再需要时释放。
 */
static struct Node *node_create(int value)
{
    /* sizeof *node 是一个完整节点的字节数，不是指针的大小。 */
    struct Node *node = malloc(sizeof *node);

    if (node == NULL)
    {
        /* 分配失败时不能访问成员，也没有新节点需要释放。 */
        return NULL;
    }

    /* malloc 不初始化内容，使用前分别设置成员。 */
    node->value = value;
    node->next = NULL;
    return node;
}
```

先成功分配、初始化，再修改原链表。这样分配失败时可以保持原链表不变。逐个节点分配无需计算元素数量乘积；若改为一次分配节点数组，则仍需考虑数量乘以元素大小的溢出。

### 遍历与查找

```c
/* head 是有效链表入口，允许为 NULL；打印需要 <stdio.h>。 */
const struct Node *current = head;

while (current != NULL)
{
    /* 先确认指向有效节点，再读取成员。 */
    printf("%d\n", current->value);

    /* 读取当前节点保存的后继地址，不移动或复制节点本身。 */
    current = current->next;
}
```

- `current = current->next` 等价于 `current = (*current).next`。
- 不能用 `current++` 寻找后继，因为节点不保证连续存放。
- 使用独立遍历指针可以保留 `head`，避免丢失链表入口。
- `const struct Node *` 限制通过该指针修改节点，但允许指针变量本身移动。
- 按值查找是在遍历过程中比较 `current->value`；找到返回节点地址，找不到返回 `NULL`。
- 查找返回的节点地址通常只是借用；节点被删除后，借用指针不能继续访问它。

### 头插与二级指针

```c
/*
 * 把一个独立的新节点接到链表头部。
 * head_ptr：调用方头指针变量的地址，要求非空。
 * node：已经成功创建的节点，要求非空且尚未加入其他链表。
 * 不分配内存，不返回结果；加入后由链表负责节点的释放。
 */
static void list_link_front(struct Node **head_ptr, struct Node *node)
{
    /* 先让新节点接住原链表，空链表时后继自然为 NULL。 */
    node->next = *head_ptr;

    /* 再修改调用方的入口，使新节点成为首节点。 */
    *head_ptr = node;
}
```

调用形式是 `list_link_front(&head, node)`。连续头插 10、20、30 后，顺序为 30 → 20 → 10。

```text
head_ptr → head → 首节点
           ↑
      *head_ptr 表示调用方的头指针变量
```

C 是值传递。传入 `head` 后给函数的局部指针重新赋值，不能改变调用方的 `head`；传入 `&head`，才能通过 `*head_ptr` 修改调用方的入口。也可以设计为返回新头指针，由调用方接收。

### 尾插与已知位置后插入

尾插时：空链表直接更新 `head`；非空链表先沿 `next` 找到尾节点，再把其 `next` 指向新节点，新节点的 `next` 为 `NULL`。连续尾插保持输入顺序。

两个循环条件的区别：

| 条件 | 用途 |
|------|------|
| `current != NULL` | 访问全部节点，结束时指针为空 |
| `current->next != NULL` | 寻找尾节点；必须先保证 current 有效 |

在已知节点后插入：

```c
/* position 是有效节点；node 是尚未链接的有效新节点。 */
node->next = position->next;  // 先保存后方连接
position->next = node;        // 再让前方连接新节点
```

两句直接交换会使新节点的 `next` 指向自己，形成错误自环。关键是先保留原后继地址。

### 删除第一个匹配节点

```c
/*
 * 删除第一个值等于 target 的节点，需要 <stdbool.h> 和 <stdlib.h>。
 * head_ptr：有效头指针变量的地址，链表可以为空。
 * target：要删除的整数。
 * 返回 true 表示已删除，false 表示未找到。
 * 节点必须是归该链表管理的动态分配对象。
 */
static bool list_remove_first(struct Node **head_ptr, int target)
{
    struct Node *previous = NULL;      // 首节点没有前驱
    struct Node *current = *head_ptr;  // 当前检查的节点

    while (current != NULL)
    {
        if (current->value == target)
        {
            if (previous == NULL)
            {
                /* 删除首节点：先把入口移到后继。 */
                *head_ptr = current->next;
            }
            else
            {
                /* 删除其他节点：让前驱跳过当前节点。 */
                previous->next = current->next;
            }

            /* 完成链接修改后再释放，之后不再读取 current。 */
            free(current);
            return true;
        }

        /* 保存旧的当前节点，再移动到后继。 */
        previous = current;
        current = current->next;
    }

    return false;
}
```

删除唯一节点时，头指针变为 `NULL`；删除尾节点时，前驱的 `next` 变为 `NULL`。存在重复值时，上述函数只删除第一个匹配节点。

### 销毁整条链表

```c
/*
 * 释放全部动态节点，无返回值。
 * head_ptr 必须指向有效头指针变量，允许链表为空。
 */
static void list_destroy(struct Node **head_ptr)
{
    struct Node *current = *head_ptr;

    while (current != NULL)
    {
        /* 释放前保存后继，否则释放后不能合法读取 next。 */
        struct Node *next_node = current->next;
        free(current);
        current = next_node;
    }

    /* 清空调用方入口；其他节点别名仍需调用方停止使用。 */
    *head_ptr = NULL;
}
```

中途创建失败也要清理已经加入链表的节点。不能只 `free(head)` 就认为释放了整条链表，`free` 不会自动沿指针递归释放。

## 12.3 双链表

### 节点与管理对象

以下为独立的双链表定义，不与前面的单链表 `struct Node` 同时定义在同一作用域。

```c
struct Node
{
    int value;          // 当前节点的数据
    struct Node *prev;  // 前驱地址；首节点为 NULL
    struct Node *next;  // 后继地址；尾节点为 NULL
};

struct List
{
    struct Node *head;  // 首节点地址，空链表时为 NULL
    struct Node *tail;  // 尾节点地址，空链表时为 NULL
};

/* 两个成员按指针语义初始化为空指针。 */
struct List list = {NULL, NULL};
```

- 空表：`head`、`tail` 都为空。
- 单节点：`head`、`tail` 指向同一个节点。
- 非空表：`head->prev == NULL`，`tail->next == NULL`。
- 对有效节点 `p`，若 `p->next` 非空，应有 `p->next->prev == p`；反方向同理。
- `a.next = &b` 不会自动设置 `b.prev`，两个方向必须分别维护。

函数接收 `struct List *list_ptr`，即可通过 `list_ptr->head` 修改调用方结构体的入口成员，不需要再把这个参数设计为二级指针。

### 双向遍历

正向从 `head` 开始，每次执行 `current = current->next`；反向从 `tail` 开始，每次执行 `current = current->prev`。两者都在 `current == NULL` 时结束。双向遍历不代表可以像数组一样按下标直接定位。

### 插入与边界处理

中间插入：

```c
/* left 和 right 是相邻有效节点，node 是独立的新节点。 */
node->prev = left;   // 新节点先记住前驱
node->next = right;  // 新节点再记住后继
left->next = node;   // 前驱连接新节点
right->prev = node;  // 后继反向连接新节点
```

头插：

```c
/* list_ptr 指向有效管理对象，node 为已创建的独立节点。 */
node->prev = NULL;
node->next = list_ptr->head;

if (list_ptr->head != NULL)
{
    /* 原首节点需要反向连接新节点。 */
    list_ptr->head->prev = node;
}
else
{
    /* 空表插入后，新节点同时是尾节点。 */
    list_ptr->tail = node;
}

list_ptr->head = node;  // 最后更新头指针
```

尾插与头插对称：先设置 `node->next = NULL`、`node->prev = list_ptr->tail`；非空时更新原尾节点的 `next`，空表时更新 `head`，最后更新 `tail`。

### 删除已知节点

```c
/*
 * 删除并释放 node，无返回值，需要 <stdlib.h>。
 * list_ptr 必须有效；node 必须是该链表中有效的动态节点。
 * 此接口不检查节点归属，调用方不能传入空指针或其他链表的节点。
 */
static void list_erase(struct List *list_ptr, struct Node *node)
{
    struct Node *left = node->prev;   // 记录前驱，可能为空
    struct Node *right = node->next;  // 记录后继，可能为空

    if (left != NULL)
    {
        left->next = right;       // 前驱跳过待删除节点
    }
    else
    {
        list_ptr->head = right;   // 删除首节点，更新入口
    }

    if (right != NULL)
    {
        right->prev = left;       // 后继反向跳过待删除节点
    }
    else
    {
        list_ptr->tail = left;    // 删除尾节点，更新尾指针
    }

    free(node);  // 两侧连接与入口更新后再释放
}
```

该逻辑统一处理首节点、尾节点、中间节点和唯一节点。删除唯一节点时，两个邻居均为空，两个入口也随之清空。

销毁时只需沿一个方向逐个保存后继并释放，最后把 `head`、`tail` 都设为空。一个节点有两个链接指针，并不意味着需要释放两次。

## 12.4 复杂度与使用场景

下面讨论基本访问和链接操作，内存分配器自身的耗时另计。`n` 为节点数。

| 操作 | 单链表 | 双链表 |
|------|------|------|
| 按值查找 | O(n) | O(n) |
| 按位置访问 | 最坏 O(n) | 最坏 O(n)，可以选择从两端开始 |
| 头插 | O(1) | O(1) |
| 尾插 | 只有 head 时 O(n)，维护 tail 时 O(1) | 维护 tail 时 O(1) |
| 在已知节点后插入 | O(1) | O(1) |
| 删除已知节点 | 通常还需寻找前驱；已有前驱时 O(1) | O(1)，直接访问 prev |
| 按值删除 | O(n)，包含查找 | O(n)，包含查找 |
| 销毁 | O(n) | O(n) |

不能笼统地说“链表插入删除都是 O(1)”，必须区分查找位置与修改链接。

- 单链表可用于栈、任务队列；维护头尾指针可实现高效入队、出队。
- 双链表适合双端队列、前后导航、频繁移除或移动已知节点的容器。
- 图像像素、连续 IMU 采样通常更适合数组或环形缓冲区。
- 每个节点都需要链接指针；双链表比单链表多一个指针成员，实际对象大小受平台和对齐影响。
- 动态分配存在开销和碎片；嵌入式场景可以用预分配节点池，不必每次调用 `malloc`。

## 12.5 常见错误与后续练习检查点

| 错误 | 原因或处理方法 |
|------|------|
| 用指针自增寻找下一节点 | 节点不保证连续，必须沿 next/prev 走 |
| 修改局部 head 副本后期望调用方改变 | 传入 &head，或返回新入口，或使用管理结构体 |
| 插入时覆盖原后继 | 先保留后继，再修改前方连接 |
| 先 free 再读取 next | 先保存需要的地址，再释放 |
| 双链表只更新一个方向 | 同步维护 next 和 prev |
| 空表插入后只更新一个入口 | 同时维护 head 和 tail |
| free 后继续使用查找得到的地址 | 别名指针已经悬空，必须停止使用 |
| 只清空 head 而没有释放节点 | 丢失入口可能造成内存泄漏 |
| 释放局部节点或重复释放 | 明确分配方式和释放责任 |

后续练习应覆盖空表、单节点、多节点、首尾和中间位置、查找失败、重复值、分配失败清理及销毁后入口状态。双链表还应核对正反遍历是否对应、相邻节点连接是否一致。

## 练习记录

### Q_1：单链表节点计数

- 文件：[Q_1/main.c](../12_structures_and_pointers/Q_1/main.c)。
- 原代码遍历、累加正确，但固定返回 0；修正为返回 count。
- 测试：空表、单节点、四节点、从第二节点开始、重复计数，以及入口、链接和数据保持不变，共 6 项检查通过。
- 时间 O(n)，额外空间 O(1)。当前 int 计数要求节点数不超过 INT_MAX，链表必须有效且无环。

### Q_2：在无序单链表中按值查找

- 文件：[Q_2/main.c](../12_structures_and_pointers/Q_2/main.c)。
- 原代码缺少 current = current->next，首节点不匹配时会死循环；已补上指针移动。
- 返回第一个匹配节点的地址，空表或找不到时返回 NULL。
- 9 个查找用例和 1 项链表不变检查通过，覆盖首、中、尾、负数、零、重复值、空表及未找到。直接比较返回地址，确认节点身份。
- 时间最坏 O(n)，额外空间 O(1)。返回地址为借用，节点失效后不能继续访问。

### Q_3：反转单链表

- 文件：[Q_3/main.c](../12_structures_and_pointers/Q_3/main.c)。
- 原非空链表算法正确；补上 first == NULL 的判断，避免读取空指针的 next。
- 修改链接之前先保存原后继；原首节点的 next 设为 NULL，最终成为尾节点。
- 调用方使用 head = sll_reverse(head) 接收新入口；仅修改形参 first 不会更新调用方头指针。
- 0、1、2、5 个节点的反转及再次反转恢复，共 8 项检查通过。按地址验证顺序，同时检查数据不变和尾部为空。
- 原地修改链接，时间 O(n)，额外空间 O(1)。

### Q_4：按节点地址移除单链表节点

- 文件：[Q_4/main.c](../12_structures_and_pointers/Q_4/main.c)。
- 修正参数为 struct NODE **rootp；删除首节点通过 *rootp 更新调用方入口。
- 使用 current == node 比较身份，不能按 value 比较。补上空参数处理，删除循环结束后对 NULL 的非法访问。
- 共 10 组测试通过，覆盖空表、单节点、首中尾、重复值、外部同值节点、空参数；成功后还检查再次移除返回 0。
- 传节点地址能精确区分同值节点，不依赖数据比较规则，也便于复用查找结果。但单链表仍需遍历确认归属及前驱，最坏 O(n)。

### Q_5：按节点地址移除双链表节点

- 文件：[Q_5/main.c](../12_structures_and_pointers/Q_5/main.c)。
- 按题目改用 struct NODE *rootp，指向管理首尾的根节点；rootp->next 保存首节点，rootp->prev 保存尾节点。
- 根节点不参与数据链，首节点 prev 和尾节点 next 为 NULL；空表时根节点两个链接均为空，不采用循环哨兵结构。
- 原 current 未初始化，已从 rootp->next 开始；删除时同时维护前驱 next、后继 prev 及必要的根节点首尾指针。
- 共 11 组测试通过，覆盖空表、单节点、首中尾、重复值、另一条链表的节点、空参数和根节点；核对正反向节点地址顺序，检查再次移除失败。
- 因题目要求确认目标属于链表，总体最坏 O(n)；找到目标后的断链为 O(1)，额外空间 O(1)。

### 移除与释放的约定、验证范围

Q_4、Q_5 只摘下节点并清空其链接，不调用 free；节点仍由调用方负责复用或释放。测试使用局部节点，不能对其调用 free。

五题均使用 MinGW64 GCC，以 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 编译并运行，各题测试通过且无编译警告。测试不代表支持无效地址或已损坏、有环的链表；本组练习未涉及动态分配失败路径。
