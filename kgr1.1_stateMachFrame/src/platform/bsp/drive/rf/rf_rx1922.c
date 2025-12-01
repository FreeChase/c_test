/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rf_rx1922.c
  * @brief   This file provides code for the configuration
  *          of the SPI RF RX1922 instances.
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
#include <stdio.h>

#include "rf_rx1922.h"
#include "sys_ctrl.h"
//#include "KgrInfo.h"

#define DBG_TAG    "RF_RX1922"
#define DBG_LVL    DBG_LOG//DBG_INFO


#define RX1922_GPIO_SPI_CS0				(9)
#define RX1922_GPIO_SPI_CLK				(10)
#define RX1922_GPIO_SPI_MISO			(12)
#define RX1922_GPIO_SPI_MOSI			(11)

#define rf_rx1922_spi_csa_out()     			PERI_DX_PIN_OUTPUT_MODE(RX1922_GPIO_SPI_CS0)
#define rf_rx1922_spi_csa_high()      			PERI_DX_PIN_OUPUT_HIGH(RX1922_GPIO_SPI_CS0)
#define rf_rx1922_spi_csa_low()       			PERI_DX_PIN_OUPUT_LOW(RX1922_GPIO_SPI_CS0)

#define rf_rx1922_spi_miso_in()      			PERI_DX_PIN_INTPUT_MODE(RX1922_GPIO_SPI_MISO)
#define rf_rx1922_spi_miso_read()    			PERI_DX_PIN_INTPUT(RX1922_GPIO_SPI_MISO)

#define rf_rx1922_spi_clk_out()     			PERI_DX_PIN_OUTPUT_MODE(RX1922_GPIO_SPI_CLK)
#define rf_rx1922_spi_clk_high()     			PERI_DX_PIN_OUPUT_HIGH(RX1922_GPIO_SPI_CLK)
#define rf_rx1922_spi_clk_low()      			PERI_DX_PIN_OUPUT_LOW(RX1922_GPIO_SPI_CLK)

#define rf_rx1922_spi_mosi_out()     			PERI_DX_PIN_OUTPUT_MODE(RX1922_GPIO_SPI_MOSI)
#define rf_rx1922_spi_mosi_high()				PERI_DX_PIN_OUPUT_HIGH(RX1922_GPIO_SPI_MOSI)
#define rf_rx1922_spi_mosi_low()				PERI_DX_PIN_OUPUT_LOW(RX1922_GPIO_SPI_MOSI)

void delay_dx_rf(unsigned int timeVal)
{
	for(int i = 0; i < timeVal*20; i++)
		for(int j = 0; j < timeVal*1; j++);
}

