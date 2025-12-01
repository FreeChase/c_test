#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <windows.h> // For Windows specific functions like ntohs, ntohl if needed

#pragma pack(push, 1)

// 状态字位定义 (51-52字节)
typedef struct {
    uint8_t self_test_status : 2;     // DB1~DB0: 自检状态
    uint8_t pos_mode : 3;            // DB4~DB2: 定位状态
    uint8_t pos_status : 2;           // DB6~DB5: 当前是否定位状态
    uint8_t sig_proc_status : 1;      // DB7: 信号处理模块自检状态
    uint8_t storage_status : 1;       // DB8: 存储模块自检状态
    uint8_t reserved_bit9 : 1;        // DB9: 默认1b
    uint8_t anti_jam_status : 1;      // DB10: 抗干扰模块自检状态
    uint8_t pll_status : 1;           // DB11: 射频芯片锁相环模块自检状态
    uint8_t ant_status : 2;           // DB13~DB12: 天线状态
    uint8_t reserved_bits : 2;        // DB15~DB14: 保留
} StatusWord;

// 用户定义的JamStatus结构体 (6字节数组)
typedef struct {
    uint8_t data[6];
} JamStatus;

// 单颗卫星信息结构 (38字节)
typedef struct {
    uint8_t prn;            // 卫星PRN号
    uint8_t participate;    // 是否参与定位
    uint8_t snr;            // 信噪比 (dB)
    uint8_t elevation;      // 仰角 (度)
    uint16_t azimuth;       // 方位角 (度)
    int32_t pseudorange;    // 伪距 (m)
    int32_t pseudorange_rate; // 伪距率 (m/s)
    int32_t pos_x;          // 位置X (m)
    int32_t pos_y;          // 位置Y (m)
    int32_t pos_z;          // 位置Z (m)
    int32_t vel_x;          // 速度Vx (m/s)
    int32_t vel_y;          // 速度Vy (m/s)
    int32_t vel_z;          // 速度Vz (m/s)
} SatelliteInfo;

// 卫星定位信息结构体 (0x36协议)
typedef struct {
    // 帧头
    uint8_t header1;        // 0x55
    uint8_t header2;        // 0xAA
    
    // 基本信息
    uint16_t data_length;   // 有效数据字节数
    uint8_t info_flag;      // 0x36 (卫星定位信息)
    
    // 定位信息
    uint8_t pos_flag;       // 定位标识
    uint16_t date_ymd;      // 年月日
    uint32_t time_24h;      // 24小时时间 (us)
    uint16_t bd_week;       // 北斗周数
    uint32_t bd_week_ms;    // 北斗周秒 (ms)
    int32_t longitude;      // 经度
    int32_t latitude;       // 纬度
    int32_t height;         // 高度
    int32_t velocity_e;     // 东向速度
    int32_t velocity_n;     // 北向速度
    int32_t velocity_u;     // 天向速度
    uint8_t reserved1;      // 保留
    uint8_t vin;            // 可视卫星数
    uint8_t reserved2;      // 保留
    uint8_t van;            // 可用卫星数
    uint16_t pdop;          // PDOP值
    
    // 状态信息
    uint8_t destroy_key;    // 毁钥状态
    uint8_t pen;            // 军码民码状态
    StatusWord status;      // 状态字
    uint8_t reserved3;      // 保留
    JamStatus jam_status;   // 抗干扰状态
    uint8_t ant_mode;       // 天线直通状态
    
    // 卫星数据 (可变部分)
    SatelliteInfo sats[1];  // 卫星信息数组 (实际数量由vin决定)
} SatellitePositionInfo;


// 0x35协议 - 单颗卫星信息结构 (38字节)
typedef struct {
    uint8_t prn;               // 卫星PRN号 (1-255)
    uint8_t participate;       // 是否参与定位 (0-否,1-是)
    uint8_t snr;               // 信噪比 (dB, LSB=1)
    uint8_t elevation;         // 仰角 (°, 0-90, LSB=1)
    uint16_t azimuth;          // 方位角 (°, 0-360, LSB=1)
    int32_t pseudorange;       // 伪距 (m, LSB=0.1)
    int32_t pseudorange_rate;  // 伪距率 (m/s, LSB=0.01)
} T_AJRSatelliteInfo_Telemetry;

