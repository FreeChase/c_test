/******************************************************************************
 * Copyright (C) 2024 ZS Development Team, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @file dxdef.h
 * @brief
 * @version 0.1
 * Change Logs:
 * Date           Author       Notes
 * 2024-12-05     Andy      first version
******************************************************************************/

#ifndef __DXDEF_H__
#define __DXDEF_H__

#include <stddef.h>
#include <stdlib.h>
#ifndef _WIN32
#include "xil_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup 基础定义
 */

/**@{*/
/* Hardware version information */
#define HW_KGR_B3_VER_7_KD              0x0
#define HW_KGR_B3_VER_4_KD              0x7

#define HW_KGR_B3_VER_4_54              0x1
#define HW_KGR_S_VER_4_54               0x2
#define HW_KGR_B1_VER_4_54              0x3

//#define HW_KGR_B3_VER                   0x0
//#define HW_KGR_B1_VER                   0x1
//#define HW_KGR_S_VER                   	0x2

/* DX version information */
#define DX_VERSION                      1L              /**< major version number */
#define DX_SUBVERSION                   0L              /**< minor version number */
#define DX_REVISION                     0L              /**< revise version number */
#define DX_NAME_MAX						16
/* DX OS version */
#define DXOS_VERSION                ((DX_VERSION * 10000) + \
                                         (DX_SUBVERSION * 100) + DX_REVISION)

#define CTRL_HIGH						1
#define CTRL_LOW						0

#define dx_malloc   malloc
#define dx_free     free
/* dxos basic data type definitions */
#ifndef DX_USING_ARCH_DATA_TYPE

#ifdef _WIN32
typedef char                   s8;      /**<  8bit integer type */
typedef short                  s16;     /**< 16bit integer type */
typedef int                    s32;     /**< 32bit integer type */
typedef unsigned char                   u8;     /**<  8bit unsigned integer type */
typedef unsigned short                  u16;    /**< 16bit unsigned integer type */
typedef unsigned int                    u32;    /**< 32bit unsigned integer type */
typedef long long                s64;     /**< 64bit integer type */
typedef unsigned long long              u64;    /**< 64bit unsigned integer type */
#endif

#ifdef ARCH_CPU_64BIT
//typedef long long                s64;     /**< 64bit integer type */
//typedef unsigned long long              u64;    /**< 64bit unsigned integer type */
#else
//typedef long long                s64;     /**< 64bit integer type */
//typedef unsigned long long              u64;    /**< 64bit unsigned integer type */
#endif
#endif

typedef	float			                f32;		/**<  single-precision floating point number */
typedef	double			                f64;		/**<  double-precision floating point number */

typedef	float			                f32;		/**<  single-precision floating point number */

typedef int                             bool_t;      /**< boolean type */
typedef long long                       base_t;      /**< Nbit CPU related date type */
typedef unsigned long long              ubase_t;     /**< Nbit unsigned CPU related data type */
//#define	err_t							base_t
//typedef u32							 err_t;
//typedef u64                          base_t;       /**< Type for error number */
//#define base_t                       u64       /**< Type for error number */
typedef u32                            tick_t;      /**< Type for tick count */
typedef s8                             err_t;      /**< Type for tick count */

#define BITS_PER_LONG                   (sizeof(long)*8)


typedef u32  U32;
typedef s32  S32;
typedef s8   S8;
typedef u8   U8;
typedef u16   U16;
typedef int  BOOL;      /**< boolean type */


#define DX_NULL                         (0)
#define DX_PORTINVALID                  (65535)             

/* boolean type definitions */
#define DX_TRUE                         1u               /**< boolean true  */
#define DX_FALSE                        0u               /**< boolean fails */

#define  DX_NO                         0u
#define  DX_YES                        1u

#define  DX_DISABLED                  0u
#define  DX_ENABLED                   1u


typedef  void*                          dx_handle;      /**< 对象句柄 */
extern u8 g_HwModuleVer;               /**< 全局变量，当前硬件模块版本*/

#define		ERR_NO_ERROR		1	/**< No error				*/
#define		ERR_NORAML_ERROR	-1	/**< Normal error			*/
#define		ERR_BAD_ARGS		-2	/**< Bad arguments			*/
#define		ERR_MEM_NONE		-4	/**< Memeroy is not enough  */
#define		ERR_FILE_ERROR		-3	/**< File error				*/
#define		ERR_ARR_OVERFLOW	-5	/**< Array bound overflow	*/
#define		ERR_TIME_OUT		-6	/**< Communication time out	*/
/**@}*/

