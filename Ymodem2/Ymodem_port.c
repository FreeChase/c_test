#include "Ymodem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 用户自定义的文件和内存管理函数，用固定变量和模拟行为代替
#define RX_BUFFER_SIZE (1024 * 10) // 10KB 接收缓冲区
static char rx_file_data[RX_BUFFER_SIZE];
static size_t rx_file_size;

// 用于发送的模拟文件
static char tx_file_name[FILE_NAME_LENGTH] = "testfile.bin";
static size_t tx_file_size;
static char tx_file_data[] = "This is a dummy file content for testing Ymodem transfer.\nIt will be used to simulate a file being sent from the device.\n";

uint8 ymodem_rx_header( char* fil_nm, size_t fil_sz )
{
  printf("RX: Received file header. Name: %s, Size: %zu\n", fil_nm, fil_sz);
  rx_file_size = fil_sz;
  if (rx_file_size > RX_BUFFER_SIZE) {
    return YMODEM_ERR; // 文件过大，缓冲区无法容纳
  }
  return YMODEM_OK;
}

uint8 ymodem_rx_finish( uint8 status )
{
  printf("RX: Transfer finished with status: %s\n", status == YMODEM_OK ? "OK" : "ERROR");
  printf("RX: Final received data:\n%.*s\n", (int)rx_file_size, rx_file_data);
  return YMODEM_OK;
}

uint8 ymodem_rx_pac_get( char *buf, size_t seek, size_t size )
{
  if (seek + size > rx_file_size || seek + size > RX_BUFFER_SIZE) {
    return YMODEM_ERR;
  }
  memcpy(rx_file_data + seek, buf, size);
  printf("RX: Received packet, offset: %zu, size: %zu\n", seek, size);
  return YMODEM_OK;
}

uint8 ymodem_tx_set_fil( char* fil_nm )
{
  printf("TX: User wants to send file: %s\n", fil_nm);
  // 在这个模拟实现中，我们使用预定义的模拟文件
  if (strcmp(fil_nm, "testfile.bin") == 0) {
    tx_file_size = strlen(tx_file_data);
    return YMODEM_OK;
  }
  return YMODEM_ERR;
}

uint8 ymodem_tx_header( char** fil_nm, size_t *fil_sz )
{
  *fil_nm = tx_file_name;
  *fil_sz = tx_file_size;
  printf("TX: Providing file header. Name: %s, Size: %zu\n", *fil_nm, *fil_sz);
  return YMODEM_OK;
}

uint8 ymodem_tx_finish( uint8 status )
{
  printf("TX: Transfer finished with status: %s\n", status == YMODEM_OK ? "OK" : "ERROR");
  return YMODEM_OK;
}

uint8 ymodem_tx_pac_get( char *buf, size_t offset, size_t size )
{
  if (offset >= tx_file_size) {
    return YMODEM_ERR;
  }
  if (offset + size > tx_file_size) {
    size = tx_file_size - offset;
  }
  memcpy(buf, tx_file_data + offset, size);
  printf("TX: Reading packet, offset: %zu, size: %zu\n", offset, size);
  return YMODEM_OK;
}

// 底层I/O函数（为Windows控制台模拟）
void __putchar( char ch )
{
  printf("IO_OUT: [%c]\n", ch);
}

void __putbuf( char *buf, size_t len )
{
  printf("IO_OUT: ");
  for (size_t i = 0; i < len; i++) {
    printf("%02X ", (unsigned char)buf[i]);
  }
  printf("\n");
}