// 状态字位定义 (2字节)
typedef struct {
    uint8_t self_test_status : 2;  // 自检状态 (00b-默认,01b-成功,11b-失败)
    uint8_t pos_mode : 3;         // 定位状态 (000b-B3I,001b-B1I,010b-GPS等)
    uint8_t pos_status : 2;       // 定位情况 (00b-定位,01b-未定位)
    uint8_t sig_proc_status : 1;  // 信号处理模块状态 (0b-正常,1b-故障)
    uint8_t storage_status : 1;   // 存储模块状态 (0b-正常,1b-故障)
    uint8_t reserved_bit9 : 1;    // 保留位 (默认1b)
    uint8_t anti_jam_status : 1;  // 抗干扰模块状态 (0b-正常,1b-故障)
    uint8_t pll_status : 1;       // 锁相环状态 (0b-正常,1b-故障)
    uint8_t ant_status : 2;       // 天线状态 (00b-默认,01b-机载,10b-弹载)
    uint8_t reserved_bits : 2;    // 保留位 (默认为0)
} AJRCheckStatus;

// 0x35协议 - 完整的卫星定位信息结构体
typedef struct {
    // 帧头 (2字节)
    uint8_t header1;             // 0x55
    uint8_t header2;             // 0xAA
    
    // 基本信息 (3字节)
    uint16_t data_length;       // 有效数据字节数
    uint8_t info_flag;          // 0x35 (遥测信息)
    
    // 定位基本信息 (45字节)
    uint8_t pos_flag;           // 定位状态 (1字节)
    uint16_t weeknum;           // 周数 (2字节)
    uint32_t secondofweek;       // 周秒 (4字节)
    int32_t longitude;          // 经度 (rad, LSB=10^-8, 东经为正)
    int32_t latitude;           // 纬度 (rad, LSB=10^-8, 北纬为正)
    int32_t height;             // 高度 (m, LSB=0.01)
    int32_t velocity_e;         // 东向速度 (m/s, LSB=0.01)
    int32_t velocity_n;         // 北向速度 (m/s, LSB=0.01)
    int32_t velocity_u;         // 天向速度 (m/s, LSB=0.01)
    uint8_t vin;                // 可视卫星数 (颗)
    uint8_t van;                // 可用卫星数 (颗)
    uint16_t pdop;              // PDOP值 (LSB=0.01)
    int32_t posiResidual;       // 位置残差
    int32_t veloResidual;       // 速度残差
    int32_t clockCorrection;    // 钟差
    int32_t clockBias;          // 钟漂
    uint8_t civiliFlag;         // 民码标识 (0-民码,1-军码)
    AJRCheckStatus status;      // 状态字 (2字节)
    uint8_t reserved3;          // 保留字节 (置0)
    uint8_t anti_status[6];     // 抗干扰状态字 (6字节)
    uint8_t ant_mode;           // 天线直通状态 (0-抗干扰模式,1-直通模式)
    
    // 卫星数据 (变长部分)
    T_AJRSatelliteInfo_Telemetry sats[16];  // 卫星信息数组 (最多16颗)
    uint8_t raim_fault_stalite[6];         // RAIM故障卫星数组 (6字节)
    
    // 校验码 (2字节)
    uint16_t crc;               // CRC-CCITT校验码
} T_AJRSatellitePositionInfo_Telemetry;

#pragma pack(pop)

// 自定义圆周率常量，解决M_PI未定义问题
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#if 0
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
#endif


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