#define NULL_U32        0xFFFFFFFF
#define NULL_PTR        (void *)0

#define CCMM_OK         0
#define CCMM_LAST_ONE   1
#define CCMM_ERROR      2
#define CCMM_ERROR_NULL_PTR   3

#define FUNC(x) 1

#if FUNC("错误码定义")

/* 错误码宏定义如下:module(16bit)+errorCode(16bit) */
#define ERROR_CODE(module, errorCode) (((module)<<16)|(errorCode))

/* 成功对应的错误码为0x00000000 */
#define NAV_OK ERROR_CODE(NAV_MODULE_OK, 0)
#endif
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define __CLANG_ARM
#endif

#if defined(__ARMCC_VERSION) && defined(__GNUC__)
#define __ARMCC_GNUC__
#endif

/* Compiler Related Definitions */
#if defined(__CC_ARM) || defined(__CLANG_ARM)           /* ARM Compiler */
    #include <stdarg.h>
    #define SECTION(x)                  __attribute__((section(x)))
    #define DX_UNUSED(x)                (void)(x)
    #define DX_USED                     __attribute__((used))
    #define ALIGN(n)                    __attribute__((aligned(n)))

    #define DX_WEAK                     __attribute__((weak))
    #define dx_inline                   static __inline

#elif defined (__GNUC__)                /* GNU GCC Compiler */
    #if defined(DX_USING_NEWLIB) || defined(DX_USING_MUSL)
        #include <stdarg.h>
    #else
        /* the version of GNU GCC must be greater than 4.x */
        typedef __builtin_va_list       __gnuc_va_list;
        typedef __gnuc_va_list          va_list;
        #define va_start(v,l)           __builtin_va_start(v,l)
        #define va_end(v)               __builtin_va_end(v)
        #define va_arg(v,l)             __builtin_va_arg(v,l)
    #endif

    #define SECTION(x)                  __attribute__((section(x)))
    #define DX_UNUSED(x)                (void)(x)
    #define DX_USED                     __attribute__((used))
    #define ALIGN(n)                    __attribute__((aligned(n)))
    #define DX_WEAK                     __attribute__((weak))
    #define dx_inline                   static __inline
#elif defined (_MSC_VER)
    #include <stdarg.h>
    #define SECTION(x)
    #define DX_UNUSED(x)                (void)(x)
    #define DX_USED
    #define ALIGN(n)                    __declspec(align(n))
    #define DX_WEAK
    #define dx_inline                   static __inline
#else
    #error not supported tool chain
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                GENERAL
*********************************************************************************************************
*/

#define  DX_OPT_NONE                           0x00u

/*
*********************************************************************************************************
*                                                UTILITIES
*********************************************************************************************************
*/
/**
 * @ingroup BasicDef
 *
 * @def DX_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. DX_ALIGN(13, 4)
 * would return 16.
 */
#define DX_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def DX_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. DX_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define DX_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def MATH_ROUND_INC_UP_PWR2(nbr, inc)
 * Round value up to the next (power of 2) increment.
 */
#define MATH_ROUND_INC_UP_PWR2(nbr, inc)                  (((nbr) & ~((inc) - 1)) + (((nbr) & ((inc) - 1)) == 0 ? 0 : (inc)))

/**
 * @ingroup BasicDef
 *
 * @def DIV_ROUND_UP(n, d)
 * 向上取整
 */
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
/**
 * @ingroup BasicDef
 *
 * @def BIT(nr)
 */
#define BIT(nr) (1UL << (nr))

/**
 * @ingroup BasicDef
 *
 * @def GENMASK(h, l)
 */
#define GENMASK(h, l)   \
        (((~0UL) - (1UL << (l)) + 1) & \
        (~0UL >> (BITS_PER_LONG - 1 -(h))))

/**
 * @ingroup BasicDef
 *
 * @def DX_MIN(a, b)
 */
#define DX_MIN(a, b)        (((a) < (b)) ? (a) : (b))

 /**
  * @ingroup BasicDef
  *
  * @def DX_MAX(a, b)
  */
