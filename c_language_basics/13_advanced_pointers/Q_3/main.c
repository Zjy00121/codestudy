#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

/*
 * 通用冒泡排序，无返回值。
 * nums：可写数组首地址；num：元素个数；size：单个元素的字节数。
 * compare：比较两个元素地址，返回负数/零/正数表示小于/等于/大于。
 * 回调必须给出一致的排序关系，不得修改数组。
 * 调用方保证数组容量足够、size 与元素类型一致；函数无法从地址验证容量。
 * 空参数、零大小、大小乘积溢出时直接返回；少于两个元素无需排序。
 * 不分配、不释放内存，通过 unsigned char 访问并交换完整对象表示。
 */
void sort(void *nums, size_t num, size_t size,
          int (*compare)(const void *left, const void *right))
{
    if (num < 2 || nums == NULL || compare == NULL || size == 0) {
        return;
    }
    if (num > SIZE_MAX / size) {
        return; // 先检查乘法溢出，再计算元素的字节偏移
    }
    unsigned char *bytes = nums; // 标准 C 不能直接对 void * 做加法
    for (size_t end = num; end > 1; --end) {
        int swapped = 0; // 一轮没有交换，说明已经有序
        for (size_t j = 1; j < end; ++j) {
            unsigned char *left = bytes + (j - 1) * size;
            unsigned char *right = bytes + j * size;
            if (compare(left, right) > 0) {
                /* 必须交换整个元素，不能只交换第一个字节。 */
                for (size_t k = 0; k < size; ++k) {
                    unsigned char temporary = left[k];
                    left[k] = right[k];
                    right[k] = temporary;
                }
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

/* 比较 int 元素；不使用相减，避免 INT_MIN、INT_MAX 等值引发溢出。 */
static int compare_int(const void *left, const void *right)
{
    const int *a = left;
    const int *b = right;
    if (*a < *b) { return -7; }
    if (*a > *b) { return 7; } // 特意不用 1，检查 sort 是否只判断符号
    return 0;
}

/* 调换参数实现降序，无需对任意比较结果取负。 */
static int compare_descending(const void *left, const void *right)
{
    return compare_int(right, left);
}

/* 比较不含 NaN 的 double 元素；本例未定义 NaN 的排序策略。 */
static int compare_double(const void *left, const void *right)
{
    const double *a = left;
    const double *b = right;
    if (*a < *b) { return -1; }
    if (*a > *b) { return 1; }
    return 0;
}

/* 元素是字符串指针，传入的是指针元素的地址，因此需要再解引用。 */
static int compare_strings(const void *left, const void *right)
{
    const char *const *a = left;
    const char *const *b = right;
    return strcmp(*a, *b);
}

/* 元素本身是以空字符结尾的字符数组，地址可直接作为字符串首地址。 */
static int compare_rows(const void *left, const void *right)
{
    return strcmp(left, right); // 包装成正确的通用回调类型，不强制转换函数指针
}

struct Record {
    int key;       // 比较使用的关键字
    char label[8]; // 必须随整个元素一起移动的数据
};

/* 只比较 key，相同 key 在本稳定冒泡排序中保持原顺序。 */
static int compare_records(const void *left, const void *right)
{
    const struct Record *a = left;
    const struct Record *b = right;
    return compare_int(&a->key, &b->key);
}

/* 输出测试名称与结果，返回本项失败数：成功为 0，失败为 1。 */
static int check(const char *name, int passed)
{
    printf("%s: %s\n", name, passed ? "PASS" : "FAIL");
    return passed ? 0 : 1;
}

int main(void)
{
    int failures = 0; // 累计失败次数，决定最终退出状态
    struct {
        const char *name; // 测试名称
        size_t count;    // 本项有效元素数
        int values[6];   // 待排序数据
        int expected[6]; // 手工给出的预期顺序
    } cases[] = {
        {"Empty", 0, {0}, {0}},
        {"Single", 1, {42}, {42}},
        {"Sorted", 4, {-2, 0, 1, 9}, {-2, 0, 1, 9}},
        {"Reversed", 4, {9, 1, 0, -2}, {-2, 0, 1, 9}},
        {"Duplicates", 6, {3, -1, 0, 3, -1, 2}, {-1, -1, 0, 2, 3, 3}},
        {"Equal", 4, {5, 5, 5, 5}, {5, 5, 5, 5}},
        {"Integer limits", 4, {INT_MAX, 0, INT_MIN, -1}, {INT_MIN, -1, 0, INT_MAX}}
    };
    size_t case_count = sizeof cases / sizeof cases[0];
    for (size_t t = 0; t < case_count; ++t) {
        sort(cases[t].values, cases[t].count, sizeof cases[t].values[0], compare_int);
        int passed = 1;
        for (size_t i = 0; i < cases[t].count; ++i) {
            if (cases[t].values[i] != cases[t].expected[i]) { passed = 0; }
        }
        failures += check(cases[t].name, passed);
    }

    int descending[] = {1, 3, 2};
    sort(descending, 3, sizeof descending[0], compare_descending);
    failures += check("Descending", descending[0] == 3 && descending[1] == 2 && descending[2] == 1);

    double fractions[] = {2.5, -1.25, 0.0, 2.5};
    sort(fractions, 4, sizeof fractions[0], compare_double);
    failures += check("Double", fractions[0] == -1.25 && fractions[1] == 0.0 &&
                      fractions[2] == 2.5 && fractions[3] == 2.5);

    /* 排序移动指针元素，不修改字面量，不需要申请或释放内存。 */
    const char *words[] = {"motor", "lidar", "imu", ""};
    sort(words, 4, sizeof words[0], compare_strings);
    failures += check("String pointers", strcmp(words[0], "") == 0 && strcmp(words[1], "imu") == 0 &&
                      strcmp(words[2], "lidar") == 0 && strcmp(words[3], "motor") == 0);

    struct Record records[] = {{2, "first"}, {1, "low"}, {2, "second"}};
    sort(records, 3, sizeof records[0], compare_records);
    /* 按成员检查，不对结构体填充字节做内容相等判断。 */
    failures += check("Records and stability", records[0].key == 1 && strcmp(records[0].label, "low") == 0 &&
                      records[1].key == 2 && strcmp(records[1].label, "first") == 0 &&
                      records[2].key == 2 && strcmp(records[2].label, "second") == 0);

    /* 300 个元素用于检查数量和下标未被 char 截断。 */
    int many[300];
    size_t many_count = sizeof many / sizeof many[0];
    for (size_t i = 0; i < many_count; ++i) {
        many[i] = (int)(many_count - i); // 值在 1～300 内，可由 int 表示
    }
    sort(many, many_count, sizeof many[0], compare_int);
    int many_ok = 1;
    for (size_t i = 0; i < many_count; ++i) {
        if (many[i] != (int)(i + 1)) { many_ok = 0; }
    }
    failures += check("300 elements", many_ok);

    /* 单个元素为 300 字节，首尾哨兵检查越界写入。 */
    struct {
        unsigned char before; // 排序区域之前的哨兵
        char rows[2][300];    // 两个宽字符数组元素，并非宽字符类型 wchar_t
        unsigned char after; // 排序区域之后的哨兵
    } wide = {0x5A, {"z", "a"}, 0xA5};
    wide.rows[0][299] = 'Z'; // 字符串结束符后也属于元素，仍然必须交换
    wide.rows[1][299] = 'A';
    sort(wide.rows, 2, sizeof wide.rows[0], compare_rows);
    failures += check("Wide elements and boundaries", wide.before == 0x5A && wide.after == 0xA5 &&
                      strcmp(wide.rows[0], "a") == 0 && strcmp(wide.rows[1], "z") == 0 &&
                      wide.rows[0][299] == 'A' && wide.rows[1][299] == 'Z');

    int unchanged[] = {2, 1};
    sort(NULL, 0, sizeof(int), compare_int); // 空输入不访问内存
    sort(NULL, 2, sizeof(int), compare_int);
    sort(unchanged, 2, 0, compare_int);
    sort(unchanged, 2, sizeof unchanged[0], NULL);
    sort(unchanged, SIZE_MAX, 2, compare_int); // 应在访问数组前发现大小溢出
    failures += check("Invalid arguments", unchanged[0] == 2 && unchanged[1] == 1);

    if (failures != 0) {
        printf("Failed checks: %d\n", failures);
        return EXIT_FAILURE;
    }
    printf("All tests passed.\n");
    return EXIT_SUCCESS;
}
