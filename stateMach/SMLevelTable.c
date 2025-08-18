#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ===========================================
// 1. 模拟你的项目中的结构体和函数
//    请用你项目中真实的定义来替换这些部分
// ===========================================

// 模拟状态机控制结构体
typedef struct {
    int state;
} T_STATEMACHCtrl_LV1SampleReplay;

typedef struct {
    int state;
} T_STATEMACHCtrl_Lv2SampleData;

typedef struct {
    int state;
} T_STATEMACHCtrl_LV2ReplayData;

// 模拟全局控制和嵌套结构
typedef struct {
    T_STATEMACHCtrl_LV1SampleReplay lv1sm;
    struct {
        T_STATEMACHCtrl_Lv2SampleData lv2smSampleTrans;
        T_STATEMACHCtrl_LV2ReplayData lv2smReplay;
    } lv1sm_usr_data;
} T_SampleCtrl;

T_SampleCtrl gSampleCtrl;

// 模拟状态
typedef enum {
    LV1_STATE_IDLE,
    LV1_STATE_SAMPLING,
    LV1_STATE_REPLAYING,
} Lv1State_t;

typedef enum {
    LV2_STATE_IDLE,
    LV2_STATE_WORKING,
} Lv2State_t;

// 模拟事件
typedef enum {
    EVENT_NONE,
    EVENT_CMD_DBGSAM,
    EVENT_CMD_DBGREP,
    EVENT_SAMPLE_START,
    EVENT_SAMPLE_COMPLETE,
    EVENT_REPLAY_START,
    EVENT_REPLAY_COMPLETE,
    EVENT_TICK,
} EventType_t;

// 模拟上下文结构体，用于父子状态机通信
typedef struct {
    Lv1State_t parentState;
} Lv2Context_t;

// 模拟动作函数
void Lv1Action_StartSampling() { 
    printf("Action: LV1 starts sampling...\n"); 
}
void Lv1Action_StartReplaying() { 
    printf("Action: LV1 starts replaying...\n"); 
}
void Lv1Action_DoNothing() { /* do nothing */ }
void Lv2Action_StartWorking(Lv2Context_t* context) { 
    printf("Action: LV2 starts working, parent state is %d\n", context->parentState);
    // 在这里可以执行真正的初始化任务
}
void Lv2Action_DoNothing(Lv2Context_t* context) { /* do nothing */ }

// 模拟外部接口
int check_uart_for_command() { 
    static int num = 0;
    num =(num+1)% 20;
    return !num;
} // 2% 概率有命令
EventType_t convert_cmd_to_event(const char* cmd) {
    if (strcmp(cmd, "$DBGSAM") == 0) return EVENT_CMD_DBGSAM;
    if (strcmp(cmd, "$DBGREP") == 0) return EVENT_CMD_DBGREP;
    return EVENT_NONE;
}

// 模拟数据采集完成检查
int DataSample_IsComplete() { return 1; } // 5% 概率完成

// 模拟命令字符串
const char* cmd_list[] = {"$DBGSAM", "$DBGREP"};


// ===========================================
// 2. 状态机跃迁表和通用引擎
// ===========================================

// 2.1 顶层状态机（LV1）跃迁表
typedef struct {
    Lv1State_t  currentState;
    EventType_t event;
    Lv1State_t  nextState;
    void (*action)(void);
} Lv1Transition_t;

Lv1Transition_t g_Lv1TransitionTable[] = {
    { LV1_STATE_IDLE,       EVENT_CMD_DBGSAM,       LV1_STATE_SAMPLING,     Lv1Action_StartSampling },
    { LV1_STATE_IDLE,       EVENT_CMD_DBGREP,       LV1_STATE_REPLAYING,    Lv1Action_StartReplaying },
    { LV1_STATE_SAMPLING,   EVENT_SAMPLE_COMPLETE,  LV1_STATE_IDLE,         Lv1Action_DoNothing },
    { LV1_STATE_REPLAYING,  EVENT_REPLAY_COMPLETE,  LV1_STATE_IDLE,         Lv1Action_DoNothing },
};

// 2.2 子状态机（LV2）跃迁表
typedef struct {
    Lv2State_t  currentState;
    EventType_t event;
    Lv2State_t  nextState;
    void (*action)(Lv2Context_t*); // 动作函数接受上下文
} Lv2Transition_t;

Lv2Transition_t g_Lv2SampleTransitionTable[] = {
    { LV2_STATE_IDLE,       EVENT_SAMPLE_START,     LV2_STATE_WORKING,      Lv2Action_StartWorking },
    { LV2_STATE_WORKING,    EVENT_SAMPLE_COMPLETE,  LV2_STATE_IDLE,         Lv2Action_DoNothing },
};

