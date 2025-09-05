#include "uart4sm.h"
#include <stdio.h>
#include <stdint.h>


static int StateSwitchInit(void *p);
static int StateSwitchExit(void *p);
static int StateHandle(void *p,char ch);

T_Uart4sm_data sprotocolData; 

static char sdatabuf[256]={0};

T_STATEMACHFuncUnit state1_unit[eState_Max] = {
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
    {StateSwitchInit, StateHandle, NULL},
};

T_STATEMACHCtrl gStateUart4Ctrl = {{eState_Head1, &sprotocolData}, state1_unit};

static int StateHandle(void *p, char ch) {
    T_STATEMACHCtrl *pStateCtrl = (T_STATEMACHCtrl *)p;
    T_Uart4sm_data *pData = (T_Uart4sm_data *)pStateCtrl->attr.usrData;

    switch (pStateCtrl->attr.state) {
        case eState_Head1:
            if (ch == 0x5A) {
                pStateCtrl->attr.state = eState_Head2;
            }
            break;

        case eState_Head2:
            if (ch == 0xA5) {
                pStateCtrl->attr.state = eState_Type;
            } else if (ch == 0x5A) {
                // Stay in the current state for the next byte
            } else {
                pStateCtrl->attr.state = eState_Head1;
            }
            break;

        case eState_Type:
            pData->type = (uint16_t)ch; // Fill the type byte
            pStateCtrl->attr.state = eState_Len1;
            break;

        case eState_Len1:
            pData->len = (uint16_t)ch << 8; // Fill the high byte of length
            pStateCtrl->attr.state = eState_Len2;
            break;

        case eState_Len2:
            pData->len |= (uint16_t)ch; // Fill the low byte of length
            pStateCtrl->attr.state = eState_Data;
            pData->dataIndex = 0; // Reset dataIndex
            break;

        case eState_Data:
            // Fill the data bytes based on length information
            if (pData->dataIndex < pData->len) {
                pData->data[pData->dataIndex++] = (uint8_t)ch;
                if (pData->dataIndex == pData->len) {
                    // If all data bytes are received, process the data
                    xprintf("Type: 0x%x\n", pData->type);
                    xprintf("Data: ");
                    for (int i = 0; i < pData->len; ++i) {
                        xprintf("0x%x ", pData->data[i]);
                    }
                    xprintf("\n");
                    // Reset state to wait for the next data
                    pStateCtrl->attr.state = eState_Head1;
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