#define DX_MAX(a, b)        (((a) < (b)) ? (b) : (a))
/**
 * @ingroup BasicDef
 *
 * @def DX_MIN3(x, y, z)
 */
#define DX_MIN3(x, y, z)    (DX_MIN(DX_MIN(x, y), z))

/**
 * @ingroup BasicDef
 *
 * @def dx_container_of - return the member address of ptr, if the type of ptr is the
 * struct type
 */
#define dx_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (ubase_t)(&((type *)0)->member)))


 /**
  * @brief 双链表
  *
  */
struct dx_list_node
{
    struct dx_list_node* next;                          /**< point to next node. */
    struct dx_list_node* prev;                          /**< point to prev node. */
};
typedef struct dx_list_node dx_list_t;                  /**< Type for lists. */

/**
 * @brief 单链表
 *
 */
struct dx_slist_node
{
    struct dx_slist_node* next;                         /**< point to next node. */
};
typedef struct dx_slist_node dx_slist_t;                /**< Type for single list. */


/**
 * The hook function call macro
 */
#ifdef DX_USING_HOOK
#define DX_OBJECT_HOOK_CALL(func, argv) \
    do { if ((func) != DX_NULL) func argv; } while (0)
#else
#define DX_OBJECT_HOOK_CALL(func, argv)
#endif

/*
*********************************************************************************************************
*                                        CREATE OPTS (LOCK ONLY)
*********************************************************************************************************
*/

#define  DX_OPT_CREATE_NONE           (u8)DX_OPT_NONE

#define  DX_OPT_CREATE_NON_REENTRANT           DX_OPT_NONE    /* Create non-re-entrant lock.                          */
#define  DX_OPT_CREATE_REENTRANT      (u8)0x01u      /* Create     re-entrant lock.                          */


/*
*********************************************************************************************************
*                                       PEND OPTS (LOCK, SEM, Q)
*********************************************************************************************************
*/

#define  DX_OPT_PEND_NONE             (u8)DX_OPT_NONE

#define  DX_OPT_PEND_BLOCKING                  DX_OPT_NONE    /* Block until rsrc is avail.                           */
#define  DX_OPT_PEND_NON_BLOCKING     (u8)0x01u      /* Don't block if rsrc is unavail.                      */


/*
*********************************************************************************************************
*                                       POST OPTS (LOCK, SEM, Q)
*********************************************************************************************************
*/

#define  DX_OPT_POST_NONE                      DX_OPT_NONE


/*
*********************************************************************************************************
*                                        ABORT OPTS (LOCK, SEM)
*********************************************************************************************************
*/

#define  DX_OPT_ABORT_NONE                     DX_OPT_NONE


/*
*********************************************************************************************************
*                                      DELETE OPTS (LOCK, SEM, Q)
*********************************************************************************************************
*/

#define  DX_OPT_DEL_NONE                       DX_OPT_NONE

/*
*********************************************************************************************************
*                                               TMR OPTS
*********************************************************************************************************
*/

#define  DX_OPT_TMR_NONE              (u8)DX_OPT_NONE

#define  DX_OPT_TMR_ONE_SHOT          DX_OPT_NONE            /* One-shot tmr,   callback called only once.           */
#define  DX_OPT_TMR_PERIODIC          (u8)0x01u      /* Periodic timer, callback called periodically.        */


/*
*********************************************************************************************************
*                                               DLY OPTS
*********************************************************************************************************
*/

#define  DX_OPT_DLY_NONE                       DX_OPT_NONE

#define  DX_OPT_DLY                            DX_OPT_NONE    /* 'Normal' delay.                                      */
#define  DX_OPT_DLY_PERIODIC                   0x01u      /* Periodic delay.                                      */


/*
*********************************************************************************************************
*                                            DX FEATURES
*********************************************************************************************************
*/

/**
 * @addtogroup 系统特征码
 */