/*
* ****************************************************************
*2025 07 07 进行寄存器配置修改
* reg24 修改成0x180003;  原配置0x400003；
* reg25 修改为0xAFC09F;原配置0xFC209F；
*
*2025 07 10
* 4个通道RF增益=44
* 所有通道增益更改为30db,主支路偏置电流及补偿通路偏置电流均改为最大值！
* 在之前的基础上添加环路滤波，可以有效减少带内杂散！
*******************************************************************
*
*/
static const struct rf_rx1922_config IC0_B1_RX1922Regs_1490[] =
{
#if 0
		{ RX1922_REG0_ADDR ,  0x00F793 }, { RX1922_REG1_ADDR ,  0x514514 }, { RX1922_REG2_ADDR ,  0xEFFC2A },
		{ RX1922_REG3_ADDR ,  0x38E38E }, { RX1922_REG4_ADDR ,  0xD0112A }, { RX1922_REG5_ADDR ,  0x000000 },
		{ RX1922_REG6_ADDR ,  0xFFFFF4 }, { RX1922_REG7_ADDR ,  0xF96C80 }, { RX1922_REG8_ADDR ,  0x000000 },
		{ RX1922_REG9_ADDR ,  0x4319F8 }, { RX1922_REG10_ADDR , 0xB60000 }, { RX1922_REG11_ADDR , 0xFFFFFF },
		{ RX1922_REG12_ADDR , 0x000000 }, { RX1922_REG13_ADDR , 0x00A741 }, { RX1922_REG14_ADDR , 0x57D500 },
		{ RX1922_REG15_ADDR , 0xFDB8F8 }, { RX1922_REG16_ADDR , 0x525800 }, { RX1922_REG17_ADDR , 0xFDB8F8 },
		{ RX1922_REG18_ADDR , 0x525800 }, { RX1922_REG19_ADDR , 0xFDB8F8 }, { RX1922_REG20_ADDR , 0x525800 },
		{ RX1922_REG21_ADDR , 0xFDB8F8 }, { RX1922_REG22_ADDR , 0x525800 }, { RX1922_REG23_ADDR , 0x7E8AC0 },
		{ RX1922_REG24_ADDR , 0x400003 }, { RX1922_REG25_ADDR , 0xFC209F }, { RX1922_REG26_ADDR , 0xD11900 },
		{ RX1922_REG27_ADDR , 0xC801F4 }, { RX1922_REG28_ADDR , 0x4598E3 }, { RX1922_REG29_ADDR , 0xE00052 },
		{ RX1922_REG30_ADDR , 0xFF3402 }, { RX1922_REG31_ADDR , 0xBA210A }, { RX1922_REG32_ADDR , 0x8981F4 },
		{ RX1922_REG33_ADDR , 0x640000 }, { RX1922_REG34_ADDR , 0xFB6155 }, { RX1922_REG35_ADDR , 0xE71000 },
		{ RX1922_REG36_ADDR , 0x7770EA }, { RX1922_REG37_ADDR , 0x00CCCC }, { RX1922_REG38_ADDR , 0x000000 },
		{ RX1922_REG39_ADDR , 0x000000 }, { RX1922_REG40_ADDR , 0x000000 }, { RX1922_REG41_ADDR , 0x5C0000 }
#else
	{ RX1922_REG0_ADDR ,  0x00F793 }, { RX1922_REG1_ADDR ,	0x514514 }, { RX1922_REG2_ADDR ,  0xEFFC2A },
	{ RX1922_REG3_ADDR ,  0xB2CB2C }, { RX1922_REG4_ADDR ,	0xD0112A}, { RX1922_REG5_ADDR ,  0x000000 }, 
	{ RX1922_REG6_ADDR ,  0xFFFFF4 }, { RX1922_REG7_ADDR ,	0xF96C80 }, { RX1922_REG8_ADDR ,  0x000000 },
	{ RX1922_REG9_ADDR ,  0x4319F8 }, { RX1922_REG10_ADDR , 0xB60000 }, { RX1922_REG11_ADDR , 0xFFFFFF },
	{ RX1922_REG12_ADDR , 0x000000 }, { RX1922_REG13_ADDR , 0x00A741 }, { RX1922_REG14_ADDR , 0x57D500 },
	{ RX1922_REG15_ADDR , 0xFDB8F8 }, { RX1922_REG16_ADDR , 0xFE5800 }, { RX1922_REG17_ADDR , 0xFDB8F8 },
	{ RX1922_REG18_ADDR , 0xFE5800 }, { RX1922_REG19_ADDR , 0xFDB8F8 }, { RX1922_REG20_ADDR , 0xFE5800 },
	{ RX1922_REG21_ADDR , 0xFDB8F8 }, { RX1922_REG22_ADDR , 0xFE5800 }, { RX1922_REG23_ADDR , 0x7E8AC0 },
	{ RX1922_REG24_ADDR , 0x180003 }, { RX1922_REG25_ADDR , 0xAFC09F }, { RX1922_REG26_ADDR , 0xD11900 },
	{ RX1922_REG27_ADDR , 0xC801F4 }, { RX1922_REG28_ADDR , 0x4598E3 }, { RX1922_REG29_ADDR , 0xE00052 },
	{ RX1922_REG30_ADDR , 0xFF3402 }, { RX1922_REG31_ADDR , 0xBA210A }, { RX1922_REG32_ADDR , 0x8981F4 },
	{ RX1922_REG33_ADDR , 0x640000 }, { RX1922_REG34_ADDR , 0xFB6155 }, { RX1922_REG35_ADDR , 0xE71000 },
	{ RX1922_REG36_ADDR , 0x7770EA }, { RX1922_REG37_ADDR , 0x00CCCC }, { RX1922_REG38_ADDR , 0x000000 },
	{ RX1922_REG39_ADDR , 0x000000 }, { RX1922_REG40_ADDR , 0x000000 }, { RX1922_REG41_ADDR , 0x5C0000 },

#endif
};