// 解析抗干扰状态 (0x36协议)
void parse_jam_status(const JamStatus *jam_status) {
    // 提取第一个干扰源强度 (7位)
    uint8_t jam_strength1 = (jam_status->data[0] >> 1) & 0x7F;
    
    // 提取后续干扰源强度
    // 注意：这里的位操作需要根据实际协议文档精确调整，假设是连续的位流
    // 以下是根据原注释推测的位提取方式，可能需要根据具体协议文档修正
    uint32_t temp_val = 0;
    memcpy(&temp_val, &jam_status->data[1], 3); // 复制3字节数据
    // 假设是小端序，需要字节序转换
    // temp_val = ntohl(temp_val); // 如果数据是大端序，则需要反转
    
    // 这里根据0x36协议的JamStatus结构体定义进行位操作
    // 原始注释的位操作可能不完全符合6字节数组的连续位流解析，这里重新尝试
    // 示例解析，可能需要根据实际协议文档调整位掩码和移位
    uint8_t proc_status = (jam_status->data[0] >> 0) & 0x01;
    uint8_t interfered = (jam_status->data[0] >> 1) & 0x01;
    uint8_t jam_source_count = (jam_status->data[0] >> 2) & 0x07;

    // 假设剩余位是干扰强度，每7位一个强度值
    // 这部分解析在原代码中也被注释掉了，这里给出一种可能的实现，需要核对协议文档
    // 抗干扰强度通常以1dB为LSB，范围0-127dB
    uint64_t jam_data_64 = 0;
    memcpy(&jam_data_64, jam_status->data, 6); // 复制6字节数据到64位变量
    // 如果系统是小端序，这里可能需要反转字节序才能进行位操作

    printf("抗干扰状态:\n");
    printf("  处理状态: %s\n", proc_status ? "启用" : "默认");
    printf("  受干扰: %s\n", interfered ? "是" : "否");
    printf("  干扰源数量: %d\n", jam_source_count);
    
    // 重新实现抗干扰强度的解析，需要更精确的协议文档
    // 以下是一个假设的解析，仅供参考，请根据实际协议进行修改
    // 假设6字节数据是连续的48位，每7位代表一个强度
    if (jam_source_count > 0) {
        printf("  干扰强度: ");
        // 伪代码，实际需要根据6字节数据精确提取7位
        // 这里只是为了示例，具体位提取需要查阅协议文档
        for (int i = 0; i < jam_source_count && i < 6; ++i) { // 最多6个干扰源
            uint8_t strength = (jam_status->data[i] >> 1) & 0x7F; // 假设每字节第1位开始的7位
            if (i == 0) strength = (jam_status->data[0] >> 1) & 0x7F; // 根据旧注释
            // 真实的解析会更复杂，涉及跨字节的位操作
            printf("%.0fdB ", (double)strength);
        }
        printf("\n");
    }
}

