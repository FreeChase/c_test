#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <windows.h>

// 自定义圆周率
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#pragma pack(push, 1)

// 0x35协议 - 单颗卫星信息结构 (38字节)
typedef struct {
    uint8_t prn;               // 卫星PRN号 (1-255)
    uint8_t participate;       // 是否参与定位 (0-否,1-是)
    uint8_t snr;               // 信噪比 (dB, LSB=1)
    uint8_t elevation;         // 仰角 (°, 0-90, LSB=1)
    uint16_t azimuth;          // 方位角 (°, 0-360, LSB=1)
    int32_t pseudorange;       // 伪距 (m, LSB=0.1)
    int32_t pseudorange_rate;  // 伪距率 (m/s, LSB=0.01)
    // 以下字段在图片中未显示但占位38字节
    uint8_t reserved1[24];      // 保留字段(实际应用中应包含位置/速度信息)
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

// CRC-CCITT多项式
#define CRC_POLY 0x1021

// 计算CRC-CCITT校验
uint16_t crc_ccitt(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;
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

// 解析抗干扰状态
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
           1, jam_strength1 * 10, 
           2, jam_strength2 * 10,
           3, jam_strength3 * 10,
           4, jam_strength4 * 10,
           5, jam_strength5 * 10,
           6, jam_strength6 * 10);
}

// 解析并打印0x35协议卫星信息
void parse_and_print_0x35(T_AJRSatellitePositionInfo_Telemetry *frame) {
    // 基础信息验证
    if (frame->header1 != 0x55 || frame->header2 != 0xAA || frame->info_flag != 0x35) {
        printf("无效帧头或协议标识: %02X %02X %02X\n", 
               frame->header1, frame->header2, frame->info_flag);
        return;
    }
    
    // 转换多字节整数的字节序
    // frame->data_length = ntohs(frame->data_length);
    // frame->weeknum = ntohs(frame->weeknum);
    // frame->secondofweek = ntohl(frame->secondofweek);
    // frame->longitude = ntohl(frame->longitude);
    // frame->latitude = ntohl(frame->latitude);
    // frame->height = ntohl(frame->height);
    // frame->velocity_e = ntohl(frame->velocity_e);
    // frame->velocity_n = ntohl(frame->velocity_n);
    // frame->velocity_u = ntohl(frame->velocity_u);
    // frame->pdop = ntohs(frame->pdop);
    // frame->posiResidual = ntohl(frame->posiResidual);
    // frame->veloResidual = ntohl(frame->veloResidual);
    // frame->clockCorrection = ntohl(frame->clockCorrection);
    // frame->clockBias = ntohl(frame->clockBias);
    
    // 计算完整帧大小
    size_t fixed_size = offsetof(T_AJRSatellitePositionInfo_Telemetry, sats);
    uint8_t sat_count = frame->vin;
    
    // 修正卫星数量错误
    if (sat_count > 16) {
        printf("警告: 可视卫星数(%d)超过最大值(16)，已修正为16\n", sat_count);
        sat_count = 16;
    }
    
    size_t sat_size = sat_count * sizeof(T_AJRSatelliteInfo_Telemetry);
    size_t total_size = fixed_size + sat_size + 6 + 2; // + RAIM + CRC
    uint8_t *frame_ptr = (uint8_t*)frame;
    uint16_t frame_crc = (*(uint16_t*)(frame_ptr + total_size - 2));
    
    // 计算CRC校验 (从帧头到RAIM之前)
    uint16_t calc_crc = crc_ccitt(frame_ptr, total_size - 2);
    
    if (calc_crc != frame_crc) {
        printf("CRC校验失败: 计算值 %04X vs 帧值 %04X\n", calc_crc, frame_crc);
        return;
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
           (double)frame->posiResidual, 
           (double)frame->veloResidual);
    
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
    printf("\n卫星详细信息(%d颗):\n", sat_count);
    for (int i = 0; i < sat_count; i++) {
        T_AJRSatelliteInfo_Telemetry *sat = &frame->sats[i];
        
        // 转换字节序
        // sat->azimuth = ntohs(sat->azimuth);
        // sat->pseudorange = ntohl(sat->pseudorange);
        // sat->pseudorange_rate = ntohl(sat->pseudorange_rate);
        
        printf("卫星 %d: PRN=%d, 参与定位=%s\n", 
               i + 1, sat->prn, sat->participate ? "是" : "否");
        printf("  信噪比: %ddB, 仰角: %d°, 方位角: %d°\n", 
               sat->snr, sat->elevation, sat->azimuth);
        printf("  伪距: %.1fm, 伪距率: %.4fm/s\n", 
               sat->pseudorange * 0.1, sat->pseudorange_rate * 0.01);
    }
    
    // 解析RAIM故障卫星
    printf("\nRAIM故障卫星: ");
    for (int i = 0; i < 6; i++) {
        if (frame->raim_fault_stalite[i] != 0) {
            printf("%d ", frame->raim_fault_stalite[i]);
        }
    }
    printf("\n");
    
    printf("=================================================\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("用法: %s <二进制文件>\n", argv[0]);
        printf("示例: %s data.bin\n", argv[0]);
        return 1;
    }
    
    // 打开二进制文件
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("打开文件失败");
        return 1;
    }
    
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size <= 0) {
        printf("文件为空\n");
        fclose(file);
        return 1;
    }
    
    // 读取文件内容
    uint8_t *buffer = (uint8_t*)malloc(file_size);
    if (!buffer) {
        perror("内存分配失败");
        fclose(file);
        return 1;
    }
    
    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);
    
    if (bytes_read != (size_t)file_size) {
        printf("文件读取不完整\n");
        free(buffer);
        return 1;
    }
    
    // 处理数据帧
    long offset = 0;
    int frame_count = 0;
    const size_t min_frame_size = 80;  // 最小帧大小(不含卫星数据)
    
    while (offset < file_size - min_frame_size) {
        // 搜索帧头
        if (buffer[offset] == 0x55 && buffer[offset + 1] == 0xAA) {
            T_AJRSatellitePositionInfo_Telemetry *frame = 
                (T_AJRSatellitePositionInfo_Telemetry*)(buffer + offset);
            
            // 计算最小帧大小
            size_t frame_size = min_frame_size;
            
            // 检查帧完整性
            if (offset + frame_size > file_size) {
                printf("文件结尾，数据不完整\n");
                break;
            }
            
            // 解析并打印帧
            parse_and_print_0x35(frame);
            
            // 更新位置指针 (按最大帧大小前进)
            offset += sizeof(T_AJRSatellitePositionInfo_Telemetry);
            frame_count++;
        } else {
            offset++;
        }
    }
    
    // 统计结果
    if (frame_count == 0) {
        printf("未找到有效数据帧\n");
    } else {
        printf("\n处理完成! 共发现 %d 个有效数据帧\n", frame_count);
    }
    
    // 清理资源
    free(buffer);
    return 0;
}