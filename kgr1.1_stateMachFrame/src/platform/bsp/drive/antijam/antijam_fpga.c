/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    antijam_fpga.c
  * @brief   This file provides code for the configuration
  *          of the spi antijam instances.
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
#include <stdio.h>
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"

#include "antijam_fpga.h"

#include "xil_types.h"
#include "xil_assert.h"
#include "xil_io.h"

/*****************************************************************************************************
*	函 数 名: s32 bd21_to_fpga_write_kgr_reg(u8 addr,u32 val)
*	功能说明: bd21_fpga模拟spi总线写数据
*	形	  参:
*		      u8 addr: 寄存器地址
*	返 回 值: 寄存器的值
******************************************************************************************************/
void mcu_to_fpga_write_kgr_reg(u32 addr, u32 val)
{
	*(volatile u32*)(XPAR_MCU_AHB_BASEADDR + addr*4) = val;
	//Xil_Out32(0x40100000 + addr*4,val);
}

/*****************************************************************************************************
*	函 数 名: s32 bd21_to_fpga_read_kgr_reg(u8 addr,u32 val)
*	功能说明: bd21_fpga模拟spi总线写数据
*	形	  参:
*		      u8 addr: 寄存器地址
*	返 回 值: 寄存器的值
******************************************************************************************************/
u32 mcu_to_fpga_read_kgr_reg(u32 addr)
{
	return *(volatile u32*)(XPAR_MCU_AHB_BASEADDR + addr*4);
}


int dx_hw_antijam_fpga_init(void)
{
	u32 sam_data_buf[1024] = {0};
	int i;
	// 寄存器遍历
#if 0
	u32 reg_value;
	mcu_to_fpga_write_kgr_reg(ANTI4B3_IRQ_EN, 0x1);
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_IRQ_EN); 
	mcu_to_fpga_write_kgr_reg(ANTI4B3_IRQ_EN, 0x0);
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_IRQ_EN);
	
	mcu_to_fpga_write_kgr_reg(ANTI4B3_IRQ_CLR, 0x1);      
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_IRQ_CLR);
	mcu_to_fpga_write_kgr_reg(ANTI4B3_IRQ_CLR, 0x0);
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_IRQ_CLR); 
	
	mcu_to_fpga_write_kgr_reg(ANTI4B3_GAIN_CTRL_EN, 0x1);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_GAIN_CTRL_EN); 
	mcu_to_fpga_write_kgr_reg(ANTI4B3_GAIN_CTRL_EN, 0x0);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_GAIN_CTRL_EN); 
	
	mcu_to_fpga_write_kgr_reg(ANTI4B3_GAIN_PWR_NUM, 0xfffffff);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_GAIN_PWR_NUM); 
	mcu_to_fpga_write_kgr_reg(ANTI4B3_GAIN_PWR_NUM, 0x0);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_GAIN_PWR_NUM);
	   
	mcu_to_fpga_write_kgr_reg(ANTI4B3_MATRIX_EN, 0x1);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_MATRIX_EN); 
	mcu_to_fpga_write_kgr_reg(ANTI4B3_MATRIX_EN, 0x0);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_MATRIX_EN);
	
	mcu_to_fpga_write_kgr_reg(ANTI4B3_ZD_MODE, 0x3);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_ZD_MODE); 
	mcu_to_fpga_write_kgr_reg(ANTI4B3_ZD_MODE, 0x0);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_ZD_MODE);
	
	mcu_to_fpga_write_kgr_reg(ANTI4B3_DATA_SWITCH, 0x7);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_DATA_SWITCH); 
	mcu_to_fpga_write_kgr_reg(ANTI4B3_DATA_SWITCH, 0x0);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_DATA_SWITCH);
	
	mcu_to_fpga_write_kgr_reg(ANTI4B3_SUBBAND_FLAG, 0x1);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_SUBBAND_FLAG); 
	mcu_to_fpga_write_kgr_reg(ANTI4B3_SUBBAND_FLAG, 0x0);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_SUBBAND_FLAG);
	
	mcu_to_fpga_write_kgr_reg(ANTI4B3_SWITCH_NUM, 0xff);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_SWITCH_NUM); 
	mcu_to_fpga_write_kgr_reg(ANTI4B3_SWITCH_NUM, 0x0);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_SWITCH_NUM);
	
	mcu_to_fpga_write_kgr_reg(ANTI4B3_AGC_EN, 0x1);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_AGC_EN); 
	mcu_to_fpga_write_kgr_reg(ANTI4B3_AGC_EN, 0x0);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_AGC_EN);
	
	mcu_to_fpga_write_kgr_reg(ANTI4B3_AGC_TH, 0xffff);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_AGC_TH); 
	mcu_to_fpga_write_kgr_reg(ANTI4B3_AGC_TH, 0x0);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_AGC_TH);
	
	mcu_to_fpga_write_kgr_reg(ANTI4B3_AGC_TH, 0x3);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_AGC_TH); 
	mcu_to_fpga_write_kgr_reg(ANTI4B3_AGC_TH, 0x0);       
	reg_value = mcu_to_fpga_read_kgr_reg(ANTI4B3_AGC_TH);
	end
#endif
    // 数据采集功能

//	mcu_to_fpga_write_kgr_reg(ANTI4B3_SAM_CTRL, 0x1);
//	mcu_to_fpga_write_kgr_reg(ANTI4B3_SAM_CTRL, 0x0);
//	dx_dly_us(1);
//	for(i=0;i<1024;i++)
//	{
//		sam_data_buf[i]=mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i);
//		//dx_kprintf("//////////////////////%8x\r\n", sam_data_buf[i]);
//	}


	return DX_EOK;
}
