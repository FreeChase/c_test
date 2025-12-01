/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    antijam_fpga.h
  * @brief   This file contains all the function prototypes for
  *          the antijam_fpga.c file.
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
  * 2024-11-26     Andy      the first version
  ******************************************************************************
  */

#ifndef _ANTIJAM_FPGA_H
#define _ANTIJAM_FPGA_H

/* Includes ------------------------------------------------------------------*/
#include "dxdef.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup ANTIJAM_FPGA Infos
  * @{
  *
  */

#define ANTI4B3_IRQ_EN                 0   /*中断使能*/
#define ANTI4B3_IRQ_CLR            	   1   /*中断状态清除*/
#define ANTI4B3_IRQ_STATUS             2   /*中断状态清除*/

#define ANTI4B3_GAIN_CTRL_EN           10  /*功率统计使能*/
#define ANTI4B3_GAIN_PWR_NUM           11  /*功率统计点数*/
#define ANTI4B3_GAIN_PWR_RPT0          12  /*增益功率统计0 只读*/ 
#define ANTI4B3_GAIN_PWR_RPT1          13  /*增益功率统计1 只读*/
#define ANTI4B3_GAIN_PWR_RPT2          14  /*增益功率统计2 只读*/
#define ANTI4B3_GAIN_PWR_RPT3          15  /*增益功率统计3 只读*/
#define ANTI4B3_GAIN_PWR_RPT4          16  /*增益功率统计4 只读*/
#define ANTI4B3_GAIN_PWR_RPT5          17  /*增益功率统计5 只读*/
#define ANTI4B3_GAIN_PWR_RPT6          18  /*增益功率统计6 只读*/
#define ANTI4B3_GAIN_PWR_RPT7          19  /*增益功率统计7 只读*/
#define ANTI4B3_GAIN_PWR_max           20  /*增益功率统计最大值 只读*/

#define ANTI4B3_MATRIX_EN              30  /*矩阵计算使能 */
#define ANTI4B3_ZD_MODE                31  /*ZD模式 */
#define ANTI4B3_DATA_SWITCH            32  /*主路数据选择 */
#define ANTI4B3_SUBBAND_FLAG           33  /*子带标志 */
#define ANTI4B3_SWITCH_NUM             34  /*  */
#define ANTI4B3_AGC_EN                 35  /*使能信号，1'b0打开AGC，反之直通*/
#define ANTI4B3_AGC_TH                 36  /*门限乘法因子*/
#define ANTI4B3_FIR_COE_SWITCH         37  /*滤波器系数切换*/

// adc dco 相位校正
#define ADC_DCO_DELAY_CE               40  /*使能*/
#define ADC_DCO_DELAY_LD               41  /*加载延时值*/
#define ADC_DCO_DELAY_INC              42  /*1：增加延迟 0：减少延迟*/

#define ADC_DCO_DELAY_VALUE0            50  /*使能*/
#define ADC_DCO_DELAY_VALUE1            51  /*使能*/
#define ADC_DCO_DELAY_VALUE2            52  /*使能*/
#define ADC_DCO_DELAY_VALUE3            53  /*使能*/

// 数据采集
#define ANTI4B3_SAM_CTRL               100 /*数据采集控制
                                            [0]   sam_dat_on
                                            [6:4] sam_dat_sel
                                            [8]   sam_dat_run 只读*/
                                            
#define ANTI4B3_SAM_ADDR_BASE          0x20000 /*数据缓存基地址*/
//#define ANTI4B3_SAM_ADDR_BASE          0x2000000 /*数据缓存基地址*/


#define FPGA_MCU_ADDR_BASE             0x40100000 /*数据缓存基地址*/

#define FPGA_XADC_TEMP_REG			   (170)

#define FPGA_STAP_WEIGHT_TIMEC_REG			   (131)
#define FPGA_STAP_BYPASSSET_REG			   (132)

/******************功率统计寄存器
*addr：156                 [2:0]功率统计长度 （0:8192 1:16384 2:32768 ...7:1048576）[28:30]:功率统计通道选择  [31]功率统计使能
*addr：157                 [23:0]功率统计更新周期 
*addr：158                 对消前功率
*addr：159                 对消后功率
*addr：160                 一次功率统计完成标志：1：完成 
*
*
**/
#define FPGA_STAP_SJR_CTRL_REG				    (156)
#define FPGA_STAP_SJR_CAL_TIME_REG				(157)
#define FPGA_STAP_BEFORE_POWER_REG				(158)
#define FPGA_STAP_AFTER_POWER_REG   	  		(159)
#define FPGA_STAP_SJR_CAL_DONE_REG				(160)

#define FPGA_STAP_ADJ_PHASEAPM_SET_BASEREG				(162)

#define FPGA_STAP_ADJ_PHASEAPM_CONFIG_REG				(162)



void antijam_fpga_write_reg(u8 addr, u32 val);
u32 antijam_fpga_read_reg(u8 addr);
int dx_hw_antijam_fpga_init(void);
void mcu_to_fpga_write_kgr_reg(u32 addr, u32 val);
u32 mcu_to_fpga_read_kgr_reg(u32 addr);

extern u32 Value[9];
extern int IntrFlag;
#ifdef __cplusplus
}
#endif
#endif
