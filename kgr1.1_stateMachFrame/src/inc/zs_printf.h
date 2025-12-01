/******************************************************************************************
* 文件 :       zs_printf.h
* 功能描述 : 格式化打印
* 作者 :       huanglixiong
* 修改记录 :
*    2019/12/25 created by huanglixiong
*
******************************************************************************************/


#ifndef __ZS_PRINTF_H__
#define __ZS_PRINTF_H__

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"{
#endif

int zs_sprintf(char *buf, const char *fmt, ...);

int zs_snprintf(char *buf, size_t size, const char *fmt, ...);

int zs_vsnprintf(char *buf, size_t size, const char *fmt, va_list va);

void dx_kprintf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
