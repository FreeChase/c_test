#include "uartPowerUpSM.h"
#include "xstatemach.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "userTask.h"
#include "xil_printf.h"
#include "strSplit/strparse.h"
#include "strSplit/paramparse.h"





//***************************************************
//* shell interaction function
//* Start
//**************************************************/
static int StateHandle_ShellInteract_Init(void *p);
static int StateSwitch_ShellInteract_Exit(void *p);
static int StateHandle_ShellInteract(void *p,unsigned char ch);

static T_Uart_AsciiData sprotocolData_ShellInteract; 


static T_STATEMACHFuncUnit state_shellInteract_unit[eState_Max] = {
    {StateHandle_ShellInteract_Init, StateHandle_ShellInteract, NULL},
    {StateHandle_ShellInteract_Init, StateHandle_ShellInteract, NULL},
    {StateHandle_ShellInteract_Init, StateHandle_ShellInteract, NULL},
};


void xil_printf(const char *fmt, ...);
int AsciiDataDecode(T_Uart_AsciiData *pDataCtrl);




static T_STATEMACHCtrl gStateShellInteract = {{eState_Head1, &sprotocolData_ShellInteract}, state_shellInteract_unit};
static int StateHandle_ShellInteract(void *p,unsigned char ch) {
    T_STATEMACHCtrl *pStateCtrl = (T_STATEMACHCtrl *)p;
    T_Uart_AsciiData *pDataCtrl = (T_Uart_AsciiData *)pStateCtrl->attr.usrData;
#if 1
    switch (pStateCtrl->attr.state)
    {
        case eState_Interact_Head1:
            // xil_printf("ch 0x%x state %d\r\n",ch,pStateCtrl->attr.state);
            if (ch == SHELL_INTERACTION_MAGIC_START) {
                pStateCtrl->attr.state = eState_Interact_RecvDate;
                pDataCtrl->curlen = 1;
                pDataCtrl->pdata[0]=ch;
            }
            break;

        case eState_Interact_RecvDate:
            if (ch == SHELL_INTERACTION_MAGIC_END) {
                pDataCtrl->pdata[ pDataCtrl->curlen ] = 0;
                AsciiDataDecode(pDataCtrl);
                pStateCtrl->attr.state = eState_Interact_Head1;
            }
            else
            {
                if(pDataCtrl->curlen < pDataCtrl->maxlen)
                {
                    pDataCtrl->pdata[ pDataCtrl->curlen ]=ch;
                    pDataCtrl->curlen += 1;
                }
                else
                {
                    //* buff is full, but havn't \n force to analyse
                    pDataCtrl->pdata[ pDataCtrl->curlen ] = 0;
                    AsciiDataDecode(pDataCtrl);
                    pStateCtrl->attr.state = eState_Interact_Head1;
                }
            }
            break;
        case eState_Interact_EndCharacter:

            pStateCtrl->attr.state = eState_Interact_Head1;
            break;

        default:
            break;
    }
#endif
    return 0;
}



int StateHandle_ShellInteract_Init(void *p) {
    // 初始化状态机
    return 0;
}

int StateSwitch_ShellInteract_Exit(void *p) {
    // 退出状态机
    return 0;
}



extern uint8_t u8buf_strsplit[128];
void ShellAsciiInit(void)
{
    sprotocolData_ShellInteract.pdata = u8buf_strsplit;
    sprotocolData_ShellInteract.maxlen = sizeof(u8buf_strsplit) - 1;
    sprotocolData_ShellInteract.curlen = sizeof(u8buf_strsplit) - 1;
}

int ShellInteract(unsigned char ch)   
{
    if (gStateShellInteract.punit->init)
        gStateShellInteract.punit->init((void *)&gStateShellInteract);
    if (gStateShellInteract.punit->handlers)
        gStateShellInteract.punit->handlers((void *)&gStateShellInteract, ch);
    return 0;
}
char bbuf[512];
int parsetest() {
#if 0
   // 示例数据
   char data[] = "$TSTMODE,print,1,2,3.14,,Hello,world,*FC";
   char data2[] = "BDCNO,,0,";
   char data3[] = "BDCOV,0,25,0,";
   char data4[] = "$flashread,88*FC\r\n";
   char * string = "-";

   if( *string == '-')
   {
    xil_printf("Hello\r\n");
   }
   T_ParamsInfo paramInfo;

   memset((char*)&paramInfo,0,sizeof(paramInfo));
   paramInfo.itemMax = 12;
   paramInfo.itemUnitMax = 32;
   paramInfo.pdst = bbuf;

//    parseDataProtocol(data,&paramInfo);

   parseDataProtocol(data4,&paramInfo);
    xil_printf("flashread %d\r\n",paramsParsePara( &paramInfo.pdst[1*paramInfo.itemUnitMax] ));
#endif
   return 0;
}
//***************************************************
//* shell interaction function
//* END
//**************************************************/


static int StateSwitchInit(void *p);
static int StateSwitchExit(void *p);
static int StateHandle(void *p,unsigned char ch);

static T_Uart4sm_data sprotocolData; 


static T_STATEMACHFuncUnit state1_unit[eState_Max] = {
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
};

