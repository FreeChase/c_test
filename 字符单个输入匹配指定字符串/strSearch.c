#include <stdio.h>
#include <string.h>

#define MAX_PATTERN_LENGTH 64

// 状态机结构
typedef struct {
    const char* pattern;    // 模式串
    int pattern_len;       // 模式串长度
    int next[MAX_PATTERN_LENGTH];  // next数组，固定大小
    int current_state;    // 当前状态
} T_StrSearchSM;

// 构建next数组
static void build_next(T_StrSearchSM* sm) {
    sm->next[0] = -1;
    
    int i = 0;
    int j = -1;
    
    while (i < sm->pattern_len - 1) {
        if (j == -1 || sm->pattern[i] == sm->pattern[j]) {
            i++;
            j++;
            sm->next[i] = j;
        } else {
            j = sm->next[j];
        }
    }
}

// 初始化状态机
void init_state_machine(T_StrSearchSM* sm, const char* pattern) {
    sm->pattern = pattern;
    sm->pattern_len = strlen(pattern);
    if (sm->pattern_len >= MAX_PATTERN_LENGTH) {
        printf("错误：模式串长度不能超过%d\n", MAX_PATTERN_LENGTH);
        return;
    }
    sm->current_state = 0;
    build_next(sm);
}

// 处理单个字符，返回：
// -1: 不匹配
//  0: 部分匹配
//  1: 完全匹配
int process_char(T_StrSearchSM* sm, char c) {
    // 当字符不匹配时，回退状态
    while (sm->current_state != -1 && c != sm->pattern[sm->current_state]) {
        sm->current_state = sm->next[sm->current_state];
    }
    
    // 状态前进
    sm->current_state++;
    
    // 判断匹配情况
    if (sm->current_state == sm->pattern_len) {
        // 完全匹配，重置状态以便继续搜索
        sm->current_state = sm->next[sm->current_state - 1];
        return 1;  // 完全匹配
    }
    
    return 0;  // 部分匹配
}

// 重置状态机
void reset_state_machine(T_StrSearchSM* sm) {
    sm->current_state = 0;
}

#if 1
// 测试代码
void test_stream_search() {
    const char* pattern = "abcabc";
    printf("模式串: %s\n", pattern);
    
    // 初始化状态机
    T_StrSearchSM sm;
    init_state_machine(&sm, pattern);
    
    // 测试字符流
    const char* test_stream = "ccabcabcabcabcabcccc";
    printf("输入流: %s\n", test_stream);
    
    printf("\n逐字符处理:\n");
    for (int i = 0; test_stream[i] != '\0'; i++) {
        char c = test_stream[i];
        int result = process_char(&sm, c);
        
        printf("输入字符 '%c': ", c);
        if (result == 1) {
            printf("完全匹配！\n");
        } else {
            printf("部分匹配中...\n");
        }
    }
}

int main() {
    test_stream_search();
    return 0;
}
#endif