#include "userTask.h"
#include "stdlib.h"
#include "string.h"
#include "uartPowerUpSM.h"
#include "Ymodem/Ymodem.h"
//#include "shell.h"


T_SMCtrl gCtrl;


static int s_intEvent[8] = {0};
CircularBuffer_Int geventBuf;

void xil_printf(const char *fmt, ...);
void Lv1Action_PowerUpIdle(void * p);
void Lv1Action_UpdateFirewareInit(void * p);
void Lv1Action_UpdateFireware(void * p) ;

void Lv1Action_ShellWork(void * p);
void Lv1Action_NormalWork(void * p) ;

Lv1Transition_t g_Lv1TransitionTable[] = {
    { LV1_STATE_IDLE,               EVENT_IDLE,                     LV1_STATE_IDLE,             Lv1Action_PowerUpIdle   ,   NULL},      //*do nothing
    { LV1_STATE_IDLE,               EVENT_CMD_UPDATEBIN,           LV1_STATE_UPDATEBIN,       Lv1Action_PowerUpIdle    ,   Lv1Action_UpdateFirewareInit},     //*jump BD21Update
    { LV1_STATE_IDLE,               EVENT_CMD_POWERUP_TIMEOUT,      LV1_STATE_WORK_SHELL,             Lv1Action_PowerUpIdle    ,   NULL},    //*jump normal work

    { LV1_STATE_UPDATEBIN,         EVENT_IDLE,                     LV1_STATE_UPDATEBIN,       Lv1Action_UpdateFireware    ,   NULL},
    { LV1_STATE_UPDATEBIN,         EVENT_CMD_SWITCH_SHELL,         LV1_STATE_WORK_SHELL,       Lv1Action_UpdateFireware    ,   NULL},
    
    { LV1_STATE_WORK_SHELL,         EVENT_IDLE,                     LV1_STATE_WORK_SHELL,       Lv1Action_ShellWork    ,   NULL},
    { LV1_STATE_WORK_SHELL,         EVENT_CMD_UPDATEBIN,            LV1_STATE_UPDATEBIN,       Lv1Action_ShellWork    ,   NULL},


    { LV1_STATE_WORK,               EVENT_IDLE,                     LV1_STATE_WORK,             Lv1Action_NormalWork    ,   NULL},
    { LV1_STATE_WORK,               EVENT_CMD_SWITCH_SHELL,         LV1_STATE_WORK_SHELL,       Lv1Action_NormalWork    ,   NULL},

    // { LV1_STATE_WORK_SHELL,         EVENT_IDLE,                     LV1_STATE_WORK_SHELL,       Lv1Action_NormalWork    ,   NULL},
    // { LV1_STATE_UPDATEBIN,         EVENT_CMD_DBGREP,           LV1_STATE_REPLAYING,    Lv1Action_StartReplaying },
    // { LV1_STATE_SAMPLING,   EVENT_SAMPLE_COMPLETE,  LV1_STATE_IDLE,         Lv1Action_DoNothing },
    // { LV1_STATE_REPLAYING,  EVENT_REPLAY_COMPLETE,  LV1_STATE_IDLE,         Lv1Action_DoNothing },
};

void StateMachInit(void)
{

    memset((char*)&gCtrl, 0, sizeof(gCtrl));

    //* init lv1stateMach 
    gCtrl.lv1sm.pstateTab = g_Lv1TransitionTable;
    gCtrl.lv1sm.state = LV1_STATE_IDLE;

    memset((char*)&geventBuf, 0, sizeof(geventBuf));
    initializeBufferInt(&geventBuf, s_intEvent, sizeof(s_intEvent)/sizeof(s_intEvent[0]));


}

// 2.3 状态机引擎
void Lv1ProcessEvent(T_STATEMACHCtrl_LV1* psm, int event) {
    for (int i = 0; i < sizeof(g_Lv1TransitionTable) / sizeof(Lv1Transition_t); i++) {

        if (g_Lv1TransitionTable[i].currentState == psm->state &&
            g_Lv1TransitionTable[i].event == event) {
            
            Lv1Context_t context;
            context.curState = psm->state;
            context.event    = event;
            if (g_Lv1TransitionTable[i].action != NULL) {
                g_Lv1TransitionTable[i].action((void*)&context);
            }
            //* state switch init
            if (g_Lv1TransitionTable[i].next_init ) 
                g_Lv1TransitionTable[i].next_init((void*)&context);
            psm->state = g_Lv1TransitionTable[i].nextState;
            return;
        }
    }
}

//* return : 1 suceess, 0 fail
int Event_Put(int data)
{
    return writeBufferIntNoMutex(&geventBuf, data);
}

int Event_Get(int *pdata)
{
    return readBufferIntNoMutex(&geventBuf, pdata);
}
//extern Shell shell;
void Lv1Action_PowerUpIdle(void * p) { 
    //* PowerUp Wait Message
    unsigned char ret = 1;
	unsigned char ch;
	//ret = gDbgUart.pfRecvByte(&ch);
	//注意，只实现单字节访问
	// ret = dequeue(&ch);
	ret = PSUART_RX_BYTE_GET(ch);
    if(ret)
    {
        if(PowerUpStrCheck(ch))
        {
            Event_Put(EVENT_CMD_UPDATEBIN);
        }
    }
    //* timeout send normal work event
    if( tick_diff(gCtrl.lv1sm_usr_data.u32PowerUptick) >500 )
    {
        Event_Put(EVENT_CMD_POWERUP_TIMEOUT);
        xil_printf("Send Timeout Event\n"); 
    }
}
void Lv1Action_UpdateFirewareInit(void * p) {
        outbyte(UPDATE_MAGIC_NUM1);
        outbyte(UPDATE_MAGIC_NUM2);
        outbyte(UPDATE_MAGIC_NUM3);
        outbyte(UPDATE_MAGIC_NUM4);
}

void Lv1Action_UpdateFireware(void * p) {
    int ret = 0; 
    char ch;

    ret = PSUART_RX_BYTE_GET(ch);
    YmodemProcess(ch,ret);

    //* shell unlock logic
    if(ret)
    {
        if(ShellUnlockStrCheck(ch))
        {
            Event_Put(EVENT_CMD_SWITCH_SHELL);
        }
    }
    //*Ymodem function set at this, temporary
}


void Lv1Action_ShellWork(void * p) {
    int ret = 0; 
    char ch;

    ret = PSUART_RX_BYTE_GET(ch);
    if(ret)
    {
        ShellInteract(ch);
    }
    //*Ymodem function set at this, temporary
}


void Lv1Action_NormalWork(void * p) { 
    Lv1Context_t * pContext = (Lv1Context_t *)p;
    unsigned char ret = 1;
	unsigned char ch;
    // xil_printf("Action: LV1 starts sampling...\n"); 
    //* update BD21 image
#if 0 //XXX 放开上电默认支持HEX cmd
    if( pContext->curState == LV1_STATE_WORK)
    {
        ret = PLUART_RX_BYTE_GET(PLUART_INDEX_0_PC,ch);
        if(ret)
        {
            UartPCCmdProcess(ch);   
        }
    }
    else
#endif
    {
        // shellTask((void *)&shell);
    }
    
}
