/**
@author   XiangWei 2005/3/30  11:36
@email    waterknows@gmail.com
@file     icetype.h
公用类型声明
*********************************************************************/

/*
modification history
--------------------
$Log: icetype.h,v $
Revision 1.4  2005/08/08 02:47:33  xb
加入网络

Revision 1.3.26.1  2005/08/05 09:29:07  lzj
协议理解又错误，需要重新对频点的窄带干扰的显示改动

Revision 1.3  2005/07/08 02:04:26  xb
6-24分支已合并

Revision 1.2.10.1  2005/06/28 16:23:42  xw
改正了帧提取时没有校验和的错误, 加上了帧类型判断函数.

*/
#ifndef ICETYPE_H_XW_2005_01_01_1
#define ICETYPE_H_XW_2005_01_01_1
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "dxdef.h"

#ifdef __cplusplus
extern "C"{
#endif

#define FUNC(x) 1

typedef	u8 *			P_U8;		/**<  unsigned 8 bit data  */
typedef	u16 *			P_U16;		/**<  unsigned 16 bit data */
typedef	u32 *			P_U32;		/**<  unsigned 32 bit data */
typedef	s8 *			P_S8;		/**<  signed 8 bit data  */
typedef	s16 *			P_S16;		/**<  signed 16 bit data */
typedef	s32 *			P_S32;		/**<  signed 32 bit data */
typedef u64 *			P_U64;
typedef s64 *			P_S64;

typedef char			S8_CHAR;

#define NULL_U32        0xFFFFFFFF
#define NULL_PTR        (void *)0

#define CCMM_OK         0
#define CCMM_LAST_ONE   1
#define CCMM_ERROR      2
#define CCMM_ERROR_NULL_PTR   3

#if FUNC("错误码定义")
typedef enum enNavModule
{
    NAV_MODULE_OK    = 0, // 该模块定义保留
    NAV_MODULE_PUB   = 1,
    NAV_MODULE_CCMM  = 2,
    NAV_MODULE_PIC   = 3,
    NAV_MODULE_TRK   = 4,
    NAV_MODULE_TICK  = 5,

    NAV_SYSTEM_BUTT
} NAV_MODULE_E;

/* 错误码宏定义如下:module(16bit)+errorCode(16bit) */
#define ERROR_CODE(module, errorCode) (((module)<<16)|(errorCode))

/* 成功对应的错误码为0x00000000 */
#define NAV_OK ERROR_CODE(NAV_MODULE_OK, 0)
#endif

#define _PACK_STRUCT

#define		ERR_NO_ERROR		1	/**< No error				*/
#define		ERR_NORAML_ERROR	-1	/**< Normal error			*/
#define		ERR_BAD_ARGS		-2	/**< Bad arguments			*/
#define		ERR_MEM_NONE		-4	/**< Memeroy is not enough  */
#define		ERR_FILE_ERROR		-3	/**< File error				*/
#define		ERR_ARR_OVERFLOW	-5	/**< Array bound overflow	*/
#define		ERR_TIME_OUT		-6	/**< Communication time out	*/
#ifdef MODMERGE
#define RNRAWSL
#endif

#ifdef __cplusplus
}
#endif




#endif/* ICETYPE_H_XW_2005_01_01_1*/
