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

typedef struct {
    int dataIndex; 
    uint16_t type;
    uint16_t len;
    uint8_t data[256];
}T_Uart4sm_data;

enum{
  eState_Head1,
  eState_Head2,
  eState_Type,
  eState_Len1,
  eState_Len2,
  eState_Data,
  eState_Max,
};


extern T_STATEMACHCtrl gStateUart4Ctrl;
#ifdef __cplusplus
}
#endif

#endif 

