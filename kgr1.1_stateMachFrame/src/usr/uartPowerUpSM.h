#ifndef __UARTPOWERUPSM_H__
#define __UARTPOWERUPSM_H__

#include "stdint.h"
#include "xstatemach.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int dataIndex; 
    uint16_t type;
    uint16_t len;
    uint8_t data[16];
}T_Uart4sm_data;

enum{
  eState_Head1 = 0,
  eState_Head2,
  eState_Head3,
  eState_Head4,
  eState_UPDATE,
  eState_Max,
};

enum{
  eStatePcWork_Head1 = 0,
  eStatePcWork_Head2,
  eStatePcWork_Type,
  eStatePcWork_Len1,
  eStatePcWork_Len2,
  eStatePcWork_Data,
  eStatePcWork_Max,
};


// extern T_STATEMACHCtrl gStatePowerUp;
#ifdef __cplusplus
}
#endif

#endif 