// 解析并打印0x36协议卫星信息
void parse_and_print_0x36(SatellitePositionInfo *frame) {
    // 基础信息验证
    if (frame->header1 != 0x55 || frame->header2 != 0xAA || frame->info_flag != 0x36) {
        printf("无效帧头或协议标识: %02X %02X %02X\n", frame->header1, frame->header2, frame->info_flag);
        return;
    }
    
    // 计算完整帧大小
    size_t fixed_part_size = offsetof(SatellitePositionInfo, sats);
    // data_length 包含了从 pos_flag 到 sats[vin-1] 的数据长度，不包含帧头和 data_length 自身
    // 所以总长度是 帧头(2) + data_length(2) + data_length + CRC(2)
    size_t total_frame_size = 4 + frame->data_length + 2; 

    // CRC校验的数据是从 data_length 开始，长度为 data_length + 2 (info_flag + data_length)
    // 实际CRC校验的是 data_length 之后的所有数据，直到 CRC 字段之前
    uint16_t calc_crc = calcCRC16((uint8_t*)&frame->data_length, frame->data_length+2); // +1 是因为 info_flag 在 data_length 之后
    // 真实的CRC是整个数据包有效载荷的CRC，从 info_flag 到 raim_fault_stalite
    // 这里需要确认协议文档，一般CRC校验是从 data_length 后的第一个字节开始到校验码前一个字节结束
    // 0x36 协议的 CRC 校验范围是 data_length 后的所有数据直到校验码前一个字节
    // 重新计算CRC的起点和长度
    uint16_t received_crc = *(uint16_t*)((uint8_t*)frame + total_frame_size - 2);



    if (calc_crc != received_crc) {
        printf("CRC校验失败: 计算值 %04X vs 帧值 %04X\n", calc_crc, received_crc);
        // return; // 调试时可以注释掉，方便查看其他数据
    }
    else
    {
        printf("\n============ CRC校验合格 ============\n");
        printf(" 计算值 %04X vs 帧值 %04X\n", calc_crc, received_crc);
    }
    
    printf("\n============ 卫星定位信息 (0x36协议) ============\n");
    
    // 基础定位信息
    uint16_t date = frame->date_ymd;
    int year = 2000 + ((date >> 9) & 0x7F);
    int month = (date >> 5) & 0x0F;
    int day = date & 0x1F;
    
    printf("时间: %04d-%02d-%02d %uus\n", year, month, day, frame->time_24h * 40);
    printf("时间: %02dH-%02dM-%02dS \n", (frame->time_24h /1000000 * 40)/3600, ((frame->time_24h /1000000* 40)%3600)/60,((frame->time_24h /1000000* 40)%60));
    printf("定位状态: %s\n", frame->pos_flag ? "已定位" : "未定位");
    printf("位置: 经度 %.6f°, 纬度 %.6f°, 高度 %.2fm\n", 
           (double)frame->longitude * 0.00000001 * (180.0 / M_PI),
           (double)frame->latitude * 0.00000001 * (180.0 / M_PI),
           (double)frame->height * 0.01);
    printf("速度: 东向 %.2fm/s, 北向 %.2fm/s, 天向 %.2fm/s\n",
           (double)frame->velocity_e * 0.01,
           (double)frame->velocity_n * 0.01,
           (double)frame->velocity_u * 0.01);
    
    // 北斗信息
    printf("北斗周数: %u, 周秒: %.3fs\n", frame->bd_week, frame->bd_week_ms / 1000.0);
    
    // 卫星状态
    printf("卫星: 可视%d颗, 可用%d颗, PDOP: %.2f\n", 
           frame->vin, frame->van, frame->pdop * 0.01);
    
    // 状态信息
    printf("状态信息:\n");
    printf("  毁钥状态: %d\n", frame->destroy_key);
    printf("  工作状态: %s\n", frame->pen ? "军码" : "民码");
    printf("  自检状态: %s\n", 
           frame->status.self_test_status == 1 ? "成功" : 
           frame->status.self_test_status == 3 ? "失败" : "默认");
    printf("  定位模式: %d\n", frame->status.pos_mode);
    printf("  定位情况: %s\n", frame->status.pos_status == 0 ? "定位" : "未定位");
    printf("  天线状态: %s\n", 
           frame->status.ant_status == 1 ? "机载天线" : 
           frame->status.ant_status == 2 ? "弹载天线" : "默认");
    printf("  天线模式: %s\n", frame->ant_mode ? "直通模式" : "抗干扰模式");
    
    // 抗干扰状态
    parse_jam_status(&frame->jam_status);
    
    // 解析并打印卫星信息
    uint8_t actual_sat_count = frame->vin; // 0x36协议根据vin确定卫星数量
    if (actual_sat_count > 16) { // 防止越界
        actual_sat_count = 16;
    }
    printf("\n卫星详细信息(%d颗):\n", actual_sat_count);
    for (int i = 0; i < actual_sat_count; i++) {
        SatelliteInfo *sat = &frame->sats[i];
        if(sat->prn == 0) // 某些实现中，如果PRN为0表示无效卫星
            continue;
        printf("卫星 %d:\n", i + 1);
        printf("  PRN: %d, 参与定位: %s, 信噪比: %ddB\n", 
               sat->prn, sat->participate ? "是" : "否", sat->snr);
        printf("  仰角: %d°, 方位角: %d°\n", sat->elevation, sat->azimuth);
        printf("  伪距: %.1fm, 伪距率: %.4fm/s\n", 
               sat->pseudorange * 0.1, sat->pseudorange_rate * 0.01);
        printf("  位置: (%.2fm, %.2fm, %.2fm)\n", 
               sat->pos_x * 0.05, sat->pos_y * 0.05, sat->pos_z * 0.05);
        printf("  速度: (%.5fm/s, %.5fm/s, %.5fm/s)\n", 
               sat->vel_x * 0.001, sat->vel_y * 0.001, sat->vel_z * 0.001);
    }
    
    printf("======================================\n");
}