//模拟SPI总线前，将硬件SPI配置为RX1922 GPIO
/*****************************************************************************************************
*	函 数 名: static void xnd2260_gpio_spi_begin(void)
*	功能说明: 模拟SPI总线前，将硬件SPI配置为RX1922 GPIO
*	形	  参:
*             eRfRx1922ChipMux rfMux: 选择RX1922芯片
*	返 回 值: 无
******************************************************************************************************/
static void rx1922_gpio_spi_begin(eRfRx1922ChipMux rfMux)
{
	rf_rx1922_spi_clk_out();
	rf_rx1922_spi_miso_in();
	rf_rx1922_spi_mosi_out();

	rf_rx1922_spi_clk_low();
	rf_rx1922_spi_mosi_high();
	switch (rfMux)
	{
		case RF_RX1922_C1:
		{
			rf_rx1922_spi_csa_out();
			rf_rx1922_spi_csa_high();
		}
		break;
		default:
			break;
	}
}

/*****************************************************************************************************
*	函 数 名: static void rx1922_gpio_spi_end(eRfRx1922ChipMux rfMux)
*	功能说明: 模拟SPI处理完后，将RX1922_GPIO配置为硬件SPI模式
*	形	  参:
*             eRfRx1922ChipMux rfMux: 选择RX1922芯片
*	返 回 值: 无
******************************************************************************************************/
static void rx1922_gpio_spi_end(eRfRx1922ChipMux rfMux)
{
	rf_rx1922_spi_clk_high();
	rf_rx1922_spi_mosi_high();

	switch (rfMux)
	{
		case RF_RX1922_C1:
		{
			rf_rx1922_spi_csa_high();
		}
		break;
		default:
			break;
	}
}

/*****************************************************************************************************
*	函 数 名: static void rf_rx1922_cs_level_mux(eRfRx1922ChipMux rfMux, eRfRx1922CsLevelMux rfCsLevelMux)
*	功能说明: 选择rx1922芯片
*	形	  参:
*			  eRfRx1922ChipMux rfMux: 选择rx1922芯片
* 			  eRfCsLevelMux rfCsLevelMux: 选择rx1922芯片选高低
*	返 回 值: 无
******************************************************************************************************/
static void rf_rx1922_cs_level_mux(eRfRx1922ChipMux rfMux, eRfRx1922CsLevelMux rfCsLevelMux)
{
	if (RF_RX1922_CS_LOW == rfCsLevelMux)
	{
		switch (rfMux)
		{
			case RF_RX1922_C1:
			{
				rf_rx1922_spi_csa_low();
			}
			break;
			default:
				break;
		}
	}
	else
	{
		switch (rfMux)
		{
			case RF_RX1922_C1:
			{
				rf_rx1922_spi_csa_high();
			}
			break;
			default:
				break;
		}
	}
}

/*****************************************************************************************************
*	函 数 名: static void rf_rx1922_gpio_spi_write_reg(eRfRx1922ChipMux rfMux, 
															u8 addr, u32 val)
*	功能说明: rx1922模拟spi总线写数据
*	形	  参:
*			  eRfRx1922ChipMux rfMux: 选择rx1922芯片
*		      u8 addr: 寄存器地址
*             u32 val: 写数据
*	返 回 值: 无
******************************************************************************************************/
static void rf_rx1922_gpio_spi_write_reg(eRfRx1922ChipMux rfMux, u8 addr, u32 val)
{
	u32 TxData;
	u8 w_cnt;
	u32 dly_time = 50;

	addr &= 0x7F;/*7位有效地址*/
	val &= 0xFFFFFF;/*24位数据*/

	//写数据拼接，W最高位为低
	TxData = ((0x0 << 31) | (addr << 24) | val);

	rx1922_gpio_spi_begin(rfMux);
	//delay_dx_rf(dly_time);
	delay_dx_rf(dly_time);
	//<使能>，低有效
	rf_rx1922_cs_level_mux(rfMux, RF_RX1922_CS_LOW);
	delay_dx_rf(dly_time);
	for (w_cnt = 0; w_cnt < 32; w_cnt++)
	{
		//<时钟>,拉低
		rf_rx1922_spi_clk_low();

		//判断待写入数据是0还是1
		if (TxData & 0x80000000)
		{
			//待写入数据为1，将MOSI拉高
			rf_rx1922_spi_mosi_high();
		}
		else
		{
			//待写入数据为0，将MOSI拉低
			rf_rx1922_spi_mosi_low();
		}
		delay_dx_rf(dly_time);

		//待写入数据移至下一位
		TxData <<= 1;

		//<时钟>,拉高
		rf_rx1922_spi_clk_high();
		delay_dx_rf(dly_time);
	}

	//<时钟>,拉低
	rf_rx1922_spi_clk_low();
	//写完成，<使能>拉高
	rf_rx1922_cs_level_mux(rfMux, RF_RX1922_CS_HIGH);
	delay_dx_rf(dly_time);
	rx1922_gpio_spi_end(rfMux);
	delay_dx_rf(dly_time);
}

