#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>  // strlen：在 main 中检查和展示返回的字符串

char *read_string(void);

/*
 * 从标准输入读取一行文本，返回动态分配的字符串。
 * 空格和制表符属于字符串内容；换行结束本次读取，不保存换行符。
 * EOF 也会结束读取；空行或一开始就 EOF 时返回空字符串 ""。
 * 返回的内存由调用者 free；分配失败、大小超限、读取错误返回 NULL。
 * 本函数读取普通文本，不接受内嵌 NUL 字节，因为它会提前终止 C 字符串。
 * 不设固定长度上限，但仍受到可用内存和 size_t 范围的限制。
 */
char *read_string(void)
{
    // capacity 是总字节容量，包含最后一个 '\0' 的位置。
    // 16 只是起始容量，不是允许输入的最大长度。
    size_t capacity = 16;
    size_t length = 0;  // 已保存的字符字节数，不包含 '\0'
    char *text = malloc(capacity);
    if (text == NULL) {
        return NULL;
    }

    // getchar 返回 int，以便同时表示 unsigned char 的所有值和 EOF。
    // 如果先存入 char，可能丢失 EOF 与普通字符之间的区别。
    int ch;
    while ((ch = getchar()) != EOF && ch != '\n') {
        if (ch == '\0') {
            free(text);
            return NULL;
        }

        // 必须同时容纳“新字符”和“结尾的 '\0'”。
        // length == capacity - 1 时，只剩终止符的位置，需要先扩容。
        if (length == capacity - 1) {
            if (capacity == SIZE_MAX) {
                free(text);
                return NULL;  // 无法再用 size_t 表示更大的容量
            }

            size_t new_capacity;
            if (capacity > SIZE_MAX / 2) {
                new_capacity = SIZE_MAX;  // 避免 capacity * 2 溢出
            } else {
                new_capacity = capacity * 2;
            }

            // char 大小为 1 字节，所以容量本身就是所需字节数。
            // realloc 可能搬迁；先用 tmp 接收，失败时还能释放旧块。
            char *tmp = realloc(text, new_capacity);
            if (tmp == NULL) {
                free(text);
                return NULL;
            }
            text = tmp;
            capacity = new_capacity;
        }

        // getchar 的结果已经排除了 EOF，现在把该字符写入动态内存。
        text[length] = (char)ch;
        ++length;
    }

    // EOF 可能表示输入结束，也可能伴随读取错误，必须区分。
    if (ferror(stdin)) {
        free(text);
        return NULL;
    }

    // 即使没有读到字符，也会执行 text[0] = '\0'，得到有效空字符串。
    // 每次扩容都预留了这个位置，因此不会越界。
    text[length] = '\0';
    return text;
}

int main(void)
{
    // 调用函数进行交互测试：输入一行文本后按 Enter 即可结束。
    // 不用固定大小的临时数组，字符直接复制到动态内存中。
    char *copy = read_string();
    if (copy == NULL) {
        fprintf(stderr, "Read or allocation failed.\n");
        return EXIT_FAILURE;
    }

    // strlen 返回字节数；对于多字节编码的中文，它不等于汉字个数。
    // 输出长度及完整内容，方便核对是否保留空格、是否发生截断。
    printf("Length: %zu\n", strlen(copy));
    printf("Copy: %s\n", copy);

    // 函数返回后内存仍有效；最终释放责任属于调用者 main。
    free(copy);
    copy = NULL;
    return EXIT_SUCCESS;
}
