#include "userTask.h"
#include "stdlib.h"
#include "string.h"
#include "shell.h"


T_SMCtrl gCtrl;


static int s_intEvent[8] = {0};
CircularBuffer_Int geventBuf;

void dx_kprintf(const char *fmt, ...);
void Lv1Action_PowerUpIdle(void * p);
void Lv1Action_UpdateBD21(void * p) ;
void Lv1Action_NormalWork(void * p) ;

Lv1Transition_t g_Lv1TransitionTable[] = {
    { LV1_STATE_IDLE,               EVENT_IDLE,                     LV1_STATE_IDLE,             Lv1Action_PowerUpIdle   ,   NULL},      //*do nothing
    { LV1_STATE_IDLE,               EVENT_CMD_UPDATEBD21,           LV1_STATE_UPDATEBD21,       Lv1Action_PowerUpIdle    ,   NULL},     //*jump BD21Update
    { LV1_STATE_IDLE,               EVENT_CMD_POWERUP_TIMEOUT,      LV1_STATE_WORK,             Lv1Action_PowerUpIdle    ,   NULL},    //*jump normal work

    { LV1_STATE_UPDATEBD21,         EVENT_IDLE,                     LV1_STATE_UPDATEBD21,       Lv1Action_UpdateBD21    ,   NULL},


    { LV1_STATE_WORK,               EVENT_IDLE,                     LV1_STATE_WORK,             Lv1Action_NormalWork    ,   NULL},
    { LV1_STATE_WORK,               EVENT_CMD_SWITCH_SHELL,         LV1_STATE_WORK_SHELL,       Lv1Action_NormalWork    ,   NULL},

    { LV1_STATE_WORK_SHELL,         EVENT_IDLE,                     LV1_STATE_WORK_SHELL,       Lv1Action_NormalWork    ,   NULL},
    // { LV1_STATE_UPDATEBD21,         EVENT_CMD_DBGREP,           LV1_STATE_REPLAYING,    Lv1Action_StartReplaying },
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
extern Shell shell;
void Lv1Action_PowerUpIdle(void * p) { 
    //* PowerUp Wait Message
    unsigned char ret = 1;
	unsigned char ch;
	//ret = gDbgUart.pfRecvByte(&ch);
	//注意，只实现单字节访问
	// ret = dequeue(&ch);
	ret = PLUART_RX_BYTE_GET(PLUART_INDEX_0_PC,ch);
    if(ret)
    {
        if(PowerUpStrCheck(ch))
        {
            Event_Put(EVENT_CMD_UPDATEBD21);
        }
    }
    //* timeout send normal work event
    if( tick_diff(gCtrl.lv1sm_usr_data.u32PowerUptick) >500 )
    {
        Event_Put(EVENT_CMD_POWERUP_TIMEOUT);
        dx_kprintf("Send Timeout Event\n"); 
    }
}
void uartDelayRemap(u8 val,u32 delay_ms);
void Lv1Action_UpdateBD21(void * p) { 
    // dx_kprintf("Action: LV1 starts sampling...\n"); 
    //* update BD21 image
    static char tmpflagBD21 = 0;
    if(tmpflagBD21 == 0)
    {
        tmpflagBD21 = 1;
        dx_kprintf("Lv1Action_UpdateBD21 !\n");
        tick_delay_ms(100);
        uartDelayRemap(2,100);

    }
}


void Lv1Action_NormalWork(void * p) { 
    Lv1Context_t * pContext = (Lv1Context_t *)p;
    unsigned char ret = 1;
	unsigned char ch;
    // dx_kprintf("Action: LV1 starts sampling...\n"); 
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
        shellTask((void *)&shell);
    }
    
}
