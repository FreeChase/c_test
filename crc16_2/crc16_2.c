#include "stdint.h"
#include "stdio.h"
#include "string.h"

// CRC-CCITT校验码计算程序
// 注意: 因生成多项式最高位一定为"1"，故略去最高位的"1"
// CRC-CCITT=X^16+X^12+X^5+X^0
const uint16_t polyCRC_CCITT = 0x1021;
uint16_t tableCRC16[256]; // 16位CRC直接查询表

void buildTableCRC16(uint16_t poly);
// 构造16位CRC-CCITT直接查询表
void buildTableCRC_CCITT()
{
    buildTableCRC16(polyCRC_CCITT);
}

// 构造16位CRC直接查询表
void buildTableCRC16(uint16_t poly)
{
    uint16_t i16, j16;
    uint16_t data16;
    uint16_t accum16;

    for (i16 = 0; i16 < 256; i16++)
    {
        data16 = (uint16_t)(i16 << 8);
        accum16 = 0;
        
        for (j16 = 0; j16 < 8; j16++)
        {
            if ((data16 ^ accum16) & 0x8000)
            {
                // poly可以用polyCRC_CCITT或其它16位生成多项式
                accum16 = (accum16 << 1) ^ poly;
            }
            else
            {
                accum16 <<= 1;
            }
            data16 <<= 1;
        }
        tableCRC16[i16] = accum16;
    }
}

// 用查表法计算cnt个字节的16位CRC-CCITT校验码
uint16_t calcCRC16(uint8_t * bytes, int cnt)
{
    int i;
    uint16_t crc16;
    crc16 = 0; // 预置初始值
    
    for (i = 0; i < cnt; i++)
    {
        crc16 = (crc16 << 8) ^ tableCRC16[(crc16 >> 8) ^ *bytes++];
    }
    return crc16;
}



// CRC-CCITT多项式
#define CRC_POLY 0x1021

// 计算CRC-CCITT校验
uint16_t crc_ccitt(const uint8_t *data, size_t len) {
    uint16_t crc = 0x0;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC_POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}



int main(void)
{
    uint16_t crcValue = 0;
    char teststr[] = "HelloWorld\x7b\x0a";

    buildTableCRC_CCITT();
    crcValue = calcCRC16(teststr, 10);
    printf("crc value1 : 0x%x ",crcValue);

    crcValue = calcCRC16(teststr, 12);
    printf("crc value2 : 0x%x ",crcValue);

    crcValue = crc_ccitt(teststr, 10);
    printf("crc value3 : 0x%x ",crcValue);

    return 0;
}