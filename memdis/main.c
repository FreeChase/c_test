#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>


typedef struct {
    int width;          // 字段宽度
    bool left_align;    // 左对齐标志
    char pad_char;      // 填充字符
    bool is_uppercase;  // 大写标志
    int min_digits;     // 最小位数
    int precision;      // 浮点数精度
} format_info;

// 辅助函数：向缓冲区写入字符
static char print_buffer[32];
static int buffer_pos = 0;

void my_putchar(char c) {
    // 实际实现时替换为您的输出函数
    putchar(c);
}

void my_puts(const char *s) {
    while (*s) {
        my_putchar(*s++);
    }
}

// 辅助函数：获取整数部分的位数
int get_num_width(uint64_t num, int base) {
    int width = 0;
    do {
        width++;
        num /= base;
    } while (num > 0);
    return width;
}

// 辅助函数：处理浮点数
void print_float(double num, int precision) {
    // 处理负数
    if (num < 0) {
        my_putchar('-');
        num = -num;
    }
    
    // 处理整数部分
    uint64_t int_part = (uint64_t)num;
    double frac_part = num - int_part;
    
    // 打印整数部分
    if (int_part == 0) {
        my_putchar('0');
    } else {
        char int_buffer[20];
        int i = 0;
        while (int_part > 0) {
            int_buffer[i++] = '0' + (int_part % 10);
            int_part /= 10;
        }
        while (i > 0) {
            my_putchar(int_buffer[--i]);
        }
    }
    
    // 如果指定了精度，打印小数部分
    if (precision > 0) {
        my_putchar('.');
        
        // 处理小数部分
        while (precision > 0) {
            frac_part *= 10;
            int digit = (int)frac_part;
            my_putchar('0' + digit);
            frac_part -= digit;
            precision--;
        }
    }
}

// 辅助函数：解析格式化信息
format_info parse_format(const char **format) {
    format_info info = {0, false, ' ', false, 0, 6};  // 默认精度为6
    
    // 检查左对齐标志
    if (**format == '-') {
        info.left_align = true;
        (*format)++;
    }
    
    // 检查填充字符
    if (**format == '0') {
        info.pad_char = '0';
        (*format)++;
    }
    
    // 解析宽度
    while (**format >= '0' && **format <= '9') {
        info.width = info.width * 10 + (**format - '0');
        (*format)++;
    }
    
    // 解析精度
    if (**format == '.') {
        (*format)++;
        info.precision = 0;
        while (**format >= '0' && **format <= '9') {
            info.precision = info.precision * 10 + (**format - '0');
            (*format)++;
        }
    }
    
    return info;
}

// 辅助函数：填充字符
void pad_space(char pad_char, int count) {
    while (count-- > 0) {
        my_putchar(pad_char);
    }
}

// 辅助函数：输出十六进制数
void print_hex(uint64_t num, int min_digits, bool uppercase) {
    char hex[16];
    int i = 0;
    
    if (num == 0) {
        int zeros = min_digits ? min_digits : 1;
        while (zeros--) my_putchar('0');
        return;
    }
    
    while (num) {
        int digit = num & 0xF;
        hex[i++] = digit < 10 ? '0' + digit : 
                   (uppercase ? 'A' : 'a') + (digit - 10);
        num >>= 4;
    }
    
    while (i < min_digits) hex[i++] = '0';
    
    while (i--) my_putchar(hex[i]);
}

void xprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    while (*format) {
        if (*format == '%') {
            format++; // 跳过%
            
            // 解析格式化信息
            format_info info = parse_format(&format);
            
            switch (*format) {
                case 'f': {
                    double num = va_arg(args, double);
                    // 计算整个数字的长度（包括小数点和小数部分）
                    char temp_buf[32];
                    int num_len = snprintf(temp_buf, sizeof(temp_buf), "%.*f", 
                                         info.precision, num);
                    
                    if (!info.left_align) {
                        pad_space(info.pad_char, info.width - num_len);
                    }
                    print_float(num, info.precision);
                    if (info.left_align) {
                        pad_space(info.pad_char, info.width - num_len);
                    }
                    break;
                }
                
                case 's': {
                    char *str = va_arg(args, char *);
                    int len = 0;
                    char *p = str;
                    while (*p++) len++;
                    
                    if (!info.left_align) {
                        pad_space(info.pad_char, info.width - len);
                    }
                    my_puts(str);
                    if (info.left_align) {
                        pad_space(info.pad_char, info.width - len);
                    }
                    break;
                }
                
                case 'p': {
                    void *ptr = va_arg(args, void *);
                    my_putchar('0');
                    my_putchar('x');
                    print_hex((uint64_t)ptr, sizeof(void*) * 2, true);
                    break;
                }
                
                case 'X':
                    info.is_uppercase = true;
                    // fall through
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);
                    if (info.pad_char == '0' && info.width > 0) {
                        print_hex(num, info.width, info.is_uppercase);
                    } else {
                        int num_width = get_num_width(num, 16);
                        if (!info.left_align && info.width > num_width) {
                            pad_space(info.pad_char, info.width - num_width);
                        }
                        print_hex(num, 0, info.is_uppercase);
                        if (info.left_align && info.width > num_width) {
                            pad_space(info.pad_char, info.width - num_width);
                        }
                    }
                    break;
                }
                
                case 'c': {
                    char c = (char)va_arg(args, int);
                    my_putchar(c);
                    break;
                }
                
                case 'l': {
                    format++;
                    if (*format == 'l' && *(format + 1) == 'X') {
                        format++;
                        uint64_t num = va_arg(args, uint64_t);
                        print_hex(num, 16, true);
                    }
                    break;
                }
                
                default:
                    my_putchar('%');
                    my_putchar(*format);
                    break;
            }
        } else {
            my_putchar(*format);
        }
        format++;
    }
    
    va_end(args);
}

void memdisplay(void *start_addr, size_t length, size_t unit_size) {
    if (unit_size != 1 && unit_size != 2 && unit_size != 4 && unit_size != 8) {
        xprintf("Invalid unit_size. Please use 1, 2, 4, or 8.\n");
        return;
    }

    size_t bytes_per_line = 16;
    uint8_t *mem = (uint8_t *)start_addr;

    // 打印列头
    xprintf("%-10s", "Address");
    for (size_t i = 0; i < bytes_per_line; ++i) {
        xprintf("0x%X ", i);
    }
    xprintf("  ASCII\n");

    // 打印横线
    xprintf("------------------");
    for (size_t i = 0; i < bytes_per_line; ++i) {
        xprintf("-----");
    }
    xprintf("\n");

    size_t addr = 0;
    while (addr < length) {
        // 打印行头地址
        xprintf("%p  ", (void *)(mem + addr));

        // 打印每行的内容
        for (size_t i = 0; i < bytes_per_line && addr < length; i += unit_size) {
            switch (unit_size) {
                case 1:
                    xprintf("%02X  ", mem[addr + i]);
                    break;
                case 2:
                    xprintf("%04X    ", *((uint16_t *)(mem + addr + i)));
                    break;
                case 4:
                    xprintf("%08X        ", *((uint32_t *)(mem + addr + i)));
                    break;
                case 8:
                    xprintf("%016llX   ", *((uint64_t *)(mem + addr + i)));
                    break;
                default:
                    // 不太可能到达这里，因为上面已经检查了 unit_size 的合法性
                    break;
            }
        }

        // 打印ASCII码
        xprintf("  ");
        for (size_t i = 0; i < bytes_per_line && addr < length; ++i) {
            char c = (mem[addr + i] >= 32 && mem[addr + i] <= 126) ? mem[addr + i] : '.';
            xprintf("%c", c);
        }

        xprintf("\n");
        addr += bytes_per_line;
    }
}

int main() {
    int arr[256];
    char ch[256];
    size_t arr_length = sizeof(arr) / sizeof(arr[0]);

    for (size_t i = 0; i < 256; i++) {
        arr[i] = i;
        ch[i] = i;
    }

    memdisplay(ch, 256, 1);
    memdisplay(ch, 256, 2);
    memdisplay(ch, 256, 4);

    return 0;
}
