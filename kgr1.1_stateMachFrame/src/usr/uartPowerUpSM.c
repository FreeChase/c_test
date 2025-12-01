#include "uartPowerUpSM.h"
#include "xstatemach.h"
#include <stdio.h>
#include <stdint.h>
#include "userTask.h"



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
void dx_kprintf(const char *fmt, ...);
static int StateHandle(void *p,unsigned char ch) {
    T_STATEMACHCtrl *pStateCtrl = (T_STATEMACHCtrl *)p;
    T_Uart4sm_data *pData = (T_Uart4sm_data *)pStateCtrl->attr.usrData;
    static char tmpflag = 0;
#if 0
    if(pStateCtrl->attr.state == eState_Head1)
    {
        dx_kprintf("Is equal\r\n");
    }
    else
    {
        dx_kprintf("No equal\r\n");
    }
    // if (pStateCtrl->attr.state == eState_Head1)
    if (pStateCtrl->attr.state == 0)
    {
    	dx_kprintf("ch 0x%x state %d\r\n",ch,pStateCtrl->attr.state);
        if (ch == (char)0xA5)
        {
    	    dx_kprintf("ch 0x%x state %d\r\n",ch,pStateCtrl->attr.state);
            // pStateCtrl->attr.state = eState_Head2;
            pStateCtrl->attr.state = 1;
        }
    }
    else if (pStateCtrl->attr.state == eState_Head2)
    {
        if (ch == 0x5A)
        {
            dx_kprintf("ch 0x%x\r\n",ch);   
            pStateCtrl->attr.state = eState_Head3;
        }
        else if (ch == (char)0xA5)
        {
            pStateCtrl->attr.state = eState_Head2;
        }
        else
        {
            pStateCtrl->attr.state = eState_Head1;
        }
    }
    else if (pStateCtrl->attr.state == eState_Head3)
    {
        dx_kprintf("ch 0x%x\r\n",ch);
        if (ch == (char)0x5A)
        {
            pStateCtrl->attr.state = eState_Head4;
        }
        else if (ch == (char)0xA5)
        {
            pStateCtrl->attr.state = eState_Head2;
        }
        else
        {
            pStateCtrl->attr.state = eState_Head1;
        }
    }
    else if (pStateCtrl->attr.state == eState_Head4)
    {
        dx_kprintf("ch 0x%x\r\n",ch);
        if (ch == (char)0xA5)
        {
            pStateCtrl->attr.state = eState_UPDATE;
        }
        else
        {
            pStateCtrl->attr.state = eState_Head1;
        }
    }
    else if (pStateCtrl->attr.state == eState_UPDATE)
    {
    }
#endif
#if 1
    switch (pStateCtrl->attr.state)
    {
        case eState_Head1:
            dx_kprintf("ch 0x%x state %d\r\n",ch,pStateCtrl->attr.state);
            if (ch == 0xA5) {
                pStateCtrl->attr.state = eState_Head2;
            }
            break;

        case eState_Head2:
            if (ch == 0x5A) {
                pStateCtrl->attr.state = eState_Head3;
            } else if (ch == 0xA5) {
                pStateCtrl->attr.state = eState_Head2;
            }else{
                pStateCtrl->attr.state = eState_Head1;
            }
            break;
        case eState_Head3:
            if (ch == 0x5A) {
                pStateCtrl->attr.state = eState_Head4;
            } else if (ch == 0xA5) {
                pStateCtrl->attr.state = eState_Head2;
            } else {
                pStateCtrl->attr.state = eState_Head1;
            }
            break;
        case eState_Head4:
            if (ch == 0xA5) {
                pStateCtrl->attr.state = eState_UPDATE;
            } else {
                pStateCtrl->attr.state = eState_Head1;
            }
            break;
        case eState_UPDATE:
            if(tmpflag == 0)
            {
                tmpflag = 1;
                dx_kprintf("Alreay Receive Update Cmd !\r\n",ch);
            }
            break;
#if 0
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
                    // xprintf("Type: 0x%x\n", pData->type);
                    // xprintf("Data: ");
                    // for (int i = 0; i < pData->len; ++i) {
                    //     xprintf("0x%x ", pData->data[i]);
                    // }
                    // xprintf("\n");

                    if(pData->type < eUartDataType_Max && guartDatCb[pData->type] != NULL)
                        guartDatCb[pData->type](pData);
                    // Reset state to wait for the next data
                    pStateCtrl->attr.state = eState_Head1;
                }
            }
            break;
#endif
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


