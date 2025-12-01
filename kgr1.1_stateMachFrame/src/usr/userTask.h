#ifndef __USERTASK_H__
#define __USERTASK_H__

#include "../platform/components/circlebuf_x/circlebuf_x.h"
#include "../platform/components/circlebuf_x/circlebuf_int.h"
#include "../usr/userStateMach.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "timers.h"
//#include "queue.h"
//#include "dxdef.h"
//#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#define ACKCHECK_IOIN       (0)
#define ACKCHECK_IOOUT      (1)

#define CHECK_RESULT_SUCCESS  (0)
#define CHECK_RESULT_FAIL     (1)

#define QUEUE_STATE_LENGTH 1
#define QUEUE_STATE_ITEM_SIZE sizeof( int )


#define PLUART_INDEX_0_PC  0
#define PLUART_INDEX_1_UBLOX  1
#define PLUART_INDEX_2_BD21_1  2
#define PLUART_INDEX_3_BD21_2  3
#define PLUART_INDEX_4_FPGA  4
#define PLUART_MAX_NUM    7 


#define ZS_UART_FRAME_MAGIC  0xa55a5aa5

#define PLUART_RX_BYTE_GET(x,y)  (gUartPL[x].rx.getByte(&gUartPL[x].rx.cirbuf,&y) )

// 模拟事件
typedef enum {
    EVENT_NONE,
    EVENT_CMD_UPDATEBD21,
    EVENT_CMD_POWERUP_TIMEOUT,   //* Power Up and Timeout ,Enter Normal Work
    EVENT_CMD_SWITCH_SHELL,   //* Power Up and Timeout ,Enter Normal Work
    EVENT_IDLE,
    EVENT_TICK,
    EVENT_MAX,
} EventType_t;

enum{
  eUartDataType_GetFreq = 1,
  eUartDataType_SetFreq,
  eUartDataType_RF3701Read,
  eUartDataType_RF3701Write,
  eUartDataType_Max,
};


enum{
  eTimerID_Mcu = 1,
  eTimerID_Max,
};

typedef void (* pfuartcb)(void *);
typedef int (* pfcharPut)(void* ,char);
typedef int (* pfcharxGet)(void*,char *);
typedef int (* pfMultiCharPut)(void*,char *,int );
typedef int (* pfMultiCharGet)(void*,char *,int );

extern pfuartcb guartDatCb[eUartDataType_Max];

typedef struct 
{
  CircularBuffer cirbuf;  //循环buf
  pfcharPut putByte;         //param1 &cirbuf param2 byte
  pfcharPut putByteIsr;      //param1 &cirbuf param2 byte
  pfcharPut putByteNoMutex;  //param1 &cirbuf param2 byte

  pfcharxGet getByte;        //param1 &cirbuf param2 &byte
  pfcharxGet getByteIsr;     //param1 &cirbuf param2 &byte
  pfcharxGet getByteNoMutex; //param1 &cirbuf param2 &byte

  pfMultiCharPut putMultiByte;  //
  pfMultiCharGet getMultiByte;  //

}T_UartBufCtrl;

typedef struct 
{
  //接受循环buf
  T_UartBufCtrl rx;
  T_UartBufCtrl tx;
  //行为函数，接受数据，发送数据
  pfcharPut pfSendByte;
  pfcharxGet pfRecvByte;

}T_UartCtrl;


typedef struct{
  char en;
  char timeCur;
  char timeoutFlag;
  char ack;
  int result;
}T_UartEvent;


typedef struct{
  char clksel;      //当前时钟选择
  char clkdetect;   //时钟检测有无，bit0 内部时钟  bit1 外部时钟
}T_SystemState;

extern T_UartCtrl gUartShell;

extern T_UartCtrl gUartPL[7];



typedef struct{
  uint32_t raw[10];
  uint32_t average;
  uint32_t cnt;
}T_ADCValue;

typedef struct{
  T_ADCValue adcData;
  void (*pfcompute)(void);
  double (*pfGetAdcValue)(void);
}T_ADCCtrl;

typedef struct{
  int state;           //状态
  char setValue;        //设置值
  char readValue[16];   //读取值，最多支持16Byte
  uint32_t (*pcfgTable_IOout)[2]; //支持16个，第一项Port，第二项Pin
  uint32_t (*pcfgTable_IOin)[2]; //支持16个，第一项Port，第二项Pin
  //IO测试项目
  int (* pIOinTestItem)[16];  //io测试项配置值
  int IOinTestItemNum;        //io测试项数目
  int IOinTestItemCur;        //io测试项当前测试
  int (* pIOoutTestItem)[16];
  int IOoutTestItemNum;
  int IOoutTestItemCur;

  //测试过程记录变量
  char timeout;
  char rcvAck;
  char rcvCmd[16];        //本次接收到命令
  char rcvParams[16][16];  //本次接收到参数
  int  rcvParamNum;       //本次接收到参数个数
  //结果
  int  result;          //测试结果，整形值
  int  u32txtOffset;       //信息偏移
  char ctxt[256];       //测试结果，字符串形式
  int  ctxt_num;        //测试失败信息条数
}T_BD21Ctrl_Attr;

typedef struct{
  int (* start)(int );  //测试开始
  int (* wait)(int );   //测试持续过程
  int (* ack)(int );    //测试结果反馈
}T_BD21Ctrl_Func;

typedef struct{
  T_BD21Ctrl_Attr attr; //属性值
  T_BD21Ctrl_Func pf; //执行函数
}T_BD21Ctrl;

extern T_ADCCtrl gAdcCtrl;
extern T_UartCtrl gUartPL[7];


extern void PreInit(void);

uint32_t tick_diff(uint32_t old);
uint32_t tick_get(void);
uint32_t tick_delay_ms(uint32_t ms);

int PowerUpStrCheck(unsigned char ch);
int UartPCCmdProcess(unsigned char ch);   

#ifdef __cplusplus
}
#endif

#endif 