static T_STATEMACHCtrl gStatePowerUp = {{eState_Head1, &sprotocolData}, state1_unit};
void xil_printf(const char *fmt, ...);
static int StateHandle(void *p,unsigned char ch) {
    T_STATEMACHCtrl *pStateCtrl = (T_STATEMACHCtrl *)p;
    //T_Uart4sm_data *pData = (T_Uart4sm_data *)pStateCtrl->attr.usrData;
    static char tmpflag = 0;
#if 1
    switch (pStateCtrl->attr.state)
    {
        case eState_Head1:
            // xil_printf("ch 0x%x state %d\r\n",ch,pStateCtrl->attr.state);
            if (ch == UPDATE_MAGIC_NUM1) {
                pStateCtrl->attr.state = eState_Head2;
            }
            break;

        case eState_Head2:
            if (ch == UPDATE_MAGIC_NUM2) {
                pStateCtrl->attr.state = eState_Head3;
            } else if (ch == UPDATE_MAGIC_NUM1) {
                pStateCtrl->attr.state = eState_Head2;
            }else{
                pStateCtrl->attr.state = eState_Head1;
            }
            break;
        case eState_Head3:
            if (ch == UPDATE_MAGIC_NUM3) {
                pStateCtrl->attr.state = eState_Head4;
            } else if (ch == UPDATE_MAGIC_NUM1) {
                pStateCtrl->attr.state = eState_Head2;
            } else {
                pStateCtrl->attr.state = eState_Head1;
            }
            break;
        case eState_Head4:
            if (ch == UPDATE_MAGIC_NUM4) {
                pStateCtrl->attr.state = eState_UPDATE;
            } else {
                pStateCtrl->attr.state = eState_Head1;
            }
            break;
        case eState_UPDATE:
            if(tmpflag == 0)
            {
                tmpflag = 1;
                xil_printf("Alreay Receive Update Cmd !\r\n",ch);
            }
            break;
        default:
            break;
    }
#endif
    return 0;
}

int StateSwitchInit(void *p) {
    // 初始化状态机
    return 0;
}

int StateSwitchExit(void *p) {
    // 退出状态机
    return 0;
}


int PowerUpStrCheck(unsigned char ch)   
{
    if (gStatePowerUp.punit->init)
        gStatePowerUp.punit->init((void *)&gStatePowerUp);
    if (gStatePowerUp.punit->handlers)
        gStatePowerUp.punit->handlers((void *)&gStatePowerUp, ch);

    if(gStatePowerUp.attr.state == eState_UPDATE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//****************************************
//* shell unlock, switch to shell function

static int StateHandle_ShellUnlock_Init(void *p);
static int StateSwitch_ShellUnlock_Exit(void *p);
static int StateHandle_ShellUnlock(void *p,unsigned char ch);

static T_Uart4sm_data sprotocolData_Shell; 


static T_STATEMACHFuncUnit state_shell_unit[eState_Max] = {
    {StateHandle_ShellUnlock_Init, StateHandle_ShellUnlock, NULL},
    {StateHandle_ShellUnlock_Init, StateHandle_ShellUnlock, NULL},
    {StateHandle_ShellUnlock_Init, StateHandle_ShellUnlock, NULL},
    {StateHandle_ShellUnlock_Init, StateHandle_ShellUnlock, NULL},
    {StateHandle_ShellUnlock_Init, StateHandle_ShellUnlock, NULL},
};

static T_STATEMACHCtrl gStateShellUnlock = {{eState_Head1, &sprotocolData_Shell}, state_shell_unit};
void xil_printf(const char *fmt, ...);
static int StateHandle_ShellUnlock(void *p,unsigned char ch) {
    T_STATEMACHCtrl *pStateCtrl = (T_STATEMACHCtrl *)p;
    //T_Uart4sm_data *pData = (T_Uart4sm_data *)pStateCtrl->attr.usrData;
    static char tmpflag = 0;
#if 1
    switch (pStateCtrl->attr.state)
    {
        case eState_Head1:
            // xil_printf("ch 0x%x state %d\r\n",ch,pStateCtrl->attr.state);
            if (ch == SHELL_UNLCOK_MAGIC_NUM1) {
                pStateCtrl->attr.state = eState_Head2;
            }
            break;

        case eState_Head2:
            if (ch == SHELL_UNLCOK_MAGIC_NUM2) {
                pStateCtrl->attr.state = eState_Head3;
            } else if (ch == SHELL_UNLCOK_MAGIC_NUM1) {
                pStateCtrl->attr.state = eState_Head2;
            }else{
                pStateCtrl->attr.state = eState_Head1;
            }
            break;
        case eState_Head3:
            if (ch == SHELL_UNLCOK_MAGIC_NUM3) {
                pStateCtrl->attr.state = eState_Head4;
            } else if (ch == SHELL_UNLCOK_MAGIC_NUM1) {
                pStateCtrl->attr.state = eState_Head2;
            } else {
                pStateCtrl->attr.state = eState_Head1;
            }
            break;
        case eState_Head4:
            if (ch == SHELL_UNLCOK_MAGIC_NUM4) {
                pStateCtrl->attr.state = eState_UPDATE;
            } else {
                pStateCtrl->attr.state = eState_Head1;
            }
            break;
        case eState_UPDATE:
            if(tmpflag == 0)
            {
                tmpflag = 1;
                xil_printf("Alreay Receive Unlock Cmd !\r\n",ch);
            }
            break;
        default:
            break;
    }
#endif
    return 0;
}

int StateHandle_ShellUnlock_Init(void *p) {
    // 初始化状态机
    return 0;
}

int StateSwitch_ShellUnlock_Exit(void *p) {
    // 退出状态机
    return 0;
}


int ShellUnlockStrCheck(unsigned char ch)   
{
    if (gStateShellUnlock.punit->init)
        gStateShellUnlock.punit->init((void *)&gStateShellUnlock);
    if (gStateShellUnlock.punit->handlers)
        gStateShellUnlock.punit->handlers((void *)&gStateShellUnlock, ch);

    if(gStateShellUnlock.attr.state == eState_UPDATE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void ShellUnlockInit(void)   
{
    gStateShellUnlock.attr.state = eState_Head1;
}


//***************************************************
//* shell unlock, switch to shell function 
//* END
//**************************************************/
