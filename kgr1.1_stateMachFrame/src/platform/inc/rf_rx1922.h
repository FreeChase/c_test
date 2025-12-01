/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rf_rx1922.h
  * @brief   This file contains all the function prototypes for
  *          the rf_rx1922.c file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025, ZS Development Team.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  * Change Logs:
  * Date           Author       Notes
  * 2025-4-27     Andy      the first version
  ******************************************************************************
  */

#ifndef _RF_RX1922_H
#define _RF_RX1922_H

#include "xil_types.h"
#define CONFIG_RF_RX1922

#ifdef __cplusplus
extern "C" {
#endif

#define RX1922_REG0_ADDR                     0x00
#define RX1922_REG1_ADDR                     0x01
#define RX1922_REG2_ADDR                     0x02
#define RX1922_REG3_ADDR                     0x03
#define RX1922_REG4_ADDR                     0x04
#define RX1922_REG5_ADDR                     0x05
#define RX1922_REG6_ADDR                     0x06
#define RX1922_REG7_ADDR                     0x07
#define RX1922_REG8_ADDR                     0x08
#define RX1922_REG9_ADDR                     0x09
#define RX1922_REG10_ADDR                    0x0A
#define RX1922_REG11_ADDR                    0x0B
#define RX1922_REG12_ADDR                    0x0C
#define RX1922_REG13_ADDR                    0x0D
#define RX1922_REG14_ADDR                    0x0E
#define RX1922_REG15_ADDR                    0x0F
#define RX1922_REG16_ADDR                    0x10
#define RX1922_REG17_ADDR                    0x11
#define RX1922_REG18_ADDR                    0x12
#define RX1922_REG19_ADDR                    0x13
#define RX1922_REG20_ADDR                    0x14
#define RX1922_REG21_ADDR                    0x15
#define RX1922_REG22_ADDR                    0x16
#define RX1922_REG23_ADDR                    0x17
#define RX1922_REG24_ADDR                    0x18
#define RX1922_REG25_ADDR                    0x19
#define RX1922_REG26_ADDR                    0x1A
#define RX1922_REG27_ADDR                    0x1B
#define RX1922_REG28_ADDR                    0x1C
#define RX1922_REG29_ADDR                    0x1D
#define RX1922_REG30_ADDR                    0x1E
#define RX1922_REG31_ADDR                    0x1F
#define RX1922_REG32_ADDR                    0x20
#define RX1922_REG33_ADDR                    0x21
#define RX1922_REG34_ADDR                    0x22
#define RX1922_REG35_ADDR                    0x23
#define RX1922_REG36_ADDR                    0x24
#define RX1922_REG37_ADDR                    0x25
#define RX1922_REG38_ADDR                    0x26
#define RX1922_REG39_ADDR                    0x27
#define RX1922_REG40_ADDR                    0x28
#define RX1922_REG41_ADDR                    0x29


typedef enum
{
    RF_RX1922_C1 = 1,
    RF_RX1922_C2
}eRfRx1922ChipMux;

typedef enum 
{
    RF_RX1922_CH1 = 0,
    RF_RX1922_CH2,
    RF_RX1922_CH3,
    RF_RX1922_CH4
}eRfRx1922ChannelMux;

typedef enum
{
    RF_RX1922_CS_LOW = 0,
    RF_RX1922_CS_HIGH
}eRfRx1922CsLevelMux;

struct rf_rx1922_config {
    u8 RegAddr;
    u32 RegData;
};


extern unsigned char g_KgrFreqId;



void rx1922_init(void);

void rx1922_write_reg(u32 addr, u32 val);

u32 rx1922_read_reg(u32 addr);

int rx1922_Set_GaindB(u16 chcn,float gainVal);

float rx1922_Get_GaindB(u16 chcn);


void rx1922_open_chn(u16 chnno);

void rx1922_close_chn(u16 chnno);



#ifdef __cplusplus
}
#endif
#endif