/**@{*/
typedef enum
{
    DX_FEATURE_TASK_CREATE = 0u,                               /**< Task creation. */
    DX_FEATURE_TASK_DEL,                                       /**< Task del. */

    DX_FEATURE_LOCK_CREATE,                                    /**< Lock create, acquire and release.*/
    DX_FEATURE_LOCK_ACQUIRE,                                   /**< Lock pend. */
    DX_FEATURE_LOCK_RELEASE,                                   /**< Lock post. */
    DX_FEATURE_LOCK_DEL,                                       /**< Lock del. */

    DX_FEATURE_SEM_CREATE,                                     /**< Sem creation. */
    DX_FEATURE_SEM_PEND,                                       /**< Sem pend. */
    DX_FEATURE_SEM_POST,                                       /**< Sem post. */
    DX_FEATURE_SEM_ABORT,                                      /**< Sem pend abort. */
    DX_FEATURE_SEM_SET,                                        /**< Sem set cnt. */
    DX_FEATURE_SEM_DEL,                                        /**< Sem del. */

    DX_FEATURE_TMR,                                            /**< Tmr creation and exec. */

    DX_FEATURE_Q_CREATE,                                       /**< Q creation. */
    DX_FEATURE_Q_POST,                                         /**< Q post. */
    DX_FEATURE_Q_PEND,                                         /**< Q pend. */

    DX_FEATURE_DLY,                                            /**< Dly in both ms and ticks. */

    DX_FEATURE_TASK_REG,                                       /**< Task storage creation, get and set. */

    DX_FEATURE_TICK_GET                                        /**< Get OS tick val. */
} DX_FEATURE;
/**@}*/

/**
 * @addtogroup 全局错误码
 */

/**@{*/
typedef enum 
{
    DX_EOK = 0,                                                 /**< No err. */
    DX_ERROR,                                                   /**< Generic OS err. */
    DX_ETIMEOUT,                                                /**< Operation timed out. */
    DX_EFULL,                                                   /**< The resource is full */
    DX_EEMPTY,                                                  /**< The resource is empty */
    DX_ENOMEM,                                                  /**< No memory */
    DX_ENOSYS,                                                  /**< No system */
    DX_EBUSY,                                                   /**< Busy */
    DX_EIO,                                                     /**< IO error */
    DX_EINTR,                                                   /**< Interrupted system call */
    DX_ENULLPTR,                                                /**< Null ptr */
    DX_EINVAL,                                                  /**< Invalid argument */
    DX_ETRAP,                                                   /**< trap event */
    DX_EABORT,                                                  /**< Operation aborted */
    DX_EISR,                                                    /**< Illegal call from ISR */
    DX_ENOAVAIL,                                                /**< Feature not supported */
    DX_EOWNERSHIP,                                              /**< Ownership err */
    DX_EWOULDBLOCK,                                             /**< Non-blocking operation would block. */
    DX_EWOULDOVF,                                               /**< Item would overflow. */
    
} DX_ERR;
/**@}*/


#ifdef CONFIG_USING_CONSOLE
/* console输出类型定义 */
typedef enum {
    CONSOLE_NONE    = 0,    // 关闭所有信息输出
    CONSOLE_LOG     = 1,    // 开启log信息输出
    CONSOLE_PROTOL  = 2,    // 开启$协议类信息输出
    CONSOLE_OBS     = 4     // 开启0x8开头二进制观测量/电文输出
}CONSOLE_OUT_TYPE;
#endif


/**
 * @addtogroup 设备
 */

/**@{*/

/**
 * device (I/O) class type
 */
enum dx_device_class_type
{
    DX_Device_Class_Char = 0,                           /**< character device */
    DX_Device_Class_Block,                              /**< block device */
    DX_Device_Class_NetIf,                              /**< net interface */
    DX_Device_Class_MTD,                                /**< memory device */
    DX_Device_Class_RTC,                                /**< RTC device */
    DX_Device_Class_Sound,                              /**< Sound device */
    DX_Device_Class_Graphic,                            /**< Graphic device */
    DX_Device_Class_I2CBUS,                             /**< I2C bus device */
    DX_Device_Class_SPIBUS,                             /**< SPI bus device */
    DX_Device_Class_SPIDevice,                          /**< SPI device */
    DX_Device_Class_PM,                                 /**< PM pseudo device */
    DX_Device_Class_Pipe,                               /**< Pipe device */
    DX_Device_Class_Timer,                              /**< Timer device */
    DX_Device_Class_Miscellaneous,                      /**< Miscellaneous device */
    DX_Device_Class_Sensor,                             /**< Sensor device */
    DX_Device_Class_WDT,                                /**< WDT device */
    DX_Device_Class_PHY,                                /**< PHY device */
    DX_Device_Class_DAC,                                /**< DAC device */
    DX_Device_Class_RF,                                 /**< RF device */
    DX_Device_Class_BB,                                 /**< BB device */
    DX_Device_Class_IPI,                                /**< IPI device */
    DX_Device_Class_Unknown                             /**< unknown device */
};




