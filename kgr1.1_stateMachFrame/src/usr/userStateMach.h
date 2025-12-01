#ifndef __USERSTATEMACH_H__
#define __USERSTATEMACH_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// 模拟状态
typedef enum {
    LV1_STATE_IDLE = 0,         //* 0 default state
    LV1_STATE_UPDATEBD21,       //* update bd21, when check uart data in power-up 500ms  
    LV1_STATE_WORK,
    LV1_STATE_WORK_SHELL,
    LV1_STATE_MAX,
} Lv1State_t;

typedef enum {
    LV2_STATE_IDLE,
    LV2_STATE_WORKING,
} Lv2State_t;



// 模拟上下文结构体，用于父子状态机通信
typedef struct {
    int curState;
    int event;
} Lv1Context_t;

typedef struct {
    Lv1State_t parentState;
} Lv2Context_t;

// ===========================================
// 0. 状态机跃迁表和通用引擎
// ===========================================

// 2.1 顶层状态机（LV1）跃迁表
typedef struct {
    Lv1State_t  currentState;
    int event;
    Lv1State_t  nextState;
    void (*action)(void *);
    void (*next_init)(void *);
} Lv1Transition_t;

/*
Lv1Transition_t g_Lv1TransitionTable[] = {
    { LV1_STATE_IDLE,       EVENT_CMD_UPDATEBD21,       LV1_STATE_SAMPLING,     Lv1Action_StartSampling },
    { LV1_STATE_IDLE,       EVENT_CMD_DBGREP,       LV1_STATE_REPLAYING,    Lv1Action_StartReplaying },
    { LV1_STATE_SAMPLING,   EVENT_SAMPLE_COMPLETE,  LV1_STATE_IDLE,         Lv1Action_DoNothing },
    { LV1_STATE_REPLAYING,  EVENT_REPLAY_COMPLETE,  LV1_STATE_IDLE,         Lv1Action_DoNothing },
};
*/

// 2.2 子状态机（LV2）跃迁表
typedef struct {
    Lv2State_t  currentState;
    int event;
    Lv2State_t  nextState;
    void (*action)(Lv2Context_t*); // 动作函数接受上下文
} Lv2Transition_t;

/*
Lv2Transition_t g_Lv2SampleTransitionTable[] = {
    { LV2_STATE_IDLE,       EVENT_SAMPLE_START,     LV2_STATE_WORKING,      Lv2Action_StartWorking },
    { LV2_STATE_WORKING,    EVENT_SAMPLE_COMPLETE,  LV2_STATE_IDLE,         Lv2Action_DoNothing },
};

Lv2Transition_t g_Lv2ReplayTransitionTable[] = {
    { LV2_STATE_IDLE,       EVENT_REPLAY_START,     LV2_STATE_WORKING,      Lv2Action_StartWorking },
    { LV2_STATE_WORKING,    EVENT_REPLAY_COMPLETE,  LV2_STATE_IDLE,         Lv2Action_DoNothing },
};

*/

// ===========================================
// 1. 模拟你的项目中的结构体和函数
//    请用你项目中真实的定义来替换这些部分
// ===========================================

// 模拟状态机控制结构体
typedef struct {
    int state;
    Lv1Transition_t * pstateTab;
} T_STATEMACHCtrl_LV1;

typedef struct {
    int state;
    Lv2Transition_t * pstateTab;
} T_STATEMACHCtrl_Lv2_a;

typedef struct {
    int state;
    Lv2Transition_t * pstateTab;
} T_STATEMACHCtrl_Lv2_b;

// 模拟全局控制和嵌套结构
typedef struct {
    T_STATEMACHCtrl_LV1 lv1sm;
    struct {
        uint32_t u32PowerUptick;
    } lv1sm_usr_data;
} T_SMCtrl;







void Lv1ProcessEvent(T_STATEMACHCtrl_LV1* psm, int event);
int Event_Put(int data);
int Event_Get(int* pdata);

#ifdef __cplusplus
}
#endif

#endif 