/*****************************************************************************************************
*	函 数 名: static u32 rf_rx1922_gpio_spi_read_reg(eRfRx1922ChipMux rfMux, u8 addr)
*	功能说明: rx1922模拟spi总线写数据
*	形	  参:
*			  eRfRx1922ChipMux rfMux: 选择rx1922芯片
*		      u8 addr: 寄存器地址
*	返 回 值: 寄存器的值
******************************************************************************************************/
static u32 rf_rx1922_gpio_spi_read_reg(eRfRx1922ChipMux rfMux, u8 addr)
{
	u8 w_cnt;
	u32 r_cnt;
	u32 value = 0, read_data;
	u32 dly_time = 50;

	//读最高位为1
	addr &= 0x7F;/*7位有效地址*/
	addr = 0x80 | addr;

	rx1922_gpio_spi_begin(rfMux);
	delay_dx_rf(dly_time);
	//<使能>，低有效
	rf_rx1922_cs_level_mux(rfMux, RF_RX1922_CS_LOW);

	//判断待写入数据是0还是1
	for (w_cnt = 0; w_cnt < 8; w_cnt++)
	{
		//<时钟>,拉低
		rf_rx1922_spi_clk_low();

		//判断待写入数据是0还是1
		if (addr & 0x80)
		{
			//待写入数据为1，将MOSI拉高
			rf_rx1922_spi_mosi_high();
		}
		else
		{
			//待写入数据为0，将MOSI拉低
			rf_rx1922_spi_mosi_low();
		}
		delay_dx_rf(dly_time);

		//待写入数据移至下一位
		addr <<= 1;

		//<时钟>,拉高
		rf_rx1922_spi_clk_high();
		delay_dx_rf(dly_time);
	}

	for (r_cnt = 24; r_cnt > 0; r_cnt--)
	{
		//<时钟>,拉低
		rf_rx1922_spi_clk_low();

		delay_dx_rf(dly_time);

		//从SDI中读出当前bit数据
		read_data = rf_rx1922_spi_miso_read();

		//将读出的数据移位与之前读出的数据进行拼接
		read_data = read_data << (r_cnt - 1);

		value = value | read_data;

		//<时钟>,拉高
		rf_rx1922_spi_clk_high();

		delay_dx_rf(dly_time);

	}
	//<时钟>,拉低
	rf_rx1922_spi_clk_low();
	//读完成，<使能>拉高
	rf_rx1922_cs_level_mux(rfMux, RF_RX1922_CS_HIGH);
	delay_dx_rf(dly_time);
	rx1922_gpio_spi_end(rfMux);
	delay_dx_rf(dly_time);
	return (value & 0xFFFFFF);
}

void rx1922_write_reg(u32 addr, u32 val)
{
	rf_rx1922_gpio_spi_write_reg(RF_RX1922_C1, addr, val);
}

u32 rx1922_read_reg(u32 addr)
{
	return rf_rx1922_gpio_spi_read_reg(RF_RX1922_C1, addr);
}

