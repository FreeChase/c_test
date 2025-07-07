#include <stdint.h>   // 提供 uint8_t、uint16_t 等类型
#include <string.h>   // 提供 memcpy、strlen 等函数
#include <stdio.h>    // 提供 printf 等调试输出

// CRC-CCITT 多项式 (0x1021)，对应 X^16 + X^12 + X^5 + 1
const uint16_t polyCRC_CCITT = 0x1021;  
// 16 位 CRC 查表法的表，共 256 项
uint16_t tableCRC16[256];               

// ------------------------------
// 1. 构造 16 位 CRC 查表法的表
// ------------------------------
void buildTableCRC16(uint16_t poly) {
    for (int i = 0; i < 256; ++i) {
        uint16_t crc = 0;
        // 当前字节数据
        uint8_t byte = (uint8_t)i;    

        for (int bit = 0; bit < 8; ++bit) {
            // 高位是否为 1？
            if ((crc & 0x8000) ^ (byte & 0x80)) {  
                // 左移 + 异或多项式
                crc = (crc << 1) ^ poly;          
            } else {
                // 仅左移
                crc <<= 1;                         
            }
            // 处理下一位
            byte <<= 1;                           
        }
        // 保存当前字节对应的 CRC 余数
        tableCRC16[i] = crc;                      
    }
}

// ------------------------------
// 2. 构造 CRC-CCITT 专用表（调用通用建表函数）
// ------------------------------
void buildTableCRC_CCITT() {
    buildTableCRC16(polyCRC_CCITT);
}

// ------------------------------
// 3. 计算 CRC-CCITT 校验码
//    - 入参: data 数据指针, length 数据长度
//    - 出参: 返回 16 位 CRC 值
// ------------------------------
uint16_t calcCRC16(const uint8_t* data, uint32_t length) {
    // 初始 CRC 值（可根据需求调整，如 0x0000 或 0xFFFF）
    uint16_t crc = 0x0000;                        

    for (uint32_t i = 0; i < length; ++i) {
        // 关键逻辑：高 8 位异或当前字节，查表更新 CRC
        crc = (crc << 8) ^ tableCRC16[((crc >> 8) ^ data[i]) & 0xFF];
    }
    return crc;
}

// ------------------------------
// 4. 测试示例
// ------------------------------
int main() {
    // 1. 初始化 CRC 表（仅需调用 1 次）
    buildTableCRC_CCITT();

    // 2. 测试数据
    uint8_t testData[] = "HelloWorld";  
    uint32_t dataLen = strlen((const char*)testData);

    // 3. 计算 CRC
    uint16_t crcResult = calcCRC16(testData, dataLen);

    // 4. 输出结果（十六进制）
    printf("数据: %s\n", testData);
    printf("CRC-CCITT (0x1021) 结果: 0x%04X\n", crcResult);

    return 0;
}