// 解析抗干扰状态 (0x35协议)
void parse_anti_status(const uint8_t anti_status[6]) {
    // 解析第一个干扰源
    uint8_t proc_status = anti_status[0] & 0x01;        // 处理状态 (位0)
    uint8_t jam_present = (anti_status[0] >> 1) & 0x01; // 受干扰 (位1)
    uint8_t jam_count = (anti_status[0] >> 2) & 0x07;   // 干扰源数量 (位2-4)
    uint8_t jam_strength1 = (anti_status[0] >> 5) & 0x07; // 干扰强度1 (位5-7)
    
    // 解析其余干扰源
    uint8_t jam_strength2 = anti_status[1] & 0x07;
    uint8_t jam_strength3 = (anti_status[1] >> 3) & 0x07;
    uint8_t jam_strength4 = ((anti_status[1] >> 6) | ((anti_status[2] & 0x01) << 2)) & 0x07;
    uint8_t jam_strength5 = (anti_status[2] >> 1) & 0x07;
    uint8_t jam_strength6 = (anti_status[2] >> 4) & 0x07;
    
    // 打印解析结果
    printf("抗干扰状态:\n");
    printf("  处理状态: %s\n", proc_status ? "启用" : "默认");
    printf("  受干扰: %s\n", jam_present ? "是" : "否");
    printf("  干扰源数量: %d\n", jam_count);
    printf("  干扰强度: [%d] %.0fdB, [%d] %.0fdB, [%d] %.0fdB, [%d] %.0fdB, [%d] %.0fdB, [%d] %.0fdB\n", 
           1, jam_strength1 * 10.0, // 假设LSB是10dB
           2, jam_strength2 * 10.0,
           3, jam_strength3 * 10.0,
           4, jam_strength4 * 10.0,
           5, jam_strength5 * 10.0,
           6, jam_strength6 * 10.0);
}


