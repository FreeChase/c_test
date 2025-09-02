#include "Ymodem.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define USART_PROTOCOL_DEFAULT         0
#define USART_PROTOCOL_YMODEM_UPLOAD   1
#define USART_PROTOCOL_YMODEM_DNLOAD   2

uint8 usart_protocol_model_cur;

// 显式声明来自 Ymodem_port.c 的函数
void serial_init(const char* port_name);
void serial_close();
size_t __getbuf(char* buf, size_t len, uint32 timeout_ms);

// 新的接收数据循环
void receive_data_from_com() {
    char rx_buffer[PACKET_OVERHEAD + PACKET_1K_SIZE];
    size_t rx_len;
    
    printf("Ready to receive Ymodem transfer on COM204...\n");

    while (1) {
        // 尝试从串口读取数据，超时1秒
        rx_len = __getbuf(rx_buffer, sizeof(rx_buffer), 1000);
        // if (rx_len > 0) {
            ymodem_rx_put(rx_buffer, rx_len);
        // }
    }
}
void YmodemInit(void);
int main() {
    printf("Starting Ymodem receiver...\n\n");
    serial_init("\\\\.\\COM204");

    // 设置为Ymodem下载模式（接收模式）
    usart_protocol_model_cur = USART_PROTOCOL_YMODEM_DNLOAD;
    YmodemInit();
    receive_data_from_com();

    serial_close();
    printf("\nReceiver finished.\n");
    return 0;
}