/**
 * device flags defitions
 */
#define DX_DEVICE_FLAG_DEACTIVATE       0x000           /**< device is not not initialized */

#define DX_DEVICE_FLAG_RDONLY           0x001           /**< read only */
#define DX_DEVICE_FLAG_WRONLY           0x002           /**< write only */
#define DX_DEVICE_FLAG_RDWR             0x003           /**< read and write */

#define DX_DEVICE_FLAG_REMOVABLE        0x004           /**< removable device */
#define DX_DEVICE_FLAG_STANDALONE       0x008           /**< standalone device */
#define DX_DEVICE_FLAG_ACTIVATED        0x010           /**< device is activated */
#define DX_DEVICE_FLAG_SUSPENDED        0x020           /**< device is suspended */
#define DX_DEVICE_FLAG_STREAM           0x040           /**< stream mode */

#define DX_DEVICE_FLAG_INT_RX           0x100           /**< INT mode on Rx */
#define DX_DEVICE_FLAG_DMA_RX           0x200           /**< DMA mode on Rx */
#define DX_DEVICE_FLAG_INT_TX           0x400           /**< INT mode on Tx */
#define DX_DEVICE_FLAG_DMA_TX           0x800           /**< DMA mode on Tx */

#define DX_DEVICE_OFLAG_CLOSE           0x000           /**< device is closed */
#define DX_DEVICE_OFLAG_RDONLY          0x001           /**< read only access */
#define DX_DEVICE_OFLAG_WRONLY          0x002           /**< write only access */
#define DX_DEVICE_OFLAG_RDWR            0x003           /**< read and write */
#define DX_DEVICE_OFLAG_OPEN            0x008           /**< device is opened */
#define DX_DEVICE_OFLAG_MASK            0xf0f           /**< mask of open flag */

/**
 * general device commands
 */
#define DX_DEVICE_CTRL_RESUME           0x01            /**< resume device */
#define DX_DEVICE_CTRL_SUSPEND          0x02            /**< suspend device */
#define DX_DEVICE_CTRL_CONFIG           0x03            /**< configure device */
#define DX_DEVICE_CTRL_CLOSE            0x04            /**< close device */
#define DX_DEVICE_CTRL_NOTIFY_SET       0x05            /**< set notify func */
#define DX_DEVICE_CTRL_CONSOLE_OFLAG    0x06            /**< get console open flag */

#define DX_DEVICE_CTRL_SET_INT          0x10            /**< set interrupt */
#define DX_DEVICE_CTRL_CLR_INT          0x11            /**< clear interrupt */
#define DX_DEVICE_CTRL_GET_INT          0x12            /**< get interrupt status */

/**
 * special device commands
 */
