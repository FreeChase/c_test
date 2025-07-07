#include <stdio.h>
#include <stdlib.h>

// 定义状态机的不同状态
typedef enum {
    STATE_WAITING_HEADER1,
    STATE_WAITING_HEADER2,
    STATE_WAITING_TYPE,
    STATE_WAITING_LENGTH,
    STATE_WAITING_DATA,
    STATE_DONE,
} ParseState;

void stateMachine(unsigned char current_byte, ParseState *pstate, unsigned char *ptype, unsigned char *pdataLen, unsigned char **payload) 
{
    switch (*pstate) {
        case STATE_WAITING_HEADER1:
            if (current_byte == 0x5a) {
                *pstate = STATE_WAITING_HEADER2;
            }
            break;

        case STATE_WAITING_HEADER2:
            if (current_byte == 0xa5) {
                *pstate = STATE_WAITING_TYPE;
            } else {
                *pstate = STATE_WAITING_HEADER1;
            }
            break;

        case STATE_WAITING_TYPE:
            *ptype = current_byte;
            *pstate = STATE_WAITING_LENGTH;
            break;

        case STATE_WAITING_LENGTH:
            *pdataLen = current_byte;
            *payload = (unsigned char *)malloc(*pdataLen);
            //利用payload最后一个字节计数，只支持小于256
            *(*payload + *pdataLen -1)=current_byte;
            *pstate = STATE_WAITING_DATA;
            break;

        case STATE_WAITING_DATA:
            if (*(*payload + *pdataLen -1) - 1 == 0) {
                // 数据内容已接收完毕
                // printf("*pdataLen %d offset %d\r\n",*pdataLen,*(*payload + *pdataLen -1));
                (*payload)[*pdataLen - *(*payload + *pdataLen -1)] = current_byte;
                *pstate = STATE_DONE;
            }
            else
            {
                // printf("*pdataLen %d offset %d\r\n",*pdataLen,*(*payload + *pdataLen -1));
                (*payload)[*pdataLen - *(*payload + *pdataLen -1)] = current_byte;
                *(*payload + *pdataLen -1) -= 1;
            }
            break;

        default:
            *pstate = STATE_WAITING_HEADER1;
            break;
    }
}

int main() {
    // 示例数据
    unsigned char data[] = {0x5a, 0xa5, 0x01, 0x04, 0x11, 0x22, 0x33, 0x44};
    int data_length = sizeof(data);

    ParseState state = STATE_WAITING_HEADER1;
    int index = 0;
    unsigned char type;
    unsigned char dataLen;
    unsigned char *ppayload = NULL;

    while (index < data_length) {
        unsigned char current_byte = data[index];

        stateMachine(current_byte, &state, &type, &dataLen, &ppayload);

        if (state == STATE_DONE) {
            // 数据已解析完毕
            printf("帧类型: 0x%02X\n", type);
            printf("数据内容长度: %d\n", dataLen);
            printf("具体数据内容: ");
            for (int i = 0; i < dataLen; i++) {
                printf("0x%02X ", ppayload[i]);
            }
            printf("\n");
            free(ppayload);
            state = STATE_WAITING_HEADER1; // 重置状态机
        }

        index++;
    }

    return 0;
}
