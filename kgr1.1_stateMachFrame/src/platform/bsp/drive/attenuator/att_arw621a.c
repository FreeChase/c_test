/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    att_pe43711.c
  * @brief   This file provides code for the configuration
  *          of the SPI Attenuator PE43711 instances.
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
#include <att_arw621a.h>
#include <stdio.h>
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "dxdef.h"
#include "antijam_fpga.h"
#define PE43711_SPI_LEA		        (13)
#define PE43711_SPI_LEB		       	(0)
#define PE43711_SPI_SCLK 		    (14)
#define PE43711_SPI_SI		        (15)

/******************************************************************************/
/**
*
* This is the interrupt handler routine for the GPIO for this example.
*
* @param	CallbackRef is the Callback reference for the handler.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/


void att_dx_dly_us(unsigned int tmpVal)
{
	for(int i = 0; i< 1000*tmpVal ; i++);
}

//int IntrFlag;
//void pe43711_handler(void *CallbackRef)
//{
//
//	Xil_ExceptionDisable();
//	/* Clear the Interrupt */
//	Value[0]=antijam_fpga_read_reg(0xF7);
//	//Value[1]=antijam_fpga_read_reg(0xF8);
//	Value[1]=antijam_fpga_read_reg(0xF9);
//	Value[2]=antijam_fpga_read_reg(0xFa);
//	Value[3]=antijam_fpga_read_reg(0xFb);
//	Value[4]=antijam_fpga_read_reg(0xFc);
////	Value[2]=antijam_fpga_read_reg(0xFd);
////	Value[3]=antijam_fpga_read_reg(0xFe);
////	Value[4]=antijam_fpga_read_reg(0xFf);
//	IntrFlag = 1;
//    //Xil_ExceptionEnable();
//}

/*****************************************************************************************************
*	函 数 名: static void pe43711_gpio_spi_begin(eAttPE43711ChipMux pe43711Mux)
*	功能说明: 模拟SPI总线前，将硬件SPI配置为GPIO
*	形	  参:
*			  eAttPE43711ChipMux pe43711Mux: 选择pe43711芯片
*	返 回 值: 无
******************************************************************************************************/
static void pe43711_gpio_spi_begin(eAttPE43711ChipMux pe43711Mux)
{
	PERI_DX_PIN_OUTPUT_MODE(PE43711_SPI_SCLK);
	PERI_DX_PIN_OUTPUT_MODE(PE43711_SPI_SI);
    //
    PERI_DX_PIN_OUPUT_LOW(PE43711_SPI_SCLK);

    switch (pe43711Mux)
	{
		case ATT_PE43711_IC0:
		{
			PERI_DX_PIN_OUTPUT_MODE(PE43711_SPI_LEA);
			PERI_DX_PIN_OUPUT_HIGH(PE43711_SPI_LEA);
		}
		break;
		case ATT_PE43711_IC1:
		{
			PERI_DX_PIN_OUTPUT_MODE(PE43711_SPI_LEB);
			PERI_DX_PIN_OUPUT_HIGH(PE43711_SPI_LEB);
		}
		break;
		default:
			break;
	}
}

/*****************************************************************************************************
*	函 数 名: static void pe43711_gpio_spi_end(eAttPE43711ChipMux pe43711Mux)
*	功能说明: 模拟SPI处理完后，将配置为硬件SPI模式
*	形	  参:
*			  eAttPE43711ChipMux pe43711Mux: 选择pe43711芯片
*	返 回 值: 无
******************************************************************************************************/
static void pe43711_gpio_spi_end(eAttPE43711ChipMux pe43711Mux)
{
	PERI_DX_PIN_OUPUT_HIGH(PE43711_SPI_SCLK);
    PERI_DX_PIN_OUPUT_HIGH(PE43711_SPI_SI);
    switch (pe43711Mux)
	{
		case ATT_PE43711_IC0:
		{
			//PERI_DX_PIN_OUPUT_HIGH(PE43711_SPI_LEA);
			PERI_DX_PIN_OUPUT_LOW(PE43711_SPI_LEA);
		}
		break;
		case ATT_PE43711_IC1:
		{
			//PERI_DX_PIN_OUPUT_HIGH(PE43711_SPI_LEB);
			PERI_DX_PIN_OUPUT_LOW(PE43711_SPI_LEB);
		}
		break;
		default:
			break;
	}
}

/*****************************************************************************************************
*	函 数 名: static void att_pe43711_cs_level_mux(eAttPE43711ChipMux pe43711Mux, AttPE43711CsLevelMux pe43711CsLevelMux)
*	功能说明: 选择pe43711芯片
*	形	  参:
*			  eAttPE43711ChipMux pe43711Mux: 选择pe43711芯片
*			  AttPE43711CsLevelMux pe43711CsLevelMux: 选择PE43711芯片选高低
*	返 回 值: 无
******************************************************************************************************/
static void att_pe43711_cs_level_mux(eAttPE43711ChipMux pe43711Mux, AttPE43711CsLevelMux pe43711CsLevelMux)
{
	if (ATT_PE43711_CS_LOW == pe43711CsLevelMux)
	{
		switch (pe43711Mux)
		{
			case ATT_PE43711_IC0:
			{
				PERI_DX_PIN_OUPUT_LOW(PE43711_SPI_LEA);
			}
			break;
			case ATT_PE43711_IC1:
			{
				PERI_DX_PIN_OUPUT_LOW(PE43711_SPI_LEB);
			}
			break;
			default:
				break;
		}
	}
	else
	{
		switch (pe43711Mux)
		{
			case ATT_PE43711_IC0:
			{
				PERI_DX_PIN_OUPUT_HIGH(PE43711_SPI_LEA);
			}
			break;
			case ATT_PE43711_IC1:
			{
				PERI_DX_PIN_OUPUT_HIGH(PE43711_SPI_LEB);
			}
			break;
			default:
				break;
		}
	}
}

