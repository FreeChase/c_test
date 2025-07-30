#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 内存分配记录结构
typedef struct malloc_record {
    void *ptr;              // 分配的内存地址
    size_t size;            // 分配大小
    uint32_t caller_stack[8]; // 调用堆栈（保存8层）
    uint32_t timestamp;     // 时间戳
    struct malloc_record *next;
} malloc_record_t;

// 全局记录链表
static malloc_record_t *g_malloc_records = NULL;
static int g_record_count = 0;

// ARM Cortex-A53 堆栈回溯函数
static int backtrace_arm(uint32_t *buffer, int max_frames) {
    uint32_t fp;  // Frame Pointer
    uint32_t lr;  // Link Register
    uint32_t pc;  // Program Counter
    int frame_count = 0;
    
    // 获取当前Frame Pointer (R11)
    __asm__ volatile("mov %0, r11" : "=r"(fp));
    
    // 获取当前Link Register (R14)
    __asm__ volatile("mov %0, lr" : "=r"(lr));
    
    // 第一个返回地址就是当前的LR
    if (frame_count < max_frames) {
        buffer[frame_count++] = lr;
    }
    
    // 遍历调用栈
    while (fp != 0 && frame_count < max_frames) {
        // 检查fp是否有效（4字节对齐，在合理范围内）
        if ((fp & 0x3) != 0 || fp < 0x1000 || fp > 0x80000000) {
            break;
        }
        
        // ARM AAPCS调用约定：
        // fp指向的位置存储上一个fp
        // fp+4位置存储返回地址(lr)
        uint32_t *frame = (uint32_t*)fp;
        
        // 读取返回地址
        lr = frame[1];  // fp+4位置
        if (lr == 0) break;
        
        buffer[frame_count++] = lr;
        
        // 获取上一个frame pointer
        fp = frame[0];  // fp位置
    }
    
    return frame_count;
}

// 符号查找函数（需要根据实际情况实现）
static const char* get_symbol_name(uint32_t addr) {
    // 这里需要根据你的系统实现符号查找
    // 可以使用符号表或者预定义的地址映射
    
    // 示例：简单的地址范围判断
    if (addr >= 0x08000000 && addr <= 0x08100000) {
        return "main_app";
    } else if (addr >= 0x08100000 && addr <= 0x08200000) {
        return "driver_module";
    } else if (addr >= 0x08200000 && addr <= 0x08300000) {
        return "protocol_stack";
    }
    
    return "unknown";
}

// malloc钩子函数
void malloc_hook(void *ptr, size_t size) {
    if (ptr == NULL) return;
    
    // 分配记录结构
    malloc_record_t *record = (malloc_record_t*)malloc(sizeof(malloc_record_t));
    if (record == NULL) return;
    
    record->ptr = ptr;
    record->size = size;
    record->timestamp = OSTimeGet(); // ucos时间戳
    
    // 获取调用堆栈
    int stack_depth = backtrace_arm(record->caller_stack, 8);
    
    // 清空未使用的堆栈位置
    for (int i = stack_depth; i < 8; i++) {
        record->caller_stack[i] = 0;
    }
    
    // 添加到链表
    record->next = g_malloc_records;
    g_malloc_records = record;
    g_record_count++;
    
    // 打印分配信息
    printf("MALLOC: ptr=%p, size=%zu, caller_stack:\n", ptr, size);
    for (int i = 0; i < stack_depth; i++) {
        printf("  [%d] 0x%08x (%s)\n", i, 
               record->caller_stack[i], 
               get_symbol_name(record->caller_stack[i]));
    }
    printf("\n");
}

// free钩子函数
void free_hook(void *ptr) {
    if (ptr == NULL) return;
    
    // 从记录中移除
    malloc_record_t **current = &g_malloc_records;
    while (*current) {
        if ((*current)->ptr == ptr) {
            malloc_record_t *to_remove = *current;
            *current = (*current)->next;
            free(to_remove);
            g_record_count--;
            break;
        }
        current = &(*current)->next;
    }
}

// 内存使用情况分析
void analyze_memory_usage(void) {
    printf("=== Memory Usage Analysis ===\n");
    printf("Total malloc records: %d\n\n", g_record_count);
    
    // 按调用者统计
    typedef struct {
        uint32_t caller_addr;
        int count;
        size_t total_size;
    } caller_stats_t;
    
    caller_stats_t stats[100];
    int stats_count = 0;
    
    malloc_record_t *record = g_malloc_records;
    while (record) {
        uint32_t caller = record->caller_stack[0]; // 直接调用者
        
        // 查找是否已存在
        int found = -1;
        for (int i = 0; i < stats_count; i++) {
            if (stats[i].caller_addr == caller) {
                found = i;
                break;
            }
        }
        
        if (found >= 0) {
            stats[found].count++;
            stats[found].total_size += record->size;
        } else if (stats_count < 100) {
            stats[stats_count].caller_addr = caller;
            stats[stats_count].count = 1;
            stats[stats_count].total_size = record->size;
            stats_count++;
        }
        
        record = record->next;
    }
    
    // 按内存使用量排序并显示
    printf("Top memory consumers:\n");
    for (int i = 0; i < stats_count; i++) {
        printf("Caller: 0x%08x (%s) - Count: %d, Total: %zu bytes\n",
               stats[i].caller_addr,
               get_symbol_name(stats[i].caller_addr),
               stats[i].count,
               stats[i].total_size);
    }
}

// 显示详细的内存分配记录
void dump_all_malloc_records(void) {
    printf("=== All Malloc Records ===\n");
    
    malloc_record_t *record = g_malloc_records;
    int index = 0;
    
    while (record) {
        printf("Record %d:\n", index++);
        printf("  Address: %p\n", record->ptr);
        printf("  Size: %zu bytes\n", record->size);
        printf("  Timestamp: %u\n", record->timestamp);
        printf("  Call stack:\n");
        
        for (int i = 0; i < 8 && record->caller_stack[i] != 0; i++) {
            printf("    [%d] 0x%08x (%s)\n", i, 
                   record->caller_stack[i],
                   get_symbol_name(record->caller_stack[i]));
        }
        printf("\n");
        
        record = record->next;
    }
}

// 在ucos中注册钩子函数的示例
void setup_malloc_hooks(void) {
    // 注册malloc和free钩子
    // 具体实现取决于你的malloc实现
    
    // 如果使用标准库malloc，可能需要：
    // __malloc_hook = malloc_hook;
    // __free_hook = free_hook;
    
    // 如果使用自定义内存管理，直接在分配/释放函数中调用钩子
    printf("Malloc hooks registered\n");
}

// 查找内存泄漏
void find_memory_leaks(void) {
    printf("=== Potential Memory Leaks ===\n");
    
    malloc_record_t *record = g_malloc_records;
    while (record) {
        printf("Leaked memory: %p, size: %zu, allocated at:\n", 
               record->ptr, record->size);
        
        for (int i = 0; i < 8 && record->caller_stack[i] != 0; i++) {
            printf("  [%d] 0x%08x (%s)\n", i, 
                   record->caller_stack[i],
                   get_symbol_name(record->caller_stack[i]));
        }
        printf("\n");
        
        record = record->next;
    }
}