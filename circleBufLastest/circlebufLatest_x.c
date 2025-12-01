#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "circlebufLatest_x.h"


static CircularBufferLatest * ps_circbuf = NULL;

// 创建循环缓冲区
CircularBufferLatest* _cbLatest_create(int capacity) {
    if (capacity <= 0) return NULL;
    
    CircularBufferLatest *cb = malloc(sizeof(CircularBufferLatest));
    if (!cb) return NULL;
    
    cb->buffer = malloc(capacity*sizeof(void*));
    if (!cb->buffer) {
        free(cb);
        return NULL;
    }
    memset((char*)cb->buffer,0,capacity*sizeof(void*));
    
    cb->capacity = capacity;
    cb->head = 0;
    cb->size = 0;
    
    return cb;
}

// 销毁循环缓冲区
void _cbLatest_destroy(CircularBufferLatest *cb) {
    if (!cb) return;
    
    if (cb->buffer) {
        free(cb->buffer);
    }
    free(cb);
}

// 添加数据
void _cbLatest_put(CircularBufferLatest *cb, void *data) {
    if (!cb) return;
    
    cb->buffer[cb->head] = data;
    cb->head = (cb->head + 1) % cb->capacity;
    
    if (cb->size < cb->capacity) {
        cb->size++;
    }
}

// 获取最新数据
void* _cbLatest_get_latest(CircularBufferLatest *cb) {
    if (!cb || cb->size == 0) return NULL;
    
    // 最新数据在head的前一个位置
    int latest_index = (cb->head - 1 + cb->capacity) % cb->capacity;
    return cb->buffer[latest_index];
}

// 获取最新的n个数据，结果存储在result数组中
// 返回实际获取的数据个数
int _cbLatest_get_n_latest(CircularBufferLatest *cb, void **result, int n) {
    if (!cb || !result || n <= 0 || cb->size == 0) {
        return 0;
    }
    
    int count = (n < cb->size) ? n : cb->size;
    
    for (int i = 0; i < count; i++) {
        int index = (cb->head - 1 - i + cb->capacity) % cb->capacity;
        result[i] = cb->buffer[index];
    }
    
    return count;
}

// 获取所有数据，按最新到最旧排序
int _cbLatest_get_all_latest_first(CircularBufferLatest *cb, void **result) {
    if (!cb || !result || cb->size == 0) {
        return 0;
    }
    
    for (int i = 0; i < cb->size; i++) {
        int index = (cb->head - 1 - i + cb->capacity) % cb->capacity;
        result[i] = cb->buffer[index];
    }
    
    return cb->size;
}

// 检查是否为空
bool _cbLatest_is_empty(CircularBufferLatest *cb) {
    return (!cb || cb->size == 0);
}

// 检查是否已满
bool _cbLatest_is_full(CircularBufferLatest *cb) {
    return (cb && cb->size == cb->capacity);
}

// 获取当前大小
int _cbLatest_current_size(CircularBufferLatest *cb) {
    return cb ? cb->size : 0;
}

// 打印缓冲区状态（用于调试）
void _cbLatest_print_status(CircularBufferLatest *cb) {
    if (!cb) {
        printf("CircularBufferLatest: NULL\n");
        return;
    }
    
    printf("CircularBufferLatest(capacity=%d, size=%d, head=%d)\n", 
           cb->capacity, cb->size, cb->head);
}
#if 1
// init CircleBuffLatest
// return : 0-success, other-fail
int CircleBuffLatest_Init(int capacity)
{
    ps_circbuf = _cbLatest_create(capacity);
    if (ps_circbuf)
        return 0;
    else
        return -1;
}