Lv2Transition_t g_Lv2ReplayTransitionTable[] = {
    { LV2_STATE_IDLE,       EVENT_REPLAY_START,     LV2_STATE_WORKING,      Lv2Action_StartWorking },
    { LV2_STATE_WORKING,    EVENT_REPLAY_COMPLETE,  LV2_STATE_IDLE,         Lv2Action_DoNothing },
};


// 2.3 状态机引擎
void Lv1ProcessEvent(T_STATEMACHCtrl_LV1SampleReplay* psm, EventType_t event) {
    for (int i = 0; i < sizeof(g_Lv1TransitionTable) / sizeof(Lv1Transition_t); i++) {
        printf("current state %d ev:%d\n", g_Lv1TransitionTable[i].currentState,g_Lv1TransitionTable[i].event);
        printf("target state %d ev:%d\n", psm->state,event);
        if (g_Lv1TransitionTable[i].currentState == psm->state &&
            g_Lv1TransitionTable[i].event == event) {
            
            printf("LV1: State %d -> Event %d -> State %d\n", psm->state, event, g_Lv1TransitionTable[i].nextState);
            if (g_Lv1TransitionTable[i].action != NULL) {
                g_Lv1TransitionTable[i].action();
            }
            psm->state = g_Lv1TransitionTable[i].nextState;
            return;
        }
    }
}

void Lv2ProcessEvent(
    T_STATEMACHCtrl_Lv2SampleData* psm, 
    EventType_t event, 
    Lv2Context_t* context,
    Lv2Transition_t* p_transition_table, 
    size_t table_size) {

    for (int i = 0; i < table_size / sizeof(Lv2Transition_t); i++) {
        if (p_transition_table[i].currentState == psm->state &&
            p_transition_table[i].event == event) {
            
            printf("LV2: State %d -> Event %d -> State %d\n", psm->state, event, p_transition_table[i].nextState);
            if (p_transition_table[i].action != NULL) {
                p_transition_table[i].action(context); // 传递上下文
            }
            psm->state = p_transition_table[i].nextState;
            return;
        }
    }
}


// ===========================================
// 3. 完整的 main 循环
// ===========================================

int main(void) {
    // 初始化状态机
    gSampleCtrl.lv1sm.state = LV1_STATE_IDLE;
    gSampleCtrl.lv1sm_usr_data.lv2smSampleTrans.state = LV2_STATE_IDLE;
    gSampleCtrl.lv1sm_usr_data.lv2smReplay.state = LV2_STATE_IDLE;
    
    printf("System initialized. Entering main loop...\n");

    while(1) {
        // 1. 检查外部输入并生成事件
        if (check_uart_for_command()) {
            int ret = rand() % 2;
            const char* cmd = cmd_list[ret];
            printf("ret : %d\n",ret);
            EventType_t event = convert_cmd_to_event(cmd);
            printf("event : %d\n",event);
            if (event != EVENT_NONE) {
                Lv1ProcessEvent(&gSampleCtrl.lv1sm, event);
            }
        }
        
        // 2. 检查周期性任务，如果完成则生成事件
        if (gSampleCtrl.lv1sm_usr_data.lv2smSampleTrans.state == LV2_STATE_WORKING) {
            if (DataSample_IsComplete()) {
                Lv1ProcessEvent(&gSampleCtrl.lv1sm, EVENT_SAMPLE_COMPLETE);
            }
        }
        
        // 3. 创建上下文，并传递给子状态机
        Lv2Context_t lv2_context = {.parentState = gSampleCtrl.lv1sm.state};

        // 4. 根据父状态，动态决定要驱动哪个子状态机
        //    这里也可以根据你的需要，通过 switch/case 驱动
        //    这个简单的例子中，我们直接全部驱动，由子状态机自行判断是否需要处理
        
        Lv2ProcessEvent(
            &gSampleCtrl.lv1sm_usr_data.lv2smSampleTrans, 
            EVENT_TICK, 
            &lv2_context, 
            g_Lv2SampleTransitionTable, 
            sizeof(g_Lv2SampleTransitionTable)
        );
        Lv2ProcessEvent(
            &gSampleCtrl.lv1sm_usr_data.lv2smReplay, 
            EVENT_TICK, 
            &lv2_context, 
            g_Lv2ReplayTransitionTable, 
            sizeof(g_Lv2ReplayTransitionTable)
        );
        
        // 模拟一下系统运行
        usleep(100000); // 100ms
    }
    return 0;
}