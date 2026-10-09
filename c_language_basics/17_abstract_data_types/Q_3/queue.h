#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include <stddef.h>

/* 模块管理一个整数链式队列，初始为空；节点均由模块拥有和释放。
 * 入队成功返回 true，分配失败或数量溢出返回 false，原队列保持不变。 */
bool enqueue(int value);
/* out_value 指向调用方 int；空队列或 NULL 输出返回 false，不修改输出。 */
bool dequeue(int *out_value);
/* 只读取队首，不移除；参数和失败规则同 dequeue。 */
bool queue_peek(int *out_value);
bool queue_is_empty(void);
size_t queue_size(void);
/* 释放全部节点并恢复空状态，可重复调用。 */
void destroy_queue(void);
#endif
