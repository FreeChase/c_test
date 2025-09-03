#ifndef _M_YMODEM_H
#define _M_YMODEM_H
/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
/*********************************************************************
 * CONSTANTS
 */
#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)     /* start, block, block-complement */
#define PACKET_TRAILER          (2)     /* CRC bytes */
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)
#define PACKET_TIMEOUT          (1)

#define INITIAL

#define FILE_NAME_LENGTH (64)
#define FILE_SIZE_LENGTH (16)

#define YMODEM_OK               0
#define YMODEM_ERR              1
#define YMODEM_PAC_EMPTY        2


#define YMODEM_DATA_SIZE_128 128
#define YMODEM_DATA_SIZE_1024 1024

#define YMODEM_RX_IDLE 0
#define YMODEM_RX_ACK 1
#define YMODEM_RX_EOT 2
#define YMODEM_RX_SOTNULL 3
#define YMODEM_RX_ERR 4
#define YMODEM_RX_EXIT 5

#define YMODEM_TX_IDLE 0
#define YMODEM_TX_IDLE_ACK 1
#define YMODEM_TX_DATA 2
#define YMODEM_TX_DATA_ACK 3
#define YMODEM_TX_EOT 4
#define YMODEM_TX_ERR 5
#define YMODEM_TX_EXIT 6

/* ASCII control codes: */
#define SOH (0x01)      /* start of 128-byte data packet */
#define STX (0x02)      /* start of 1024-byte data packet */
#define EOT (0x04)      /* end of transmission */
#define ACK (0x06)      /* receive OK */
#define NAK (0x15)      /* receiver error; retry */
#define CAN (0x18)      /* two of these in succession aborts transfer */
#define CNC (0x43)      /* character 'C' */

/* Number of consecutive receive errors before giving up: */
#define MAX_ERRORS    (5)

/*********************************************************************
 * TYPE_DEFS
 */
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned short u16;
typedef unsigned char u8;
//typedef unsigned int size_t;
typedef unsigned long unsigned_long;

typedef struct{
    uint8_t newdata;    //1.newdata
    uint8_t u8packteNum;   
}T_YmodemInfo;


//**************************************
//* 字节流部分
// 状态机定义
typedef enum {
    STATE_WAITING_SOH,
    STATE_WAITING_PKT_NUM,
    STATE_WAITING_PKT_NUM_INV,
    STATE_WAITING_DATA,
    STATE_WAITING_CRC_HI,
    STATE_WAITING_CRC_LO,
} YmodemParseState;
//**************************************
/*********************************************************************
 * FUNCTIONS
 *********************************************************************/
void ymodem_rx_put( char *buf, size_t rx_sz );
void ymodem_tx_put( char *buf, size_t rx_sz );
uint8 ymodem_tx_set_fil( char* fil_nm );

// 用户实现的函数（现在是模拟的）
uint8 ymodem_rx_header( char* fil_nm, size_t fil_sz );
uint8 ymodem_rx_finish( uint8 status );
uint8 ymodem_rx_pac_get( char *buf, size_t offset, size_t size );

uint8 ymodem_tx_header( char  **fil_nm, size_t *fil_sz );
uint8 ymodem_tx_finish( uint8 status );
uint8 ymodem_tx_pac_get( char *buf, size_t offset, size_t size );

void Assemble_SOTSTX(char current_byte);
void YmodemProcess(char s8InputByte,char isValid);
// 底层I/O函数（现在是模拟的）
void __putchar( char ch );
void __putbuf( char* buf, size_t len );

#endif    //_M_YMODEM_H