#define DX_DEVICE_CTRL_CHAR_STREAM              0x10            /**< stream mode on char device */
#define DX_DEVICE_CTRL_BLK_GETGEOME             0x10            /**< get geometry information   */
#define DX_DEVICE_CTRL_BLK_SYNC                 0x11            /**< flush data to block device */
#define DX_DEVICE_CTRL_BLK_ERASE                0x12            /**< erase block on block device */
#define DX_DEVICE_CTRL_BLK_AUTOREFRESH          0x13            /**< block device : enter/exit auto refresh mode */
#define DX_DEVICE_CTRL_NETIF_GETMAC             0x10            /**< get mac address */
#define DX_DEVICE_CTRL_MTD_FORMAT               0x10            /**< format a MTD device */
#define DX_DEVICE_CTRL_WDG_ENABLE               0x10            /**< enable wdg */
#define DX_DEVICE_CTRL_WDG_DISABLE              0x11            /**< disable wdg */
#define DX_DEVICE_CTRL_WDG_WDI_INVERT           0x12            /**< invert wdi */
#define DX_DEVICE_CTRL_WDG_SET_TIME             0x13            /**< set wdg reset time */
#define DX_DEVICE_CTRL_RTC_GET_TIME             0x10            /**< get time */
#define DX_DEVICE_CTRL_RTC_SET_TIME             0x11            /**< set time */
#define DX_DEVICE_CTRL_RTC_GET_ALARM            0x12            /**< get alarm */
#define DX_DEVICE_CTRL_RTC_SET_ALARM            0x13            /**< set alarm */
#define DX_DEVICE_CTRL_RTC_RESET                0x14            /**< reset */
#define DX_DEVICE_CTRL_RTC_INT                  0x15            /**< 1s interrupt callback */
#define DX_DEVICE_CTRL_RTC_GET_VBAT_VOL         0x16            /**< get Vbat voltage */
#define DX_DEVICE_CTRL_DAC_CONFIG               0x10            /**< set dac value */
#define DX_DEVICE_CTRL_TMP_GET                  0x10            /**< read tmp value */
#define DX_DEVICE_CTRL_SENSOR_GET_CURRENT       0x10    /**< read current value */
#define DX_DEVICE_CTRL_SENSOR_GET_VOLTAGE       0x11
#define DX_DEVICE_CTRL_GYRO_GET_DATA            0x10    /**< get current Gyroscope Data */
#define DX_DEVICE_CTRL_GYRO_GET_CHIPID          0x11    /**< get current Gyroscope Chip ID */
//#define DX_DEVICE_CTRL_GYRO_GET_ACCELERDATA     0x10    /**< get current Accelerometer value */
//#define DX_DEVICE_CTRL_GYRO_GET_GYRODATA        0x11    /**< get current Gyroscope value */
//#define DX_DEVICE_CTRL_GYRO_GET_MAGNETDATA      0x12    /**< get current Magnetometer value */
 /**
  * Device structure
  */
typedef struct dx_device* dx_device_t;

struct dx_device
{
    dx_list_t                 list;                     /**< list node of device */
    char                      name[DX_NAME_MAX];        /**< name of device */
    enum dx_device_class_type type;                     /**< device type */
    u16               flag;                     /**< device flag */
    u16               open_flag;                /**< device open flag */

    u8                ref_count;                /**< reference count */

    /* device call back */
    err_t(*rx_indicate)(dx_handle dev, size_t size);
    err_t(*tx_complete)(dx_handle dev, void* buffer);

    /* common device interface */
    err_t(*init)   (dx_device_t dev);
    err_t(*open)   (dx_device_t dev, u16 oflag);
    err_t(*close)  (dx_device_t dev);
    size_t(*read)   (dx_device_t dev, base_t pos, void* buffer, size_t size);
    size_t(*write)  (dx_device_t dev, base_t pos, const void* buffer, size_t size);
    err_t(*control)(dx_device_t dev, int cmd, void* args);
    err_t(*changeWorkMode) (dx_device_t dev);
    void* user_data;                /**< device private data */
};

/**@}*/


/**
 * @addtogroup OS
 */

/**@{*/
typedef  struct  _dx_lock_ext_cfg {                             /* ------------------- LOCK EXT CFG ------------------- */
    u8      Opt;                                           /* Opt passed to LockCreate() funct.                    */
} dx_lock_ext_cfg;

typedef  struct  _dx_q_ext_cfg {                                /* --------------------- Q EXT CFG -------------------- */
    u32   Rsvd;                                          /* Rsvd for future use.                                 */
} dx_q_ext_cfg;

typedef  struct  _dx_tmr_ext_cfg {                              /* -------------------- TMR EXT CFG ------------------- */
    u8      Opt;                                           /* Opt passed to TmrCreate() funct.                     */
} dx_tmr_ext_cfg;

typedef  struct  _dx_task_reg_ext_cfg {                         /* ----------------- TASK REG EXT CFG ----------------- */
    u32   Rsvd;                                          /* Rsvd for future use.                                 */
} dx_task_reg_ext_cfg;
/**@}*/

typedef	u8 *			P_U8;		/**<  unsigned 8 bit data  */
typedef	u16 *			P_U16;		/**<  unsigned 16 bit data */
typedef	u32 *			P_U32;		/**<  unsigned 32 bit data */
typedef	s8 *			P_S8;		/**<  signed 8 bit data  */
typedef	s16 *			P_S16;		/**<  signed 16 bit data */
typedef	s32 *			P_S32;		/**<  signed 32 bit data */
typedef u64 *			P_U64;
typedef s64 *			P_S64;

typedef char			S8_CHAR;

void dx_dly_us(s32 us);


#ifdef __cplusplus
}
#endif

#endif