int CircleBuffLatest_Put(void *pdata)
{
    int ret = 0;
    if(ps_circbuf)
    {
        _cbLatest_put(ps_circbuf, pdata);
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    return ret;
}

void* CircleBuffLatest_Get()
{
    int ret = 0;
    if(ps_circbuf)
    {
        return _cbLatest_get_latest(ps_circbuf);
    }
    else
    {
        return NULL;
    }
    
}

int CircleBuffLatest_GetN(void **result, int n)
{
    if(ps_circbuf)
    {
        return _cbLatest_get_n_latest(ps_circbuf, result, n);
    }
    else
    {
        return -1;
    }
    
}
#endif

// ==================== 使用示例 ====================

// 简单的字符串数据测试
void test_string_data() {
    printf("=== 字符串数据测试 ===\n");
    
    CircularBufferLatest *cb = _cbLatest_create(5);
    
    // 添加数据
    char *data[] = {"data_1", "data_2", "data_3", "data_4", "data_5", "data_6", "data_7"};
    
    for (int i = 0; i < 7; i++) {
        _cbLatest_put(cb, data[i]);
        printf("添加 %s: ", data[i]);
        _cbLatest_print_status(cb);
        
        char *latest = (char*)_cbLatest_get_latest(cb);
        printf("最新数据: %s\n\n", latest ? latest : "NULL");
    }
    
    // 获取最新的3个数据
    void *result[10];
    int count = _cbLatest_get_n_latest(cb, result, 3);
    printf("最新3个数据: ");
    for (int i = 0; i < count; i++) {
        printf("%s ", (char*)result[i]);
    }
    printf("\n");
    
    // 获取所有数据
    count = _cbLatest_get_all_latest_first(cb, result);
    printf("所有数据(最新到最旧): ");
    for (int i = 0; i < count; i++) {
        printf("%s ", (char*)result[i]);
    }
    printf("\n");
    
    printf("是否为空: %s\n", _cbLatest_is_empty(cb) ? "是" : "否");
    printf("是否已满: %s\n", _cbLatest_is_full(cb) ? "是" : "否");
    
    _cbLatest_destroy(cb);
}

// 整数数据测试
void test_int_data() {
    printf("\n=== 整数数据测试 ===\n");
    
    CircularBufferLatest *cb = _cbLatest_create(4);
    
    // 创建一些整数数据
    int *nums[11];
    for (int i = 0; i < 11; i++) {
        nums[i] = malloc(sizeof(int));
        *(nums[i]) = (i + 1) * 10;
    }
    
    // 添加数据
    for (int i = 0; i < 11; i++) {
        _cbLatest_put(cb, nums[i]);
        printf("添加 %d: ", *(nums[i]));
        _cbLatest_print_status(cb);
        
        int *latest = (int*)_cbLatest_get_latest(cb);
        printf("最新数据: %d\n\n", latest ? *latest : -1);
    }
    
    // 获取最新的2个数据
    void *result[10];
    int count = _cbLatest_get_n_latest(cb, result, 2);
    printf("最新2个数据: ");
    for (int i = 0; i < count; i++) {
        printf("%d ", *((int*)result[i]));
    }
    printf("\n");
    
    // 清理内存
    for (int i = 0; i < 11; i++) {
        free(nums[i]);
    }
    
    _cbLatest_destroy(cb);
}

#if  1
void test_3()
{
    int num[10]={0};
    int i=0;
    int * pintGet = NULL;
    int * pintGetN_2[2] = {NULL};
    for (i = 0; i < sizeof(num)/sizeof(num[0]); i++)
    {
        num[i]=i*10;
    }
    

    CircleBuffLatest_Init(5);

    for (i = 0; i < sizeof(num)/sizeof(num[0]); i++)
    {
        CircleBuffLatest_Put((void *)&num[i]);
    }
    
    pintGet = (int*)CircleBuffLatest_Get();

    CircleBuffLatest_GetN((void**)&pintGetN_2,2);

    printf("\r\nTest 3:\r\n");
    printf("pGet : %d\r\n",*pintGet);
    printf("*intGetN_2[0] : %d\r\n",*pintGetN_2[0]);
    printf("*pintGetN_2[1] : %d\r\n",*pintGetN_2[1]);

}
#endif 

int main() {
    test_string_data();
    test_int_data();
    test_3();
    return 0;
}