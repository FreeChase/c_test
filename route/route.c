#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_BUFFER_SIZE 1024
#define HEADER_1 0x55
#define HEADER_2 0xAA

// 串口设备结构体
typedef struct {
    HANDLE handle;      // 串口句柄
    char port_name[16]; // 串口名称(如"COM1")
    int baud_rate;      // 波特率
    BOOL is_open;       // 是否已打开
} SerialPort;

// 协议路由器结构体
typedef struct {
    SerialPort* ports[256];     // 类型到串口的映射(0-255)
    SerialPort* default_port;   // 默认串口
    unsigned char buffer[MAX_BUFFER_SIZE];
    size_t buf_len;             // 当前缓冲区长度
} ProtocolRouter;

// 打开串口
BOOL open_serial_port(SerialPort* port) {
    if (port->is_open) return TRUE;

    // 格式化串口名称
    char full_port_name[16];
    snprintf(full_port_name, sizeof(full_port_name), "\\\\.\\%s", port->port_name);

    // 打开串口
    port->handle = CreateFileA(
        full_port_name,
        GENERIC_READ | GENERIC_WRITE,
        0,    // 独占访问
        NULL,  // 安全属性
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (port->handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error opening serial port %s (Error %lu)\n", port->port_name, GetLastError());
        return FALSE;
    }

    // 配置串口参数
    DCB dcb = {0};
    dcb.DCBlength = sizeof(DCB);
    
    if (!GetCommState(port->handle, &dcb)) {
        fprintf(stderr, "Error getting serial port state (Error %lu)\n", GetLastError());
        CloseHandle(port->handle);
        return FALSE;
    }

    // 设置串口参数
    dcb.BaudRate = port->baud_rate; // 波特率
    dcb.ByteSize = 8;              // 数据位
    dcb.Parity = NOPARITY;         // 无奇偶校验
    dcb.StopBits = ONESTOPBIT;     // 1位停止位
    dcb.fBinary = TRUE;            // 二进制模式
    dcb.fOutxCtsFlow = FALSE;      // 无CTS流控
    dcb.fOutxDsrFlow = FALSE;      // 无DSR流控
    dcb.fDtrControl = DTR_CONTROL_ENABLE; // 启用DTR
    dcb.fRtsControl = RTS_CONTROL_ENABLE; // 启用RTS

    if (!SetCommState(port->handle, &dcb)) {
        fprintf(stderr, "Error setting serial port state (Error %lu)\n", GetLastError());
        CloseHandle(port->handle);
        return FALSE;
    }

    // 设置超时
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = MAXDWORD; // 非阻塞读取
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 50; // 50ms写超时
    timeouts.WriteTotalTimeoutMultiplier = 10;
    
    if (!SetCommTimeouts(port->handle, &timeouts)) {
        fprintf(stderr, "Error setting serial timeouts (Error %lu)\n", GetLastError());
        CloseHandle(port->handle);
        return FALSE;
    }

    // 清空缓冲区
    PurgeComm(port->handle, PURGE_RXCLEAR | PURGE_TXCLEAR);

    port->is_open = TRUE;
    return TRUE;
}

// 初始化协议路由器
ProtocolRouter* router_init() {
    ProtocolRouter* router = malloc(sizeof(ProtocolRouter));
    if (!router) return NULL;
    
    memset(router, 0, sizeof(ProtocolRouter));
    router->buf_len = 0;
    
    // 初始化所有端口映射为NULL
    for (int i = 0; i < 256; i++) {
        router->ports[i] = NULL;
    }
    
    return router;
}

// 添加类型到串口的映射
void router_add_mapping(ProtocolRouter* router, unsigned char type, SerialPort* port) {
    if (type < 256) {
        router->ports[type] = port;
    }
}

// 设置默认串口
void router_set_default(ProtocolRouter* router, SerialPort* port) {
    router->default_port = port;
}

// 处理接收到的数据
void process_data(ProtocolRouter* router, const unsigned char* data, size_t len) {
    // 确保缓冲区不会溢出
    if (router->buf_len + len > MAX_BUFFER_SIZE) {
        // 缓冲区溢出 - 清空缓冲区
        router->buf_len = 0;
    }
    
    // 将新数据添加到缓冲区
    memcpy(router->buffer + router->buf_len, data, len);
    router->buf_len += len;
    
    // 处理缓冲区中的所有完整帧
    while (router->buf_len >= 5) { // 最小帧长度
        // 查找帧头
        size_t start_idx = 0;
        BOOL found = FALSE;
        
        // 查找帧头 (0x55 0xAA)
        for (; start_idx <= router->buf_len - 2; start_idx++) {
            if (router->buffer[start_idx] == HEADER_1 && 
                router->buffer[start_idx + 1] == HEADER_2) {
                found = TRUE;
                break;
            }
        }
        
        if (!found) {
            // 没有找到有效帧头，清空缓冲区
            router->buf_len = 0;
            return;
        }
        
        // 检查长度是否足够解析数据长度字段
        if (router->buf_len < start_idx + 3) {
            // 数据不足，等待更多数据
            // 移动有效数据到缓冲区开头
            memmove(router->buffer, router->buffer + start_idx, router->buf_len - start_idx);
            router->buf_len -= start_idx;
            return;
        }
        
        // 获取数据长度 (第3个字节)
        size_t data_len = router->buffer[start_idx + 2];
        
        // 计算完整帧长度 = 帧头(2) + 长度(1) + 类型前字节(1) + 类型(1) + 数据
        size_t total_frame_len = 5 + data_len;
        
        // 检查是否收到完整帧
        if (router->buf_len < start_idx + total_frame_len) {
            // 数据不足，等待更多数据
            // 移动有效数据到缓冲区开头
            memmove(router->buffer, router->buffer + start_idx, router->buf_len - start_idx);
            router->buf_len -= start_idx;
            return;
        }
        
        // 提取帧类型 (第5个字节)
        unsigned char frame_type = router->buffer[start_idx + 4];
        
        // 获取目标串口
        SerialPort* target_port = router->ports[frame_type];
        if (!target_port) {
            target_port = router->default_port;
        }
        
        // 发送帧到目标串口
        if (target_port && target_port->is_open) {
            DWORD bytes_written;
            BOOL result = WriteFile(
                target_port->handle,
                router->buffer + start_idx,
                total_frame_len,
                &bytes_written,
                NULL
            );
            
            if (!result) {
                fprintf(stderr, "Error writing to serial port %s (Error %lu)\n", 
                        target_port->port_name, GetLastError());
            }
        }
        
        // 从缓冲区移除已处理的帧
        size_t remaining = router->buf_len - (start_idx + total_frame_len);
        if (remaining > 0) {
            memmove(router->buffer, 
                    router->buffer + start_idx + total_frame_len, 
                    remaining);
            router->buf_len = remaining;
        } else {
            router->buf_len = 0;
        }
    }
}

// 主函数
int main() {
    // 创建并配置串口
    SerialPort uart0 = {.port_name = "COM3", .baud_rate = CBR_115200, .is_open = FALSE};
    SerialPort uart1 = {.port_name = "COM4", .baud_rate = CBR_9600, .is_open = FALSE};
    SerialPort default_uart = {.port_name = "COM1", .baud_rate = CBR_57600, .is_open = FALSE};
    
    // 打开串口
    if (!open_serial_port(&uart0)) return EXIT_FAILURE;
    if (!open_serial_port(&uart1)) return EXIT_FAILURE;
    if (!open_serial_port(&default_uart)) return EXIT_FAILURE;
    
    printf("Serial ports opened successfully\n");
    
    // 初始化路由器
    ProtocolRouter* router = router_init();
    if (!router) {
        fprintf(stderr, "Failed to initialize router\n");
        return EXIT_FAILURE;
    }
    
    // 配置路由映射
    router_add_mapping(router, 0x37, &uart0);  // 类型0x37 -> COM3
    router_add_mapping(router, 0x38, &uart1);  // 类型0x38 -> COM4
    router_set_default(router, &default_uart); // 默认串口
    
    printf("Router mappings configured\n");
    
    // 模拟接收数据
    unsigned char test_frame1[] = {0x55, 0xAA, 0x04, 0x00, 0x37, 0x01, 0x02, 0x03, 0x04};
    unsigned char test_frame2[] = {0x55, 0xAA, 0x02, 0x00, 0x38, 0xAA, 0xBB};
    unsigned char test_frame3[] = {0x55, 0xAA, 0x06, 0x00, 0x39, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    
    printf("Processing test frames...\n");
    
    // 处理测试帧
    process_data(router, test_frame1, sizeof(test_frame1));
    printf("Frame 1 processed (type 0x37 -> COM3)\n");
    
    process_data(router, test_frame2, sizeof(test_frame2));
    printf("Frame 2 processed (type 0x38 -> COM4)\n");
    
    process_data(router, test_frame3, sizeof(test_frame3));
    printf("Frame 3 processed (type 0x39 -> default COM1)\n");
    
    // 实际应用中的主循环示例
    printf("Starting main loop (press Ctrl+C to exit)...\n");
    
    while (1) {
        // 在实际应用中，这里应该从输入设备读取数据
        // 例如:
        //   unsigned char input_buffer[256];
        //   DWORD bytes_read;
        //   if (ReadFile(input_handle, input_buffer, sizeof(input_buffer), &bytes_read, NULL)) {
        //       if (bytes_read > 0) {
        //           process_data(router, input_buffer, bytes_read);
        //       }
        //   }
        
        // 简单延迟以避免CPU占用过高
        Sleep(100);
    }
    
    // 清理资源
    free(router);
    CloseHandle(uart0.handle);
    CloseHandle(uart1.handle);
    CloseHandle(default_uart.handle);
    
    return EXIT_SUCCESS;
}