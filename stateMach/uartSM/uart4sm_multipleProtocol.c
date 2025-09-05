#include "uart4sm.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 声明特定协议的处理函数
static int protocol_5a_a5_handler(void *p, uint8_t ch);
// 这里可以声明其他协议的处理函数，例如：
// static int protocol_aa_55_handler(void *p, uint8_t ch);

// 定义协议列表，将最长的帧头放在最前面以优先匹配
// 这个列表是实现多协议解析的核心
static const ProtocolDef protocols[] = {
    // 协议1: 5A A5 协议
    { .header = (const uint8_t[]){0x5A, 0xA5}, .header_len = 2, .handler = protocol_5a_a5_handler },
    // 协议2: 你可以添加其他协议，例如 { .header = (const uint8_t[]){0xAA, 0x55}, .header_len = 2, .handler = protocol_aa_55_handler },
};

static const size_t num_protocols = sizeof(protocols) / sizeof(protocols[0]);

// 通用状态机变量
static T_Uart4sm_data sprotocolData; 
static const ProtocolDef* current_protocol = NULL;
static size_t header_match_len = 0;

static int StateHandle(void *p, char ch);

T_STATEMACHFuncUnit state1_unit[eState_Max] = {
    {NULL, StateHandle, NULL}, // eState_Head1
    {NULL, StateHandle, NULL}, // eState_HeadN
    {NULL, StateHandle, NULL}, // eState_ProtocolHandler
};

T_STATEMACHCtrl gStateUart4Ctrl = {{eState_Head1, &sprotocolData}, state1_unit};

// 状态机主处理函数
static int StateHandle(void *p, char ch) {
    T_STATEMACHCtrl *pStateCtrl = (T_STATEMACHCtrl *)p;

    switch (pStateCtrl->attr.state) {
        case eState_Head1:
            // 遍历所有协议，尝试匹配第一个字节
            for (size_t i = 0; i < num_protocols; ++i) {
                if (ch == protocols[i].header[0]) {
                    current_protocol = &protocols[i];
                    header_match_len = 1;
                    if (current_protocol->header_len > 1) {
                        pStateCtrl->attr.state = eState_HeadN;
                    } else {
                        // 如果帧头只有1个字节，直接进入协议处理
                        pStateCtrl->attr.state = eState_ProtocolHandler;
                    }
                    return 0;
                }
            }
            // 如果没有匹配，保持在当前状态
            break;

        case eState_HeadN:
            // 继续匹配帧头的后续字节
            if (header_match_len < current_protocol->header_len) {
                if (ch == current_protocol->header[header_match_len]) {
                    header_match_len++;
                    if (header_match_len == current_protocol->header_len) {
                        // 帧头匹配完成，进入协议处理状态
                        pStateCtrl->attr.state = eState_ProtocolHandler;
                    }
                } else {
                    // 匹配失败，重置状态
                    pStateCtrl->attr.state = eState_Head1;
                    current_protocol = NULL;
                    header_match_len = 0;
                }
            }
            break;

        case eState_ProtocolHandler:
            // 将控制权交给特定协议的处理函数
            if (current_protocol && current_protocol->handler) {
                if (current_protocol->handler(p, ch) == 0) {
                    // 协议处理完成，重置状态
                    pStateCtrl->attr.state = eState_Head1;
                    current_protocol = NULL;
                    header_match_len = 0;
                }
            } else {
                // 如果没有有效的处理器，重置状态
                pStateCtrl->attr.state = eState_Head1;
            }
            break;
        
        default:
            pStateCtrl->attr.state = eState_Head1;
            break;
    }
    return 0;
}

// 专门处理 5A A5 协议的函数
// 它取代了原来的 case eState_Type, eState_Len1, eState_Len2, eState_Data
static int protocol_5a_a5_handler(void *p, uint8_t ch) {
    static enum {
        TYPE,
        LEN1,
        LEN2,
        DATA,
    } sub_state = TYPE;

    T_STATEMACHCtrl *pStateCtrl = (T_STATEMACHCtrl *)p;
    T_Uart4sm_data *pData = (T_Uart4sm_data *)pStateCtrl->attr.usrData;

    switch (sub_state) {
        case TYPE:
            pData->type = (uint16_t)ch;
            sub_state = LEN1;
            break;
        
        case LEN1:
            pData->len = (uint16_t)ch << 8;
            sub_state = LEN2;
            break;

        case LEN2:
            pData->len |= (uint16_t)ch;
            pData->dataIndex = 0;
            if (pData->len == 0) {
                // 如果长度为0，直接完成
                sub_state = TYPE; // 重置子状态机
                return 0; // 返回0表示完成
            }
            sub_state = DATA;
            break;

        case DATA:
            if (pData->dataIndex < pData->len && pData->dataIndex < 256) {
                pData->data[pData->dataIndex++] = ch;
                if (pData->dataIndex == pData->len) {
                    // 数据接收完毕，可以调用回调函数
                    xprintf("Type: 0x%x\n", pData->type);
                    xprintf("Data: ");
                    for (int i = 0; i < pData->len; ++i) {
                        xprintf("0x%x ", pData->data[i]);
                    }
                    xprintf("\n");
                    sub_state = TYPE; // 重置子状态机
                    return 0; // 返回0表示完成
                }
            } else {
                // 数据溢出或错误，重置
                sub_state = TYPE;
                return -1; // 返回非0表示失败
            }
            break;
    }
    return 1; // 返回1表示需要继续接收
}

static int StateSwitchInit(void *p) {
    // 初始化状态机
    return 0;
}

static int StateSwitchExit(void *p) {
    // 退出状态机
    return 0;
}