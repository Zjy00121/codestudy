#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stddef.h>

/* 本模块管理一个整数栈；初始为空且容量为零。 */
/* 新长度单位为元素个数；成功为 true，失败保留原栈。
 * 不允许缩小到有效数量以下；空栈可以缩到零并释放空间。 */
bool resize_stack(size_t new_length);
/* 满栈入栈失败，不自动扩容，以便显式练习 resize_stack。 */
bool push(int value);
/* 输出参数指向调用方变量；失败不修改输出。 */
bool pop(int *out_value);
bool peek(int *out_value);
size_t stack_size(void);
size_t stack_capacity(void);
/* 释放模块拥有的空间并恢复空状态，可以重复调用。 */
void destroy_stack(void);

#endif
