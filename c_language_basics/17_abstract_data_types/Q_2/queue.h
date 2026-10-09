#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include <stddef.h>

/* 模块拥有一个整数队列，初始容量为零。新长度单位为元素个数。
 * 成功返回 true；过小容量、大小溢出或分配失败返回 false，原队列不变。
 * 非空队列不能缩到零；空队列缩到零释放内存。 */
bool resize_queue(size_t new_length);
/* 队尾加入，队首取出；满时不自动扩容，失败不修改输出。 */
bool enqueue(int value);
bool dequeue(int *out_value);
bool queue_peek(int *out_value);
size_t queue_size(void);
size_t queue_capacity(void);
/* 释放模块拥有的数组，恢复初始状态，可重复调用。 */
void destroy_queue(void);
#endif