// 解析并打印0x35协议卫星信息
void parse_and_print_0x35(T_AJRSatellitePositionInfo_Telemetry *frame) {
    // 基础信息验证
    if (frame->header1 != 0x55 || frame->header2 != 0xAA || frame->info_flag != 0x35) {
        printf("无效帧头或协议标识: %02X %02X %02X\n", 
               frame->header1, frame->header2, frame->info_flag);
        return;
    }
    
    // CRC校验的数据范围: 从 data_length 字段开始到 raim_fault_stalite 数组结束
    // data_length 表示的是 info_flag 到 raim_fault_stalite 数组结束的长度
    size_t data_len_for_crc = frame->data_length+2; 
    uint8_t *crc_data_start = (uint8_t*)&(frame->data_length);
    
    // 接收到的CRC是帧的最后2字节
    size_t total_frame_size = 4 + frame->data_length + 2; // 帧头(2) + data_length(2) + data_length + CRC(2)
    uint16_t received_crc = *(uint16_t*)((uint8_t*)frame + total_frame_size - 2);

    uint16_t calc_crc = calcCRC16(crc_data_start, data_len_for_crc);
    
    if (calc_crc != received_crc) {
        printf("CRC校验失败: 计算值 %04X vs 帧值 %04X\n", calc_crc, received_crc);
        // return; // 调试时可以注释掉，方便查看其他数据
    }
    
    printf("\n============ 0x35协议 - 卫星定位信息 ============\n");
    
    // 基础定位信息
    printf("时间: 北斗周数 %u, 周秒 %.3fs\n", 
           frame->weeknum, frame->secondofweek / 1000.0);
    printf("定位状态: %s\n", frame->pos_flag ? "已定位" : "未定位");
    
    // 经纬度转换 (原始单位为弧度, LSB=1e-8)
    double lon_deg = (double)frame->longitude * (180.0 / (M_PI * 1e8));
    double lat_deg = (double)frame->latitude * (180.0 / (M_PI * 1e8));
    
    printf("位置: 经度 %.8f°, 纬度 %.8f°, 高度 %.2fm\n", 
           lon_deg, lat_deg, (double)frame->height * 0.01);
    printf("速度: 东向 %.2fm/s, 北向 %.2fm/s, 天向 %.2fm/s\n",
           (double)frame->velocity_e * 0.01,
           (double)frame->velocity_n * 0.01,
           (double)frame->velocity_u * 0.01);
    
    // 精度信息
    printf("精度指标: PDOP: %.2f, 位置残差: %.1f, 速度残差: %.1f\n", 
           frame->pdop * 0.01, 
           (double)frame->posiResidual * 0.01, // 假设LSB为0.01
           (double)frame->veloResidual * 0.01); // 假设LSB为0.01
    
    // 时钟信息
    printf("时钟: 钟差 %.1fns, 钟漂 %.1fns/s\n",
           (double)frame->clockCorrection * 0.1,
           (double)frame->clockBias * 0.1);
    
    // 卫星状态
    printf("卫星: 可视%d颗, 可用%d颗\n", frame->vin, frame->van);
    
    // 状态信息
    printf("状态信息:\n");
    printf("  工作模式: %s\n", frame->civiliFlag ? "军码" : "民码");
    printf("  自检状态: %s\n", 
           frame->status.self_test_status == 1 ? "成功" : 
           frame->status.self_test_status == 3 ? "失败" : "默认");
    printf("  定位模式: %d (0=B3I,1=B1I,2=GPS)\n", frame->status.pos_mode);
    printf("  定位情况: %s\n", frame->status.pos_status == 0 ? "定位" : "未定位");
    printf("  天线状态: %s\n", 
           frame->status.ant_status == 1 ? "机载天线" : 
           frame->status.ant_status == 2 ? "弹载天线" : "默认");
    printf("  天线模式: %s\n", frame->ant_mode ? "直通模式" : "抗干扰模式");
    
    // 抗干扰状态
    parse_anti_status(frame->anti_status);
    
    // 解析并打印卫星信息
    uint8_t actual_sat_count = frame->vin; // 0x35协议也应该根据vin来确定数量，而不是固定16
    if (actual_sat_count > 16) {
        actual_sat_count = 16;
    }
    printf("\n卫星详细信息(%d颗):\n", actual_sat_count);
    for (int i = 0; i < actual_sat_count; i++) {
        T_AJRSatelliteInfo_Telemetry *sat = &frame->sats[i];
        if(sat->prn == 0) // 如果PRN为0表示无效卫星
            continue;
        
        printf("卫星 %d: PRN=%d, 参与定位=%s\n", 
               i + 1, sat->prn, sat->participate ? "是" : "否");
        printf("  信噪比: %ddB, 仰角: %d°, 方位角: %d°\n", 
               sat->snr, sat->elevation, sat->azimuth);
        printf("  伪距: %.1fm, 伪距率: %.4fm/s\n", 
               sat->pseudorange * 0.1, sat->pseudorange_rate * 0.01);
    }
    
    // 解析RAIM故障卫星
    printf("\nRAIM故障卫星: ");
    int raim_fault_found = 0;
    uint8_t* pFaultSat = (uint8_t*)&frame->sats[actual_sat_count];

    for (int i = 0; i < 6; i++) {
        if (pFaultSat[i] != 0) {
            printf("%d ", pFaultSat[i]);
            raim_fault_found = 1;
        }
    }

    if (!raim_fault_found) {
        printf("无");
    }
    printf("\n");
    
    printf("=================================================\n");
}

