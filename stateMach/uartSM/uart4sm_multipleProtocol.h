#ifndef __UART4SM_H__
#define __UART4SM_H__

#include "../component/circlebuf/circlebuf_x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "timers.h"
#include "stdint.h"
#include "../component/circlebuf/circlebuf_x.h"
#include "../component/statemach/xstatemach.h"
#include "../component/xprintf/xprintf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
    int state;      //状态
    void * usrData; //用户自定义数据
}T_STATEMACHAttr;

typedef struct 
{
    int (*  init)(void *);//进入状态，前置初始化
    int (*  handlers)(void *,char );//
    int (*  exit)(void *);
}T_STATEMACHFuncUnit;


typedef struct 
{
    T_STATEMACHAttr attr;           //状态
    T_STATEMACHFuncUnit* punit;     //状态条目   
}T_STATEMACHCtrl;



// 你原来的数据结构体
typedef struct {
    int dataIndex; 
    uint16_t type;
    uint16_t len;
    uint8_t data[256];
}T_Uart4sm_data;

// 新增的协议定义结构体
typedef struct ProtocolDef {
    const uint8_t* header;
    size_t header_len;
    // 协议处理器函数，返回0表示完成，其他值表示需要继续
    int (*handler)(void*, uint8_t);
} ProtocolDef;

// 新增的状态枚举
enum {
  eState_Head1,
  eState_HeadN, // 新增状态，用于匹配多字节帧头
  eState_ProtocolHandler, // 新增状态，进入特定协议的处理函数
  eState_Max,
};

extern T_STATEMACHCtrl gStateUart4Ctrl;
#ifdef __cplusplus
}
#endif

#endif