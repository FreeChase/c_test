#include "Ymodem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// 用户自定义的文件和内存管理函数，用固定变量和模拟行为代替
#define RX_BUFFER_SIZE (1024 * 10) // 10KB 接收缓冲区
static char rx_file_data[RX_BUFFER_SIZE];
static size_t rx_file_size;

// 用于发送的模拟文件
static char tx_file_name[FILE_NAME_LENGTH] = "testfile.bin";
static size_t tx_file_size;
static char tx_file_data[] = "This is a dummy file content for testing Ymodem transfer.\nIt will be used to simulate a file being sent from the device.\n";

// 全局串口句柄
HANDLE hCom = INVALID_HANDLE_VALUE;

// 串口初始化函数
void serial_init(const char* port_name) {
    if (hCom != INVALID_HANDLE_VALUE) {
        printf("Serial port already open.\n");
        return;
    }

    hCom = CreateFileA(port_name,
                      GENERIC_READ | GENERIC_WRITE,
                      0,
                      NULL,
                      OPEN_EXISTING,
                      0,
                      NULL);

    if (hCom == INVALID_HANDLE_VALUE) {
        printf("Error opening serial port %s. Error code: %lu\n", port_name, GetLastError());
        return;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hCom, &dcbSerialParams)) {
        printf("Error getting serial port state.\n");
        CloseHandle(hCom);
        hCom = INVALID_HANDLE_VALUE;
        return;
    }

    dcbSerialParams.BaudRate = CBR_115200; // 假设波特率为115200
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hCom, &dcbSerialParams)) {
        printf("Error setting serial port state.\n");
        CloseHandle(hCom);
        hCom = INVALID_HANDLE_VALUE;
        return;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hCom, &timeouts);

    printf("Serial port %s opened successfully.\n", port_name);
}

// 串口关闭函数
void serial_close() {
    if (hCom != INVALID_HANDLE_VALUE) {
        CloseHandle(hCom);
        hCom = INVALID_HANDLE_VALUE;
        printf("Serial port closed.\n");
    }
}

uint8 ymodem_rx_header( char* fil_nm, size_t fil_sz )
{
  printf("RX: Received file header. Name: %s, Size: %zu\n", fil_nm, fil_sz);
  rx_file_size = fil_sz;
  if (rx_file_size > RX_BUFFER_SIZE) {
    return YMODEM_ERR;
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
  if (seek + size > ((rx_file_size+(127))&(~127)) || seek + size > RX_BUFFER_SIZE) {
    return YMODEM_ERR;
  }
  memcpy(rx_file_data + seek, buf, size);
  printf("RX: Received packet, offset: %zu, size: %zu\n", seek, size);
  return YMODEM_OK;
}

uint8 ymodem_tx_set_fil( char* fil_nm )
{
  printf("TX: User wants to send file: %s\n", fil_nm);
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

// 底层I/O函数（为Windows串口实现）
void __putchar( char ch )
{
    if (hCom == INVALID_HANDLE_VALUE) {
        printf("Error: Serial port not open.\n");
        return;
    }
    DWORD bytesWritten;
    WriteFile(hCom, &ch, 1, &bytesWritten, NULL);
    printf("IO_OUT: [%c]\n", ch);
}

void __putbuf( char *buf, size_t len )
{
    if (hCom == INVALID_HANDLE_VALUE) {
        printf("Error: Serial port not open.\n");
        return;
    }
    DWORD bytesWritten;
    WriteFile(hCom, buf, len, &bytesWritten, NULL);
    printf("IO_OUT: ");
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", (unsigned char)buf[i]);
    }
    printf("\n");
}

// 新增：从串口读取数据
size_t __getbuf(char* buf, size_t len, uint32 timeout_ms) {
    if (hCom == INVALID_HANDLE_VALUE) {
        return 0;
    }

    COMMTIMEOUTS timeouts;
    GetCommTimeouts(hCom, &timeouts);
    timeouts.ReadTotalTimeoutConstant = timeout_ms;
    SetCommTimeouts(hCom, &timeouts);

    DWORD bytesRead;
    ReadFile(hCom, buf, len, &bytesRead, NULL);
    
    // 恢复原来的超时设置
    timeouts.ReadTotalTimeoutConstant = 50;
    SetCommTimeouts(hCom, &timeouts);

    return bytesRead;
}