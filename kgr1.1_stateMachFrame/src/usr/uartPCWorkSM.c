#include "uartPowerUpSM.h"
#include <stdio.h>
#include <stdint.h>
#include "userTask.h"

static int StateSwitchInit(void *p);
static int StateSwitchExit(void *p);
static int StateHandle(void *p,unsigned char ch);

static T_Uart4sm_data sprotocolData_PCWork; 

void CBPCWork_GetVer(void * p);
void CBPCWork_Switch2Shell(void * p);

pfuartcb guartDatCb[eUartDataType_Max] ={
    0,
    CBPCWork_GetVer,
    CBPCWork_Switch2Shell,
    // CBUart4_GetFreq,
    // CBUart4_SetFreq,
    // CBUart4_GetRF3701,
    // CBUart4_SetRF3701,
};

static T_STATEMACHFuncUnit state1_unit_pcwork[eStatePcWork_Max] = {
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
};

T_STATEMACHCtrl gStateUartPCWork = {{eStatePcWork_Head1, &sprotocolData_PCWork}, state1_unit_pcwork};

void dx_kprintf(const char *fmt, ...);
static int StateHandle(void *p,unsigned char ch) {
    T_STATEMACHCtrl *pStateCtrl = (T_STATEMACHCtrl *)p;
    T_Uart4sm_data *pData = (T_Uart4sm_data *)pStateCtrl->attr.usrData;

    switch (pStateCtrl->attr.state) {
        case eStatePcWork_Head1:
            if (ch == 0x5A) {
                pStateCtrl->attr.state = eStatePcWork_Head2;
            }
            break;

        case eStatePcWork_Head2:
            if (ch == 0xA5) {
                pStateCtrl->attr.state = eStatePcWork_Type;
            } else if (ch == 0x5A) {
                // Stay in the current state for the next byte
            } else {
                pStateCtrl->attr.state = eStatePcWork_Head1;
            }
            break;

        case eStatePcWork_Type:
            pData->type = (uint16_t)ch; // Fill the type byte
            pStateCtrl->attr.state = eStatePcWork_Len1;
            break;

        case eStatePcWork_Len1:
            pData->len = (uint16_t)ch << 8; // Fill the high byte of length
            pStateCtrl->attr.state = eStatePcWork_Len2;
            break;

        case eStatePcWork_Len2:
            pData->len |= (uint16_t)ch; // Fill the low byte of length
            pStateCtrl->attr.state = eStatePcWork_Data;
            pData->dataIndex = 0; // Reset dataIndex
            break;

        case eStatePcWork_Data:
            // Fill the data bytes based on length information
            if (pData->dataIndex < pData->len) {
                pData->data[pData->dataIndex++] = (uint8_t)ch;
                if (pData->dataIndex == pData->len) {
                    // If all data bytes are received, process the data
                    #if 0
                    dx_kprintf("Type: 0x%x\n", pData->type);
                    dx_kprintf("Data: ");
                    for (int i = 0; i < pData->len; ++i) {
                        dx_kprintf("0x%x ", pData->data[i]);
                    }
                    dx_kprintf("\n");
                    #endif
                    if(pData->type < eUartDataType_Max && guartDatCb[pData->type] != NULL)
                        guartDatCb[pData->type](pData);
                    // Reset state to wait for the next data
                    pStateCtrl->attr.state = eStatePcWork_Head1;
                }
            }
            break;

        default:
            break;
    }
    return 0;
}

static int StateSwitchInit(void *p) {
    // 初始化状态机
    return 0;
}

static int StateSwitchExit(void *p) {
    // 退出状态机
    return 0;
}



int UartPCCmdProcess(unsigned char ch)   
{
    if (gStateUartPCWork.punit->init)
        gStateUartPCWork.punit->init((void *)&gStateUartPCWork);
    if (gStateUartPCWork.punit->handlers)
        gStateUartPCWork.punit->handlers((void *)&gStateUartPCWork, ch);

}

void CBPCWork_GetVer(void * p)
{
    dx_kprintf("Hello World\r\n");
}

void CBPCWork_Switch2Shell(void * p)
{
    dx_kprintf("Switch to Shell\r\n");
    Event_Put(EVENT_CMD_SWITCH_SHELL);
}

