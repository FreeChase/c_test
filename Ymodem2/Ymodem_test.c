#include "Ymodem.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// 添加对 usart_protocol_model_cur 的定义
#define USART_PROTOCOL_YMODEM_DNLOAD 2
uint8_t usart_protocol_model_cur;

// 状态机定义
typedef enum {
    STATE_WAITING_SOH,
    STATE_WAITING_PKT_NUM,
    STATE_WAITING_PKT_NUM_INV,
    STATE_WAITING_DATA,
    STATE_WAITING_CRC_HI,
    STATE_WAITING_CRC_LO,
} YmodemParseState;

// 新增的全局变量
YmodemParseState ymodem_state = STATE_WAITING_SOH;
uint16_t data_len;
char rx_buffer[PACKET_OVERHEAD + PACKET_1K_SIZE];
size_t rx_byte_count = 0;
char current_packet_type;

void process_ymodem_byte(char current_byte) {
    switch (ymodem_state) {
        case STATE_WAITING_SOH:
            if (current_byte == SOH || current_byte == STX) {
                current_packet_type = current_byte;
                rx_buffer[0] = current_byte;
                ymodem_state = STATE_WAITING_PKT_NUM;
                rx_byte_count = 1;
                data_len = (current_byte == SOH) ? PACKET_SIZE : PACKET_1K_SIZE;
            }
            break;

        case STATE_WAITING_PKT_NUM:
            rx_buffer[rx_byte_count++] = current_byte;
            ymodem_state = STATE_WAITING_PKT_NUM_INV;
            break;

        case STATE_WAITING_PKT_NUM_INV:
            rx_buffer[rx_byte_count++] = current_byte;
            uint8_t pkt_num = (uint8_t)rx_buffer[1];
            uint8_t pkt_num_inv = (uint8_t)rx_buffer[2];
            
            if ((pkt_num + pkt_num_inv) == 0xFF) {
                ymodem_state = STATE_WAITING_DATA;
            } else {
                ymodem_state = STATE_WAITING_SOH;
            }
            break;

        case STATE_WAITING_DATA:
            rx_buffer[rx_byte_count++] = current_byte;
            if (rx_byte_count - 3 >= data_len) {
                ymodem_state = STATE_WAITING_CRC_HI;
            }
            break;
        
        case STATE_WAITING_CRC_HI:
            rx_buffer[rx_byte_count++] = current_byte;
            ymodem_state = STATE_WAITING_CRC_LO;
            break;

        case STATE_WAITING_CRC_LO:
            rx_buffer[rx_byte_count++] = current_byte;
            ymodem_rx_put(rx_buffer, rx_byte_count);
            ymodem_state = STATE_WAITING_SOH;
            break;

        default:
            ymodem_state = STATE_WAITING_SOH;
            break;
    }
}

// 显式声明来自 Ymodem_port.c 的函数
void serial_init(const char* port_name);
void serial_close();
size_t __getbuf(char* buf, size_t len, uint32_t timeout_ms);
void YmodemInit(void);
void ymodem_rx_put(char* rx_buf, size_t len);

// 接收数据循环
void receive_data_from_com() {
    char current_byte;
    
    printf("Ready to receive Ymodem transfer on COM204...\n");

    while (1) {
        size_t rx_len = __getbuf(&current_byte, 1, 1000);
        if (rx_len > 0) {
            process_ymodem_byte(current_byte);
        }
        if(ymodem_state == STATE_WAITING_SOH)
        {
            __putchar('C');
        }
    }
}

int main() {
    printf("Starting Ymodem receiver...\n\n");
    serial_init("\\\\.\\COM204");

    usart_protocol_model_cur = USART_PROTOCOL_YMODEM_DNLOAD;
    YmodemInit();
    receive_data_from_com();

    serial_close();
    printf("\nReceiver finished.\n");
    return 0;
}