void rx1922_init(void)
{
	u32 i, temp;
	if (HW_KGR_LC_B1 == g_KgrFreqId)
	{
		dx_kprintf(" KGR_B1I_B1C_L1_M 1490--------------------------------------------------------------<<<<<<<<<<<<<<<<<<<\r\n");
		for (i = 0; i < sizeof(IC0_B1_RX1922Regs_1490) / sizeof(struct rf_rx1922_config); i++)
		{
			rx1922_write_reg(IC0_B1_RX1922Regs_1490[i].RegAddr, IC0_B1_RX1922Regs_1490[i].RegData);
			delay_dx_rf(10);
			temp = rx1922_read_reg(IC0_B1_RX1922Regs_1490[i].RegAddr);

			if (IC0_B1_RX1922Regs_1490[i].RegData != temp)
				dx_kprintf(" KGR_B1I_B1C_L1_M rx1922: reg:%d write 0x%x read 0x%x\r\n", IC0_B1_RX1922Regs_1490[i].RegAddr, IC0_B1_RX1922Regs_1490[i].RegData, temp);
		}


	}

}

void rx1922_open_chn(u16 chnno)
{
	u32 value = 0;
	value = rx1922_read_reg(RX1922_REG0_ADDR);
	//根据通道，对Reg0#的相应通道配置
	switch (chnno)
	{
		case RF_RX1922_CH1:
		{
			value |= (0x1 << 10);
			rx1922_write_reg(RX1922_REG0_ADDR, value);
			delay_dx_rf(10);
			break;
		}
		case RF_RX1922_CH2:
		{
			value |= (0x1 << 9);
			rx1922_write_reg(RX1922_REG0_ADDR, value);
			delay_dx_rf(10);
			break;
		}
		case RF_RX1922_CH3:
		{
			value |= (0x1 << 8);
			rx1922_write_reg(RX1922_REG0_ADDR, value);
			delay_dx_rf(10);
			break;
		}
		case RF_RX1922_CH4:
		{
			value |= (0x1 << 7);
			rx1922_write_reg(RX1922_REG0_ADDR, value);
			delay_dx_rf(10);
			break;
		}
		default:
			break;
	}
}

void rx1922_close_chn(u16 chnno)
{//return 0;
	u32 value = 0;
	value = rx1922_read_reg(RX1922_REG0_ADDR);
	//根据通道，对Reg0#的相应通道配置
	switch (chnno)
	{
		case RF_RX1922_CH1:
		{
			value &= ~(0x1 << 10);
			rx1922_write_reg(RX1922_REG0_ADDR, value);
			delay_dx_rf(10);
			break;
		}
		case RF_RX1922_CH2:
		{
			value &= ~(0x1 << 9);
			rx1922_write_reg(RX1922_REG0_ADDR, value);
			delay_dx_rf(10);
			break;
		}
		case RF_RX1922_CH3:
		{
			value &= ~(0x1 << 8);
			rx1922_write_reg(RX1922_REG0_ADDR, value);
			delay_dx_rf(10);
			break;
		}
		case RF_RX1922_CH4:
		{
			value &= ~(0x1 << 7);
			rx1922_write_reg(RX1922_REG0_ADDR, value);
			delay_dx_rf(10);
			break;
		}
		default:
			break;
	}
}


int rx1922_Set_GaindB(u16 chcn,float gainVal)
{
	u32 value = 0;
	float retValf32 = 0;
	u32 setVal = 0;

	if(chcn > 3)
		return -1;

	if(gainVal < 5)
		return -1;
	
	
	value = rx1922_read_reg(RX1922_REG3_ADDR);

	retValf32 = gainVal - 5;
	

	retValf32 = retValf32/0.5;

	setVal = retValf32;

	if(setVal > 59)
	{
		return -2;
	}
	else
	{
		
		value &= ~(0x3F << (3-chcn)*6); //将对应位置数清零；  
		value = value |((setVal) << (3-chcn)*6);
		rx1922_write_reg(RX1922_REG3_ADDR, value);
	
		return setVal;
	}
	
	
}


float rx1922_Get_GaindB(u16 chcn)
{
	u32 value = 0;
	float retValf32 = 0;
	u32 setVal = 0;

	if(chcn > 3)
		return -1;
	
	value = rx1922_read_reg(RX1922_REG3_ADDR);

	value = (value>> (3-chcn)*6)&0x3F;

	retValf32 = value*0.5+5;

	return retValf32;	
	
}




