/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    att_pe43711.h
  * @brief   This file contains all the function prototypes for
  *          the att_pe43711.c file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024, ZS Development Team.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  * Change Logs:
  * Date           Author       Notes
  * 2024-11-25     Andy      the first version
  * 2024-12-22     Andy      modify
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ATT_PE43711_H__
#define __ATT_PE43711_H__

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "dxdef.h"
#include "sys_ctrl.h"

/** @defgroup PE43711 Infos
  * @{
  *
  */

#define ATT_PE43711_RIL        0x0 /*Reference IL*/
#define ATT_PE43711_0_25       0x1 /*0.25dB*/
#define ATT_PE43711_0_5        0x2 /*0.5dB*/
#define ATT_PE43711_1_0        0x4 /*1dB*/
#define ATT_PE43711_2_0        0x8 /*2dB*/
#define ATT_PE43711_4_0        0x10 /*4dB*/
#define ATT_PE43711_8_0        0x20 /*8dB*/
#define ATT_PE43711_16_0       0x40 /*16dB*/
#define ATT_PE43711_31_75      0x7F /*31.75dB*/

typedef enum
{
    ATT_PE43711_IC0 = 0,
    ATT_PE43711_IC1
}eAttPE43711ChipMux;

typedef enum
{
	ATT_PE43711_CS_LOW = 0,
	ATT_PE43711_CS_HIGH
}AttPE43711CsLevelMux;

/** @addtogroup  PE43711 Interfaces_Functions
  * @brief       This section provide a set of functions used to read and
  *              write a generic register of the device.
  *              MANDATORY: return 0 -> no Error.
  * @{
  *
  */
void pe43711_ic0_write(u8 val);
void pe43711_ic1_write(u8 val);
int dx_hw_pe43711_ic0_init(void);
int dx_hw_pe43711_ic1_init(void);

/**
  *@}
  *
  */

#ifdef __cplusplus
}
#endif

#endif /* __ATT_PE43711_H__ */

/************************ (C) COPYRIGHT Microelectronics *****END OF FILE****/

