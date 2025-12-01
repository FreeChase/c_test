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


typedef struct {
    uint16_t maxlen;
    uint16_t curlen;
    uint8_t *pdata;
}T_Uart_AsciiData;


enum{
  eState_Head1 = 0,
  eState_Head2,
  eState_Head3,
  eState_Head4,
  eState_UPDATE,
  eState_Max,
};
enum{
  eState_Interact_Head1 = 0,
  eState_Interact_RecvDate ,
  eState_Interact_EndCharacter,
  eState_Interact_Max,
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

#define UPDATE_MAGIC_NUM1 (0xAA)
#define UPDATE_MAGIC_NUM2 (0x55)
#define UPDATE_MAGIC_NUM3 (0xAA)
#define UPDATE_MAGIC_NUM4 (0x55)

#define SHELL_UNLCOK_MAGIC_NUM1 (0xEB)
#define SHELL_UNLCOK_MAGIC_NUM2 (0x95)
#define SHELL_UNLCOK_MAGIC_NUM3 (0xAB)
#define SHELL_UNLCOK_MAGIC_NUM4 (0xCD)

#define SHELL_INTERACTION_MAGIC_START ('$')
#define SHELL_INTERACTION_MAGIC_END   ('\n')

#define ASCII_PARSE_ITEM_UNIT_MAX  (32)
#define ASCII_PARSE_ITEM_NUM  (10)

// extern T_STATEMACHCtrl gStatePowerUp;
#ifdef __cplusplus
}
#endif

#endif 

