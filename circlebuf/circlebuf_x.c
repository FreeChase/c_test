#include "circlebuf_x.h"


// 初始化循环缓冲区
void initializeBuffer(CircularBuffer * cBuffer, char * pbuf, int size) {
    cBuffer->buffer = pbuf;
    cBuffer->size = size;
    cBuffer->head = 0;
    cBuffer->tail = 0;
    return;
}

// 写入数据到循环缓冲区
int writeBuffer(CircularBuffer *cBuffer, char data) {
    if ((cBuffer->tail + 1) % cBuffer->size == cBuffer->head) {
        // 缓冲区已满
        return 0; // 写入失败
    } else {
        cBuffer->buffer[cBuffer->tail] = data;
        cBuffer->tail = (cBuffer->tail + 1) % cBuffer->size;
        return 1; // 写入成功
    }
}

// 从循环缓冲区读取数据
int readBuffer(CircularBuffer *cBuffer, int *data) {
    if (cBuffer->head == cBuffer->tail) {
        // 缓冲区为空
        return 0; // 读取失败
    } else {
        *data = cBuffer->buffer[cBuffer->head];
        cBuffer->head = (cBuffer->head + 1) % cBuffer->size;
        return 1; // 读取成功
    }
}

