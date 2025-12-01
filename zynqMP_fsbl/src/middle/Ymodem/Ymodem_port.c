#include "Ymodem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userFirmwareUpdate.h"


// 用户自定义的文件和内存管理函数，用固定变量和模拟行为代替
#define RX_BUFFER_SIZE (10*1024) // 10KB 接收缓冲区
char * prx_file_data = 0;
static size_t rx_file_size;
char rx_file_name[128];

// 用于发送的模拟文件
static char tx_file_name[FILE_NAME_LENGTH] = "testfile.bin";
static size_t tx_file_size;
static char tx_file_data[] = "This is a dummy file content for testing Ymodem transfer.\nIt will be used to simulate a file being sent from the device.\n";



uint8 ymodem_rx_header( char* fil_nm, size_t fil_sz )
{
  int ret = 0;
  Ym_printf("RX: Received file header. Name: %s, Size: %zu\n", fil_nm, fil_sz);
  // strcpy(rx_file_name,fil_nm);
  rx_file_size = fil_sz;
  /*
  if (rx_file_size > RX_BUFFER_SIZE) {
    return YMODEM_ERR;
  }
  */
  ret = FirewareUpdate_Init((uint8_t*)fil_nm ,rx_file_size);
  if(!ret)
  {
    return YMODEM_ERR;
  }
  return YMODEM_OK;
}

uint8 ymodem_rx_finish( uint8 status )
{
  Ym_printf("RX: Transfer finished with status: %s\n", status == YMODEM_OK ? "OK" : "ERROR");
  Ym_printf("RX: Final received data:%s\n", prx_file_data);
  Ym_printf("FileName  %s size %d\n", rx_file_name,(int)rx_file_size);
  FirewareUpdate_Over();
  return YMODEM_OK;
}

uint8 ymodem_rx_pac_get( char *buf, size_t seek, size_t size )
{

  if (seek + size > ((rx_file_size+(127))&(~127))/* || seek + size > RX_BUFFER_SIZE*/) {
    return YMODEM_ERR;
  }

  // memcpy(prx_file_data + seek, buf, size);
  FirewareUpdate_Update( (uint8_t*)buf ,size);
  Ym_printf("RX: Received packet, offset: %zu, size: %zu\n", seek, size);
  return YMODEM_OK;
}

uint8 ymodem_tx_set_fil( char* fil_nm )
{
  Ym_printf("TX: User wants to send file: %s\n", fil_nm);
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
  Ym_printf("TX: Providing file header. Name: %s, Size: %zu\n", *fil_nm, *fil_sz);
  return YMODEM_OK;
}

uint8 ymodem_tx_finish( uint8 status )
{
  Ym_printf("TX: Transfer finished with status: %s\n", status == YMODEM_OK ? "OK" : "ERROR");
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
  Ym_printf("TX: Reading packet, offset: %zu, size: %zu\n", offset, size);
  return YMODEM_OK;
}

// 底层I/O函数（为Windows串口实现）
void __putchar( char ch )
{
    outbyte(ch);
    // Ym_printf("IO_OUT: [%c]\n", ch);
}

void __putbuf( char *buf, size_t len )
{
    for (size_t i = 0; i < len; i++)
    {
      outbyte(buf[i]);
    }
    
}

// 新增：从串口读取数据
size_t __getbuf(char* buf, size_t len, uint32 timeout_ms) {

    size_t bytesRead=0;
    return bytesRead;
}
