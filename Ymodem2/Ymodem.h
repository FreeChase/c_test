#ifndef _M_YMODEM_H
#define _M_YMODEM_H
/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdio.h>
#include <string.h>

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
typedef unsigned int size_t;
typedef unsigned long unsigned_long;

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

// 底层I/O函数（现在是模拟的）
void __putchar( char ch );
void __putbuf( char* buf, size_t len );

#endif    //_M_YMODEM_H