/*****************************************************************************************************
*	函 数 名: static void att_pe43711_gpio_spi_write_reg(eAttPE43711ChipMux pe43711Mux, u8 val)
*	功能说明: pe43711Mux模拟spi总线写数据
*	形	  参:
*			  eAttPE43711ChipMux pe43711Mux: 选择pe43711Mux芯片
*             u8 val: 写数据
*	返 回 值: 无
******************************************************************************************************/
static void att_pe43711_gpio_spi_write_reg(eAttPE43711ChipMux pe43711Mux, u8 val)
{
    u8 wcnt;
    u32 dly_time = 1;
    //串行数据D[7]必须设置为0
    u8 wdata = 0x7F & val;

    pe43711_gpio_spi_begin(pe43711Mux);

    att_pe43711_cs_level_mux(pe43711Mux, ATT_PE43711_CS_LOW);
    att_dx_dly_us(dly_time);

    for (wcnt = 0; wcnt < 8; wcnt++)
    {
    	PERI_DX_PIN_OUPUT_LOW(PE43711_SPI_SCLK);

        if (wdata & 0x1)
        {
        	PERI_DX_PIN_OUPUT_HIGH(PE43711_SPI_SI);
        }
        else
        {
        	PERI_DX_PIN_OUPUT_LOW(PE43711_SPI_SI);
        }
        wdata >>= 1;

        att_dx_dly_us(dly_time);
        PERI_DX_PIN_OUPUT_HIGH(PE43711_SPI_SCLK);
    }
    //
    PERI_DX_PIN_OUPUT_LOW(PE43711_SPI_SCLK);
    att_dx_dly_us(10);
    att_pe43711_cs_level_mux(pe43711Mux, ATT_PE43711_CS_HIGH);
    att_dx_dly_us(10);
    att_pe43711_cs_level_mux(pe43711Mux, ATT_PE43711_CS_LOW);
    pe43711_gpio_spi_end(pe43711Mux);
}

/*****************************************************************************************************
*	函 数 名: static void pe43711_ic0_write(u8 val)
*	功能说明: pe43711 IC0写数据
*	形	  参:
*             val 写的数据
*	返 回 值: 无
******************************************************************************************************/
void pe43711_ic0_write(u8 val)
{
    att_pe43711_gpio_spi_write_reg(ATT_PE43711_IC0, val);
}

/*****************************************************************************************************
*	函 数 名: int dx_hw_pe43711_ic0_init(void)
*	功能说明: pe43711 IC0初始化
*	形	  参: 无
*	返 回 值: 成功返回:DX_EOK	失败返回:-DX_EIO
******************************************************************************************************/
int dx_hw_pe43711_ic0_init(void)
{
	//SystemSetupSoftIntr(&IntcInstancePtr, INTC_SOFT_INTERRUPT_ID, pe43711_handler);
	float val = 0.0;
	if ((HW_KGR_B3_VER_4_54 == g_HwModuleVer) | (HW_KGR_B3_VER_7_KD == g_HwModuleVer) | (HW_KGR_B3_VER_4_KD == g_HwModuleVer))//B3 通道衰减器默认：第一级衰减7dB
	{
		val = 0 / 0.25;
	}
	else if (HW_KGR_B1_VER_4_54 == g_HwModuleVer)//B1 通道衰减器默认：第一级衰减7dB
	{
		val = 0 / 0.25;
	}
	else if (HW_KGR_S_VER_4_54 == g_HwModuleVer)//S频点衰减器默认：第一级衰减3dB
	{
		val = 0 / 0.25;
	}
	else
	{
		return DX_ERROR;
	}
	pe43711_ic0_write(val);
    return DX_EOK;
}

/*****************************************************************************************************
*	函 数 名: static void pe43711_ic1_write(u8 val)
*	功能说明: pe43711 IC1写数据
*	形	  参:
*             val 写的数据
*	返 回 值: 无
******************************************************************************************************/
void pe43711_ic1_write(u8 val)
{
    att_pe43711_gpio_spi_write_reg(ATT_PE43711_IC1, val);
}

/*****************************************************************************************************
*	函 数 名: int dx_hw_pe43711_ic1_init(void)
*	功能说明: pe43711 IC1 初始化
*	形	  参: 无
*	返 回 值: 成功返回:DX_EOK	失败返回:-DX_EIO
******************************************************************************************************/
int dx_hw_pe43711_ic1_init(void)
{
	float val = 0.0;
	if ((HW_KGR_B3_VER_4_54 == g_HwModuleVer) | (HW_KGR_B3_VER_7_KD == g_HwModuleVer) | (HW_KGR_B3_VER_4_KD == g_HwModuleVer))//B3 通道衰减器默认：第二级衰减10dB
	{
		val = 0 / 0.25;
	}
	else if (HW_KGR_B1_VER_4_54 == g_HwModuleVer)//B1 通道衰减器默认：第二级衰减10dB
	{
		val = 0 / 0.25;
	}
	else if (HW_KGR_S_VER_4_54 == g_HwModuleVer)//S频点衰减器默认：第二级衰减2dB
	{
		val = 0 / 0.25;
	}
	else
	{
		return DX_ERROR;
	}
	pe43711_ic1_write(val);
    return DX_EOK;
}

/************************ (C) COPYRIGHT Microelectronics *****END OF FILE****/

