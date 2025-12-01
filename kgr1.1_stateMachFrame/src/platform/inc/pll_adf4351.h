/*
 ******************************************************************************
 * @file    pll_adf4351.h
 * 
 * @brief   This file contains all the functions prototypes for the
 *          pll_adf4351.c driver.
 ******************************************************************************
 * 
 * Copyright (c) 2024, ZS Development Team
 *  
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-20     Andy     the first version
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLL_ADF4351_H__
#define __PLL_ADF4351_H__

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "dxdef.h"
#include "sys_ctrl.h"

/** @defgroup ADF4351 Infos
  * @{
  *
  */
#define ADF4351_R0_ADDR_BASE	                (0)
#define ADF4351_R1_ADDR_BASE	                (1)
#define ADF4351_R2_ADDR_BASE	                (2)
#define ADF4351_R3_ADDR_BASE	                (3)
#define ADF4351_R4_ADDR_BASE	                (4)
#define ADF4351_R5_ADDR_BASE	                (5)

/**
  * @}
  *
  */
typedef union
{
    u32 r0_data;
    struct
    {
        u32 Control     : 3;     //  Control Bits(0 - 2)
        u32 Fractional  : 12;    //  12-Bit Fractional Value ((FRAC) 3 - 14)
        u32 Integer     : 16;    //  16-Bit Integer Value ((INT)15 - 30)
        u32 Resv        : 1;     //  Reserved(31)
    };
} adf4351_r0_t;

typedef union
{
    u32 r1_data;
    struct
    {
        u32 Control         : 3;     //  Control Bits(0 - 2)
        u32 Modulus         : 12;    //  12-Bit Modulus Value (3 - 14)
        u32 Phase           : 12;    //  12-Bit Phase Value (15 - 26)
        u32 Prescaler       : 1;     //  Prescaler Value (27)
        u32 Phase_Adjust    : 1;     //  Phase adjust bit (28)
        u32 Resv            : 3;     //  Reserved(29 - 31)
    };
} adf4351_r1_t;

typedef union
{
    u32 r2_data;
    struct
    {
        u32 Control             : 3;     //  Control Bits(0 - 2)
        u32 Counter_Reset       : 1;     //  Counter Reset (3)
        u32 Three_State         : 1;     //  Charge Pump Three-State (4)
        u32 PD                  : 1;     //  Power-Down (PD(5)) 
        u32 Phase_Detector      : 1;     //  Phase Detector Polarity (6)
        u32 LDP                 : 1;     //  Lock Detect Precision (LDP(7))
        u32 LDF                 : 1;     //  Lock Detect Function (LDF(8))
        u32 C_Pump              : 4;     //  Charge Pump Current Setting (9 - 12)
        u32 D_Buffer            : 1;     //  Double Buffer (13)
        u32 R_Counter           : 10;    //  10-Bit R Counter (14 - 23)
        u32 Rdiv2               : 1;     //  RDIV2 (24)
        u32 Ref_Doubler         : 1;     //  Reference Doubler (25)
        u32 MuxOut              : 3;     //  Prescaler Value (26 - 28)
        u32 L_Noise_Spur        : 2;     //  Low Noise and Low Spur Modes (29 - 30)
        u32 Resv                : 1;     //  Reserved(31)
    };
} adf4351_r2_t;

typedef union
{
    u32 r3_data;
    struct
    {
        u32 Control             : 3;     //  Control Bits(0 - 2)
        u32 Clock_Div_Val       : 12;    //  Double Buffer (3 - 14)
        u32 Clock_Div_Mod       : 2;     //  Clock Divider Mode (15 - 16)
        u32 Resv0               : 1;     //  Reserved0(17)
        u32 CSR                 : 1;     //  CSR Enable (18)
        u32 Resv1               : 2;     //  Reserved1(19 - 20)
        u32 Cancelation         : 1;     //  Charge Cancelation (21)
        u32 ABP                 : 1;     //  Antibacklash Pulse Width (ABP(22)) 
        u32 Band_Select_Clock   : 1;     //  Band Select Clock Mode (23)
        u32 Resv2               : 8;     //  Reserved2(24 - 31)
    };
} adf4351_r3_t;

typedef union
{
    u32 r4_data;
    struct
    {
        u32 Control         : 3;     //  Control Bits(0 - 2) 
        u32 Output_Power    : 2;     //  Phase Detector Polarity (3 - 4)
        u32 RF_Output_En    : 1;     //  RF Output Enable (LDP(5))
        u32 AuxOutpu_Power  : 2;     //  AUX Output Power (6 - 7))
        u32 AuxOutput_En    : 1;     //  AUX Output Enable (8)
        u32 AuxOutput       : 1;     //  AUX Output Select (9)
        u32 MTLD            : 1;     //  Mute Till Lock Detect (MTLD(10)) 
        u32 VCO             : 1;     //  VCO Power-Down (11)
        u32 B_Clock_Div_Val : 8;     //  Band Select Clock Divider Value(12 - 19)
        u32 RF_Div          : 3;     //  RF Divider Select (20 - 22)
        u32 Feedback        : 1;     //  Feedback Select (23)
        u32 Resv            : 8;     //  Reserved(24 - 31)
    };
} adf4351_r4_t;

typedef union
{
    u32 r5_data;
    struct
    {
        u32 Control     : 3;     //  Control Bits(0 - 2) 
        u32 Resv0       : 19;    //  Reserved(3 - 21)
        u32 Lock_Detect : 2;     //  Lock Detect Pin Operation (22 - 23)
        u32 Resv1       : 8;     //  Reserved(24 - 31)
    };
} adf4351_r5_t;

/** @addtogroup  ADF4351 Interfaces_Functions
  * @brief       This section provide a set of functions used to read and
  *              write a generic register of the device.
  *              MANDATORY: return 0 -> no Error.
  * @{
  *
  */
void dx_hw_adf4351_init(void);

/**
  *@}
  *
  */

#ifdef __cplusplus
}
#endif

#endif /* __GYRO_ADF4351_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

