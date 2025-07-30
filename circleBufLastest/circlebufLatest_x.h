#ifndef _CIRCLEBUFLATEST_X_H_
#define _CIRCLEBUFLATEST_X_H_

#ifdef __cplusplus
extern "C" {
#endif


// 循环缓冲区结构体
typedef struct {
    void **buffer;      // 数据缓冲区（存储指针）
    int capacity;       // 缓冲区容量
    int head;          // 指向最新数据位置的下一个位置
    int size;          // 当前数据量
} CircularBufferLatest;


CircularBufferLatest* _cbLatest_create(int capacity);

void _cbLatest_destroy(CircularBufferLatest *cb);

// 添加数据
void _cbLatest_put(CircularBufferLatest *cb, void *data);

// 获取最新数据
void* _cbLatest_get_latest(CircularBufferLatest *cb);
// 获取最新的n个数据，结果存储在result数组中
// 返回实际获取的数据个数
int _cbLatest_get_n_latest(CircularBufferLatest *cb, void **result, int n);
// 获取所有数据，按最新到最旧排序
int _cbLatest_get_all_latest_first(CircularBufferLatest *cb, void **result);
// 检查是否为空
bool _cbLatest_is_empty(CircularBufferLatest *cb);

// 检查是否已满
bool _cbLatest_is_full(CircularBufferLatest *cb);
// 获取当前大小
int _cbLatest_current_size(CircularBufferLatest *cb);

// 打印缓冲区状态（用于调试）
void _cbLatest_print_status(CircularBufferLatest *cb);


int CircleBuffLatest_Init(int capacity);

int CircleBuffLatest_Put(void *pdata);

void* CircleBuffLatest_Get();

#ifdef __cplusplus
}
#endif


#endif