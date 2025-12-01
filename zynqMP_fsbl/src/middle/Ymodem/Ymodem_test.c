#if 0
#include "Ymodem.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// 添加对 usart_protocol_model_cur 的定义
#define USART_PROTOCOL_YMODEM_DNLOAD 2
uint8_t usart_protocol_model_cur;


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
            // Assemble_SOTSTX(current_byte);
            YmodemProcess(current_byte,1);
        }
        else
        {
            YmodemProcess(current_byte,0);
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
#endif