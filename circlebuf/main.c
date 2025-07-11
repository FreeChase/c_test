#include <stdio.h>
#include <stdlib.h>
#include "circlebuf_x.h"

#define BUFFER_SIZE 10

int main() {
    char buf[129]={0};

    CircularBuffer cBuffer ;
    initializeBuffer(&cBuffer, buf, sizeof(buf) );

    // 写入数据
    for (int i = 0; i <= sizeof(buf); i++) {
        if (writeBuffer(&cBuffer, i)) {
            printf("write data: %d\n", i);
        } else {
            printf("buff is full,write fail\n");
        }
    }

    // 读取数据
    int data;
    while (readBuffer(&cBuffer, &data)) {
        printf("read data: %d\n", data);
    }


    return 0;
}
