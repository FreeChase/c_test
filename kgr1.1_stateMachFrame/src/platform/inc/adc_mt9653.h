/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc_mt9653.h
  * @brief   This file contains all the function prototypes for
  *          the adc_mt9653.c file.
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
  * 2024-11-21     Andy      the first version
  ******************************************************************************
  */

#ifndef _ADC_MT9653_H
#define _ADC_MT9653_H

/* Includes ------------------------------------------------------------------*/
#include "dxdef.h"
#include "sys_ctrl.h"
// 调节adc dco相位控制的寄存器位于抗干扰模块配置地址中
#include "antijam_fpga.h"

#ifdef __cplusplus
extern "C" {
#endif
/*芯片配置寄存器*/
#define MT9653_CONFIG_SPI_PORT_REG                     0x00/*SPI端口配置寄存器*/
#define MT9653_CHIPID_REG                              0x01/*芯片ID寄存器*/
#define MT9653_CHIP_LEVEL_REG                          0x02/*芯片等级寄存器*/
/*通道索引和传送寄存器*/
#define MT9653_CHANNEL_INDEX_REG                       0x05/*通道索引寄存器*/
#define MT9653_TRANSMIT_REG                            0xFF/*传送寄存器*/
/*ADC功能寄存器*/
#define MT9653_POWER_MODE_REG                          0x08/*功耗模式寄存器*/
#define MT9653_GLOBAL_CLOCK_REG                        0x09/*全局时钟寄存器*/
#define MT9653_CLOCK_DIVIDER_REG                       0x0B/*时钟分频器寄存器*/
#define MT9653_DATA_OUTTEST_MODE_REG                   0x0D/*数据输出测试模式寄存器*/
#define MT9653_DATA_OUT_MODE_REG                       0x14/*数据输出模式寄存器*/
#define MT9653_DATA_OUT_ADJUST_REG                     0x15/*数据输出调整寄存器*/
#define MT9653_DCO_OUT_DELAY_REG                       0x16/*DCO输出延迟寄存器*/
#define MT9653_VREF_MUX_REG                            0x18/*VREF选择寄存器*/
#define MT9653_DATA_OUT_CTRL_REG                       0x21/*串行数据输出控制寄存器*/
/*数字特性控制寄存器*/
#define MT9653_SYNC_CTRL_REG                           0x109/*同步控制寄存器*/

#define MT9653_ID                                      (0xB5)
typedef enum
{
    ADC_MT9653_IC1 = 1,
    ADC_MT9653_IC2
}eAdcMT9653ChipMux;

typedef enum
{
    ADC_MT9653_CS_LOW = 0,
    ADC_MT9653_CS_HIGH
}eAdcMT9653CsLevelMux;

struct adc_mt9653_config {
    u8 RegAddr;
    u32 RegData;
};
int dx_hw_mt9653_ic0_init(void);
int dx_hw_mt9653_ic1_init(void);

int dx_hw_mt9653_init(void);
int dx_hw_mt9653_test_mode_on(void);
int dx_hw_mt9653_test_mode_off(void);
int dx_hw_mt9653_adjust_init(void);
int dx_hw_mt9653_adjust_judge(void);
int dx_hw_mt9653_delay_inc(u8 val);
int dx_hw_mt9653_delay_init(void);
u8 dx_hw_mt9653_self_test(void);

int dx_hw_mt9653_delay_value_adjust(void);
int dx_hw_mt9653_ic1_lowpower_mode_on(void);
int dx_hw_mt9653_ic1_lowpower_mode_off(void);

int dx_hw_mt9653_remove_dc_offset(void);


void mt9653_ic0_write_reg(u16 addr, u8 val);
u8 mt9653_ic0_read_reg(u16 addr);


void mt9653_ic1_write_reg(u16 addr, u8 val);
u8 mt9653_ic1_read_reg(u16 addr);



/*********************************************
* 函数名 adc_ctrl_chn
* 函数功能：控制AD通道的工作状态：关闭或打开
*参数：     
*		socId: 芯片序号，0,1
*		chn：bit3 bit2 bit1 bit0
*			  D   C   B  A
*		ctlVal: 1关闭，0打开
*
*/
int adc_ctrl_chn(unsigned int adId,unsigned int chn,unsigned int ctlVal);



#ifdef __cplusplus
}
#endif
#endif
