#include <stdio.h>
#include <stdarg.h>

void my_putchar(char c) {
    putchar(c);
}

void my_puts(const char *str) {
    while (*str) {
        my_putchar(*str);
        str++;
    }
}

void my_putint(int num) {
    if (num == 0) {
        my_putchar('0');
        return;
    }

    if (num < 0) {
        my_putchar('-');
        num = -num;
    }

    char buffer[20];
    int i = 0;

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0) {
        my_putchar(buffer[--i]);
    }
}

void my_putfloat(double num) {
    // 实现浮点数的输出需要更复杂的逻辑
    // 这里简化为输出小数点后两位
    int integerPart = (int)num;
    double fractionalPart = num - integerPart;

    my_putint(integerPart);
    my_putchar('.');
    my_putint((int)(fractionalPart * 100));
}

void my_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++; // Move past '%'
            switch (*format) {
                case 'd':
                    my_putint(va_arg(args, int));
                    break;
                case 's':
                    my_puts(va_arg(args, char *));
                    break;
                case 'f':
                    my_putfloat(va_arg(args, double));
                    break;
                case 'x':
                    my_putint(va_arg(args, int));
                    break;
                case 'c':
                    my_putchar(va_arg(args, int));
                    break;
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

int main() {
    int num = 42;
    char *str = "Hello, World!";
    double pi = 3.14159265;
    int hex_num = 0x1A;
    char ch = 'A';
    
    my_printf("This is a number: %d\n", num);
    my_printf("This is a string: %s\n", str);
    my_printf("This is a float: %f\n", pi);
    my_printf("This is a hex number: %x\n", hex_num);
    my_printf("This is a character: %c\n", ch);
    
    return 0;
}
