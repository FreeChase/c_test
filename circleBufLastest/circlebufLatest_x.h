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



#ifdef __cplusplus
}
#endif


#endif