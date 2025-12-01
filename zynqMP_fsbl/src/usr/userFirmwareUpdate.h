#ifndef __USERFIRMWAREUPDATE_H__
#define __USERFIRMWAREUPDATE_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "circlebuf_x/circlebuf_x.h"


#define IMAGE_BASE_ADDR_FSBL    0x0
#define IMAGE_SIZE_FSBL         0x100000

#define IMAGE_BASE_ADDR_BIT     ((IMAGE_BASE_ADDR_FSBL+IMAGE_SIZE_FSBL))
#define IMAGE_SIZE_BIT         (0x100000 * 36)

#define IMAGE_BASE_ADDR_APP    ((IMAGE_BASE_ADDR_BIT)+IMAGE_SIZE_BIT)
#define IMAGE_SIZE_APP         0x100000

enum{
    eFirewareUpdate_Init = 0,
    eFirewareUpdate_Update,
    eFirewareUpdate_Over,
};

enum{
    eFirewareUpdate_Result_Updating = 0,
    eFirewareUpdate_Result_Success,
    eFirewareUpdate_Result_Fail,
};

enum{
    eFirewareImage_Fsbl = 0,
    eFirewareImage_PLBit,
    eFirewareImage_AppA53,
    eFirewareImage_Max,
};

typedef struct 
{
    //* state
    uint32_t state;
    uint32_t result;

    //* basic parameter
    uint8_t fileName[128];      //flash起始地址
    uint32_t u32FileSize;      //存储文件大小
    uint32_t u32AddrStar;      //flash起始地址
    uint32_t u32alreayWrite;   //基于 u32AddrStar 的偏移， 已经写入大小
    uint32_t u32ErSize;        //单词擦除大小
    uint32_t u32PageSize;      //页大小


}T_UpdateCtrl_Attr;

typedef struct 
{
//   CircularBuffer rx;  //循环buf
  T_UpdateCtrl_Attr attr;
}T_UpdateCtrl;


typedef struct 
{
    char name[16];
    uint32_t u32addr;
}T_AddrSwitchTable;     //* from name switch to addr value 


extern T_AddrSwitchTable addrTable[eFirewareImage_Max];

int FirewareUpdate_Init(uint8_t * pdat ,uint32_t u32dataSize);
int FirewareUpdate_Update(uint8_t * pdat ,uint32_t u32dataSize);
int FirewareUpdate_Over(void);

#ifdef __cplusplus
}
#endif

#endif 