// 将ASCII码表示的十六进制字符串转换为字节数组
// "55AA36..." -> {0x55, 0xAA, 0x36, ...}
long hex_to_bytes(const char *hex_str, uint8_t *byte_array, long max_len) {
    long byte_count = 0;
    long hex_len = strlen(hex_str);
    if (hex_len % 2 != 0) {
        fprintf(stderr, "错误: 十六进制字符串长度为奇数，无法解析。\n");
        return -1;
    }

    for (long i = 0; i < hex_len && byte_count < max_len; i += 2) {
        char hex_byte_str[3];
        hex_byte_str[0] = hex_str[i];
        hex_byte_str[1] = hex_str[i+1];
        hex_byte_str[2] = '\0';
        
        // 使用sscanf将两个字符的十六进制字符串转换为一个字节
        if (sscanf(hex_byte_str, "%hhx", &byte_array[byte_count]) != 1) {
            fprintf(stderr, "错误: 无效的十六进制字符 '%s' 在位置 %ld。\n", hex_byte_str, i);
            return -1;
        }
        byte_count++;
    }
    return byte_count;
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("用法: %s <ASCII_hex_文件> <协议类型(0x36或0x35)>\n", argv[0]);
        printf("示例: %s data.txt 0x36\n", argv[0]);
        return 1;
    }
    
    // 打开ASCII十六进制文本文件
    FILE *file = fopen(argv[1], "r"); // 注意这里是 "r" 而不是 "rb"
    if (!file) {
        perror("打开文件失败");
        return 1;
    }
    
    // 读取文件内容到字符串
    fseek(file, 0, SEEK_END);
    long file_char_len = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_char_len <= 0) {
        printf("文件为空\n");
        fclose(file);
        return 1;
    }

    buildTableCRC_CCITT();

    uint16_t calc_crc = calcCRC16("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890", 1000);
    printf("HelloCrc :0x%x\r\n",calc_crc);
    uint16_t hex = 0x18e;
    calc_crc = calcCRC16((char*)&hex, 2);
    printf("hex0x18e 1 crc :0x%x\r\n",calc_crc);
    calc_crc = calcCRC16((char*)&hex, 2);
    printf("hex0x18e 2 crc :0x%x\r\n",calc_crc);

    // 分配足够的空间来存储ASCII十六进制字符串
    char *hex_string_buffer = (char*)malloc(file_char_len + 1);
    if (!hex_string_buffer) {
        perror("内存分配失败");
        fclose(file);
        return 1;
    }
    hex_string_buffer[file_char_len] = '\0'; // 确保字符串以null结尾
    
    size_t chars_read = fread(hex_string_buffer, 1, file_char_len, file);
    fclose(file);
    
    if (chars_read != (size_t)file_char_len) {
        printf("文件读取不完整\n");
        free(hex_string_buffer);
        return 1;
    }

    // 移除可能的换行符、空格等非十六进制字符，仅保留十六进制字符
    char *clean_hex_string = (char*)malloc(file_char_len + 1); // 额外空间以防万一
    if (!clean_hex_string) {
        perror("内存分配失败");
        free(hex_string_buffer);
        return 1;
    }
    long clean_len = 0;
    for (long i = 0; i < file_char_len; ++i) {
        if (isxdigit(hex_string_buffer[i])) { // 检查是否是十六进制数字
            clean_hex_string[clean_len++] = hex_string_buffer[i];
        }
    }
    clean_hex_string[clean_len] = '\0';
    free(hex_string_buffer); // 释放原始读取的缓冲区
    hex_string_buffer = clean_hex_string; // 现在 hex_string_buffer 指向清理后的字符串
    file_char_len = clean_len;

    // 将十六进制字符串转换为字节数组
    // 一个字节由两个十六进制字符表示，所以字节数组长度是字符串长度的一半
    long buffer_size_bytes = file_char_len / 2;
    uint8_t *byte_buffer = (uint8_t*)malloc(buffer_size_bytes);
    if (!byte_buffer) {
        perror("内存分配失败");
        free(hex_string_buffer);
        return 1;
    }
    
    long bytes_converted = hex_to_bytes(hex_string_buffer, byte_buffer, buffer_size_bytes);
    free(hex_string_buffer); // 释放十六进制字符串缓冲区

    if (bytes_converted == -1 || bytes_converted == 0) {
        printf("十六进制数据转换失败或为空。\n");
        free(byte_buffer);
        return 1;
    }
    
    uint8_t hex_cmd = (uint8_t)strtol(argv[2], NULL, 16); // 将协议类型字符串转换为十六进制数值

    // 处理数据帧
    long offset = 0;
    int frame_count = 0;
    
    // 最小帧大小的假设，用于跳过无效数据
    const size_t min_frame_size_0x36 = sizeof(SatellitePositionInfo) - sizeof(SatelliteInfo) + sizeof(SatelliteInfo); // 假设至少一颗卫星
    const size_t min_frame_size_0x35 = offsetof(T_AJRSatellitePositionInfo_Telemetry, sats); // 0x35协议的固定部分

    while (offset < bytes_converted) {
        // 确保有足够的字节来读取帧头和数据长度字段
        if (offset + 5 > bytes_converted) { // 2(header) + 2(data_length) + 1(info_flag)
            printf("数据末尾，剩余字节不足以构成完整帧头。\n");
            break;
        }

        // 查找帧头 0x55 0xAA
        if (byte_buffer[offset] == 0x55 && byte_buffer[offset + 1] == 0xAA) {
            uint8_t current_info_flag = byte_buffer[offset + 4]; // info_flag 位于第5个字节 (索引4)
            uint16_t current_data_length = *(uint16_t*)(byte_buffer + offset + 2); // data_length 位于第3-4字节 (索引2-3)

            // 根据协议类型进行解析
            if (current_info_flag == hex_cmd) {
                if (hex_cmd == 0x36) {
                    // 0x36协议的完整帧大小 = 帧头(2) + data_length(2) + data_length + CRC(2)
                    size_t frame_size_0x36 = 4 + current_data_length + 2;

                    if (offset + frame_size_0x36 > bytes_converted) {
                        printf("0x36协议帧数据不完整，跳过此帧。\n");
                        offset++; // 移动一个字节继续搜索
                        continue;
                    }
                    SatellitePositionInfo *frame = (SatellitePositionInfo*)(byte_buffer + offset);
                    parse_and_print_0x36(frame);
                    offset += frame_size_0x36;
                    frame_count++;
                } else if (hex_cmd == 0x35) {
                    // 0x35协议的完整帧大小 = 帧头(2) + data_length(2) + data_length + CRC(2)
                    size_t frame_size_0x35 = 4 + current_data_length + 2;

                    if (offset + frame_size_0x35 > bytes_converted) {
                        printf("0x35协议帧数据不完整，跳过此帧。\n");
                        offset++; // 移动一个字节继续搜索
                        continue;
                    }
                    T_AJRSatellitePositionInfo_Telemetry *frame = (T_AJRSatellitePositionInfo_Telemetry*)(byte_buffer + offset);
                    parse_and_print_0x35(frame);
                    offset += frame_size_0x35;
                    frame_count++;
                } else {
                    printf("不支持的协议类型: 0x%02X\n", hex_cmd);
                    offset++;
                }
            } else {
                offset++; // 协议类型不匹配，移动一个字节继续搜索
            }
        } else {
            offset++; // 未找到帧头，移动一个字节继续搜索
        }
    }
    
    // 统计结果
    if (frame_count == 0) {
        printf("未找到有效数据帧。\n");
    } else {
        printf("\n处理完成! 共发现 %d 个有效数据帧。\n", frame_count);
    }
    
    // 清理资源
    free(byte_buffer);
    return 0;
}