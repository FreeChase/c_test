#include "Ymodem.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define USART_PROTOCOL_DEFAULT         0
#define USART_PROTOCOL_YMODEM_UPLOAD   1
#define USART_PROTOCOL_YMODEM_DNLOAD   2

uint8 usart_protocol_model_cur;

unsigned short crc16(const unsigned char *buf, unsigned long count);

// 新的模拟传输函数
void simulate_transfer_download(const char* filename, size_t file_size) {
    char sender_packet_buffer[PACKET_OVERHEAD + PACKET_1K_SIZE] = {0};
    unsigned short crc;
    size_t bytes_sent = 0;
    uint8_t seq_num = 1;

    // --- 1. 模拟接收方启动传输
    printf("Receiver: Starting transfer by sending 'C'\n");
    ymodem_rx_put(NULL, 0); // 模拟接收方发送 'C'

    // --- 2. 模拟发送方发送文件头包 (Packet 0)
    sender_packet_buffer[0] = SOH;
    sender_packet_buffer[1] = 0;
    sender_packet_buffer[2] = ~0;
    strcpy(sender_packet_buffer + PACKET_HEADER, filename);
    sprintf(sender_packet_buffer + PACKET_HEADER + strlen(filename) + 1, "%zu", file_size);
    
    // 计算并添加CRC
    crc = crc16((unsigned char*)sender_packet_buffer + PACKET_HEADER, PACKET_SIZE);
    sender_packet_buffer[PACKET_HEADER + PACKET_SIZE] = (unsigned char)(crc >> 8);
    sender_packet_buffer[PACKET_HEADER + PACKET_SIZE + 1] = (unsigned char)(crc & 0xFF);
    
    printf("Sender: Sending header packet (seq=0)...\n");
    ymodem_rx_put(sender_packet_buffer, PACKET_OVERHEAD + PACKET_SIZE);
    
    // --- 3. 循环模拟数据包传输
    while (bytes_sent < file_size) {
        size_t pac_len = (file_size - bytes_sent >= PACKET_1K_SIZE) ? PACKET_1K_SIZE : PACKET_SIZE;
        
        // 构造数据包
        memset(sender_packet_buffer, 0, sizeof(sender_packet_buffer));
        sender_packet_buffer[0] = (pac_len == PACKET_SIZE) ? SOH : STX;
        sender_packet_buffer[1] = seq_num;
        sender_packet_buffer[2] = ~seq_num;
        
        for (size_t i = 0; i < pac_len; ++i) {
            sender_packet_buffer[PACKET_HEADER + i] = (char)((bytes_sent + i) % 256);
        }

        // 计算并添加CRC
        crc = crc16((unsigned char*)sender_packet_buffer + PACKET_HEADER, pac_len);
        sender_packet_buffer[PACKET_HEADER + pac_len] = (unsigned char)(crc >> 8);
        sender_packet_buffer[PACKET_HEADER + pac_len + 1] = (unsigned char)(crc & 0xFF);

        printf("Sender: Sending data packet (seq=%d, %zu bytes)...\n", seq_num, pac_len);
        ymodem_rx_put(sender_packet_buffer, PACKET_OVERHEAD + pac_len);

        bytes_sent += pac_len;
        seq_num++;
    }
    
    // --- 4. 模拟传输结束
    char eot_char = EOT;
    printf("Sender: Sending EOT...\n");
    ymodem_rx_put(&eot_char, 1);
    
    printf("Sender: Sending EOT again after NAK response...\n");
    ymodem_rx_put(&eot_char, 1);
}

int main() {
    printf("Starting Ymodem transfer simulation...\n\n");
    
    // --- 1. 模拟下载过程 (DOWNLOAD)
    usart_protocol_model_cur = USART_PROTOCOL_YMODEM_DNLOAD;
    simulate_transfer_download("dummy_file.txt", 2048);
    
    printf("\n");
    #if 0
    // --- 2. 模拟上传过程 (UPLOAD)
    printf("--- Simulating Ymodem UPLOAD ---\n");
    usart_protocol_model_cur = USART_PROTOCOL_YMODEM_UPLOAD;
    ymodem_tx_set_fil("testfile.bin");

    char start_char = 'C';
    ymodem_tx_put(&start_char, 1);
    #endif
    printf("\nSimulation finished.\n");
    return 0;
}