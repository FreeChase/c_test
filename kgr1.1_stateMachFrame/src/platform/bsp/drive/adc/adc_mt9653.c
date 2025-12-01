/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc_mt9653.c
  * @brief   This file provides code for the configuration
  *          of the SPI RF MT9653 instances.
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
#include <adc_mt9653.h>
#include <stdio.h>

/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "dxdef.h"


#define MT9653_GPIO_SPI_CS0			(0)
#define MT9653_GPIO_SPI_CLK			(1)
#define MT9653_GPIO_SPI_SDIO		(2)

void adc_dx_dly_us(unsigned int tmpVal)
{
	for(int i = 0 ; i < tmpVal*1000; i++);
}

/* Private variables ---------------------------------------------------------*/

/*****************************************************************************************************
*	函 数 名: static void mt9653_gpio_spi_begin(eAdcMT9653ChipMux adcMT9653Mux)
*	功能说明: 模拟SPI总线前，将硬件SPI配置为MT9653_GPIO
*	形	  参:
*            eAdcMT9653ChipMux adcMT9653Mux: 选择mt9653芯片
*	返 回 值: 无
******************************************************************************************************/
static void mt9653_gpio_spi_begin(eAdcMT9653ChipMux adcmt9653Mux)
{
	PERI_DX_PIN_OUTPUT_MODE(MT9653_GPIO_SPI_CLK);
	PERI_DX_PIN_OUTPUT_MODE(MT9653_GPIO_SPI_SDIO);

	PERI_DX_PIN_OUPUT_LOW(MT9653_GPIO_SPI_CLK);
	PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_SDIO);

	switch (adcmt9653Mux)
	{
		case ADC_MT9653_IC1:
		{
			PERI_DX_PIN_OUTPUT_MODE(MT9653_GPIO_SPI_CS0);
			PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_CS0);
		}
		break;
		case ADC_MT9653_IC2:
		{

		}
		break;
		default:
			break;
	}
}

/*****************************************************************************************************
*	函 数 名: static void mt9653_gpio_spi_end(eAdcmt9653ChipMux adcmt9653Mux)
*	功能说明: 模拟SPI处理完后，将mt9653_GPIO配置为硬件SPI模式
*	形	  参:
*            eAdcmt9653ChipMux adcmt9653Mux: 选择mt9653芯片
*	返 回 值: 无
******************************************************************************************************/
static void mt9653_gpio_spi_end(eAdcMT9653ChipMux adcmt9653Mux)
{
	PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_CLK);
	PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_SDIO);

	switch (adcmt9653Mux)
	{
		case ADC_MT9653_IC1:
		{
			PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_CS0);
		}
		break;
		case ADC_MT9653_IC2:
		{

		}
		break;
		default:
			break;
	}
}

/*****************************************************************************************************
*	函 数 名: static void adc_mt9653_cs_level_mux(eAdcMT9653ChipMux adcMT9653Mux,
*													eAdcMT9653CsLevelMux adcMT9653CsLevelMux)
*	功能说明: 选择mt9653芯片
*	形	  参:
*			  eAdcMT9653ChipMux adcMT9653Mux: 选择mt9653芯片
*			  eAdcMT9653CsLevelMux adcMT9653CsLevelMux: 选择mt9653芯片选高低
*	返 回 值: 无
******************************************************************************************************/
static void adc_mt9653_cs_level_mux(eAdcMT9653ChipMux adcMT9653Mux,
										eAdcMT9653CsLevelMux adcMT9653CsLevelMux)
{
	if (ADC_MT9653_CS_LOW == adcMT9653CsLevelMux)
	{
		switch (adcMT9653Mux)
		{
			case ADC_MT9653_IC1:
			{
				PERI_DX_PIN_OUPUT_LOW(MT9653_GPIO_SPI_CS0);
			}
			break;
			case ADC_MT9653_IC2:
			{

			}
			break;
			default:
				break;
		}
	}
	else
	{
		switch (adcMT9653Mux)
		{
			case ADC_MT9653_IC1:
			{
				PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_CS0);
			}
			break;
			case ADC_MT9653_IC2:
			{

			}
			break;
			default:
				break;
		}
	}

}

/*****************************************************************************************************
*	函 数 名: static void adc_mt9653_gpio_spi_write_reg(eAdcMT9653ChipMux adcMT9653Mux,
															u16 addr, u8 val)
*	功能说明: mt9653模拟spi总线写数据
*	形	  参:
*			  eAdcMT9653ChipMux adcMT9653Mux: 选择mt9653芯片
*		      u16 addr: 寄存器地址
*             u8 val: 写数据
*	返 回 值: 无
******************************************************************************************************/
static void adc_mt9653_gpio_spi_write_reg(eAdcMT9653ChipMux adcMT9653Mux, u16 addr, u8 val)
{
	u16 TxCmd;
	u8 w_cnt;
	u32 dly_time = 1;

	addr &= 0x1FFF;/*13位有效地址*/
	val &= 0xFF;/*8位数据*/

	//写指令数据拼接，W最高位为低
	TxCmd = (0x0 << 15) | addr;

	mt9653_gpio_spi_begin(adcMT9653Mux);
	//<使能>，低有效
	adc_mt9653_cs_level_mux(adcMT9653Mux, ADC_MT9653_CS_LOW);
	adc_dx_dly_us(dly_time);

	//写指令
	for (w_cnt = 0; w_cnt < 16; w_cnt++)
	{
		//<时钟>,拉低
		PERI_DX_PIN_OUPUT_LOW(MT9653_GPIO_SPI_CLK);

		//判断待写入数据是0还是1
		if (TxCmd & 0x8000)
		{
			//待写入数据为1，将MOSI拉高
			PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_SDIO);
		}
		else
		{
			//待写入数据为0，将MOSI拉低
			PERI_DX_PIN_OUPUT_LOW(MT9653_GPIO_SPI_SDIO);
		}
		adc_dx_dly_us(dly_time);

		//待写入数据移至下一位
		TxCmd <<= 1;

		//<时钟>,拉高
		PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_CLK);
		adc_dx_dly_us(dly_time);
	}

	//写数据
	for (w_cnt = 0; w_cnt < 8; w_cnt++)
	{
		//<时钟>,拉低
		PERI_DX_PIN_OUPUT_LOW(MT9653_GPIO_SPI_CLK);

		//判断待写入数据是0还是1
		if (val & 0x80)
		{
			//待写入数据为1，将MOSI拉高
			PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_SDIO);
		}
		else
		{
			//待写入数据为0，将MOSI拉低
			PERI_DX_PIN_OUPUT_LOW(MT9653_GPIO_SPI_SDIO);
		}
		adc_dx_dly_us(dly_time);

		//待写入数据移至下一位
		val <<= 1;

		//<时钟>,拉高
		PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_CLK);
		adc_dx_dly_us(dly_time);
	}
	//<时钟>,拉低
	PERI_DX_PIN_OUPUT_LOW(MT9653_GPIO_SPI_CLK);
	//写完成，<使能>拉高
	adc_mt9653_cs_level_mux(adcMT9653Mux, ADC_MT9653_CS_HIGH);
	adc_dx_dly_us(dly_time);
	mt9653_gpio_spi_end(adcMT9653Mux);
}

/*****************************************************************************************************
*	函 数 名: static u32 adc_mt9653_gpio_spi_read_reg(eAdcMT9653ChipMux adcMT9653Mux, u16 addr)
*	功能说明: mt9653模拟spi总线写数据
*	形	  参:
*			  eAdcMT9653ChipMux adcMT9653Mux: 选择mt9653芯片
*		      u16 addr: 寄存器地址
*	返 回 值: 寄存器的值
******************************************************************************************************/
static u8 adc_mt9653_gpio_spi_read_reg(eAdcMT9653ChipMux adcMT9653Mux, u16 addr)
{
	u8 r_cnt;
	u8 w_cnt;
	u16 TxCmd;
	u8 value = 0, read_data;
	u32 dly_time = 1;
	
	//读最高位为1
	addr &= 0x1FFF;/*13位有效地址*/

	TxCmd = 0x8000 | addr;

	mt9653_gpio_spi_begin(adcMT9653Mux);
	//<使能>，低有效
	adc_mt9653_cs_level_mux(adcMT9653Mux, ADC_MT9653_CS_LOW);

	//判断待写入数据是0还是1
	for (w_cnt = 0; w_cnt < 16; w_cnt++)
	{
		//<时钟>,拉低
		PERI_DX_PIN_OUPUT_LOW(MT9653_GPIO_SPI_CLK);

		//判断待写入数据是0还是1
		if (TxCmd & 0x8000)
		{
			//待写入数据为1，将MOSI拉高
			PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_SDIO);
		}
		else
		{
			//待写入数据为0，将MOSI拉低
			PERI_DX_PIN_OUPUT_LOW(MT9653_GPIO_SPI_SDIO);
		}
		adc_dx_dly_us(dly_time);

		//待写入数据移至下一位
		TxCmd <<= 1;

		//<时钟>,拉高
		PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_CLK);
		adc_dx_dly_us(dly_time);
	}

	//SDIO输入模式
	PERI_DX_PIN_INTPUT_MODE(MT9653_GPIO_SPI_SDIO);
	for (r_cnt = 8; r_cnt > 0; r_cnt--)
	{
		//<时钟>,拉低
		PERI_DX_PIN_OUPUT_LOW(MT9653_GPIO_SPI_CLK);

		adc_dx_dly_us(dly_time);

		//从SDO中读出当前bit数据
		read_data = PERI_DX_PIN_INTPUT(MT9653_GPIO_SPI_SDIO);

		//将读出的数据移位与之前读出的数据进行拼接
		read_data = read_data << (r_cnt - 1);

		value = value | read_data;

		//<时钟>,拉高
		PERI_DX_PIN_OUPUT_HIGH(MT9653_GPIO_SPI_CLK);

		adc_dx_dly_us(dly_time);

	}
	//<时钟>,拉低
	PERI_DX_PIN_OUPUT_LOW(MT9653_GPIO_SPI_CLK);
	//读完成，<使能>拉高
	adc_mt9653_cs_level_mux(adcMT9653Mux, ADC_MT9653_CS_HIGH);
	adc_dx_dly_us(dly_time);
	mt9653_gpio_spi_end(adcMT9653Mux);
	return (value & 0xFF);
}

void mt9653_ic0_write_reg(u16 addr, u8 val)
{
	adc_mt9653_gpio_spi_write_reg(ADC_MT9653_IC1, addr, val);
}

u8 mt9653_ic0_read_reg(u16 addr)
{
	return adc_mt9653_gpio_spi_read_reg(ADC_MT9653_IC1, addr);
}

int dx_hw_mt9653_ic0_init(void)
{
	u8 chip_id;
	u8 chip_sample_mode;
	chip_id = mt9653_ic0_read_reg(MT9653_CHIPID_REG);
	chip_sample_mode = mt9653_ic0_read_reg(0x21);
	mt9653_ic0_write_reg(0x21, 0x30);
	chip_sample_mode = mt9653_ic0_read_reg(0x21);

	dx_kprintf("LC KGR ADC USE MT9653 DEV ID [%x] \r\n",chip_id);
	mt9653_ic0_write_reg(0x0, 0x38);
	mt9653_ic0_write_reg(0x0, 0x18);
	mt9653_ic0_write_reg(0xeb, 0x13);
	mt9653_ic0_write_reg(0x92, 0xfa);
	mt9653_ic0_write_reg(0x92, 0xf8);
	adc_dx_dly_us(500000);//自校准时间500ms

	return DX_EOK;
}

void mt9653_ic1_write_reg(u16 addr, u8 val)
{
	adc_mt9653_gpio_spi_write_reg(ADC_MT9653_IC2, addr, val);
}

u8 mt9653_ic1_read_reg(u16 addr)
{
	return adc_mt9653_gpio_spi_read_reg(ADC_MT9653_IC2, addr);
}

int dx_hw_mt9653_ic1_init(void)
{
	u8 chip_id;
	u8 chip_sample_mode;
	chip_id = mt9653_ic1_read_reg(MT9653_CHIPID_REG);
	chip_sample_mode = mt9653_ic1_read_reg(0x21);
	mt9653_ic1_write_reg(0x21, 0x30);
	chip_sample_mode = mt9653_ic1_read_reg(0x21);
	if ( (HW_KGR_B1_VER_4_54 == g_HwModuleVer) | (HW_KGR_B3_VER_4_54 == g_HwModuleVer) | (HW_KGR_S_VER_4_54 == g_HwModuleVer) )//B3
	{
		mt9653_ic1_write_reg(0x0, 0x38);
		mt9653_ic1_write_reg(0x0, 0x18);
		mt9653_ic1_write_reg(0xeb, 0x13);
		mt9653_ic1_write_reg(0x92, 0xfa);
		mt9653_ic1_write_reg(0x92, 0xf8);
		adc_dx_dly_us(500000);//自校准时间500ms
	}
#if 0
	/*关闭A通道VCM环路检测功能*/
	mt9653_ic1_write_reg(MT9653_CHANNEL_INDEX_REG, 0x01);
	mt9653_ic1_write_reg(MT9653_TRANSMIT_REG, 0x01);
	mt9653_ic1_write_reg(MT9653_POWER_MODE_REG, 0x01);
	mt9653_ic1_write_reg(MT9653_TRANSMIT_REG, 0x01);
#endif
	/*关闭VCM环路检测功能*/
	/*mt9653_ic1_write_reg(0x31, 0x41);
	mt9653_ic1_write_reg(MT9653_TRANSMIT_REG, 0x01);
	mt9653_ic1_write_reg(0x3A, 0x00);
	mt9653_ic1_write_reg(MT9653_TRANSMIT_REG, 0x01);
	mt9653_ic1_write_reg(0x31, 0x00);
	mt9653_ic1_write_reg(MT9653_TRANSMIT_REG, 0x01);*/
	/*MT9653底噪调节*/

	xil_printf("MT9653 IC1 ID: 0x%04x chip sample mode:0x%X\r\n", chip_id, chip_sample_mode);
	return DX_EOK;
}

int dx_hw_mt9653_test_mode_on(void)
{
//	if ( (HW_KGR_B1_VER_4_54 == g_HwModuleVer) | (HW_KGR_B3_VER_4_54 == g_HwModuleVer) | (HW_KGR_S_VER_4_54 == g_HwModuleVer) )//B3
//	{
	mt9653_ic0_write_reg(0x0D,0x04);//0x04= 0xAAAA<-->0x5555
	mt9653_ic1_write_reg(0x0D,0x04);//0x04= 0xAAAA<-->0x5555
//	}
//	else
//	{
//	/*ADC 1 测试模式 ，寄存器配置来自贝岭技术支持*/
//	mt9653_ic0_write_reg(0x31,0x41);
//	mt9653_ic0_write_reg(0xFF,0x01);
//	mt9653_ic0_write_reg(0x6C,0x01);
//	mt9653_ic0_write_reg(0xFF,0x01);
//	mt9653_ic0_write_reg(0x0D,0x06);//0x06= 0xAAAA<-->0x5555
//	mt9653_ic0_write_reg(0xFF,0x01);
//
//	/*ADC 2 测试模式 ，寄存器配置来自贝岭技术支持*/
//	mt9653_ic1_write_reg(0x31,0x41);
//	mt9653_ic1_write_reg(0xFF,0x01);
//	mt9653_ic1_write_reg(0x6C,0x01);
//	mt9653_ic1_write_reg(0xFF,0x01);
//	mt9653_ic1_write_reg(0x0D,0x06);//0x06= 0xAAAA<-->0x5555
//	mt9653_ic1_write_reg(0xFF,0x01);
//	}
	dx_kprintf("adc test mode on\r\n");
	return DX_EOK;
}

int dx_hw_mt9653_test_mode_off(void)
{
//	if ( (HW_KGR_B1_VER_4_54 == g_HwModuleVer) | (HW_KGR_B3_VER_4_54 == g_HwModuleVer) | (HW_KGR_S_VER_4_54 == g_HwModuleVer) )//B3
//	{
		mt9653_ic0_write_reg(0x0D,0x00);//0x04= 0xAAAA<-->0x5555
		mt9653_ic1_write_reg(0x0D,0x00);//0x04= 0xAAAA<-->0x5555
//	}
//	else
//	{
//	/*ADC 1关闭 测试模式 ，寄存器配置来自贝岭技术支持*/
//	mt9653_ic0_write_reg(0x0D,0x00);
//	mt9653_ic0_write_reg(0x6C,0x00);
//	mt9653_ic0_write_reg(0x31,0x00);
//	mt9653_ic0_write_reg(0xFF,0x01);
//
//	/*ADC 2关闭 测试模式 ，寄存器配置来自贝岭技术支持*/
//	mt9653_ic1_write_reg(0x0D,0x00);
//	mt9653_ic1_write_reg(0x6C,0x00);
//	mt9653_ic1_write_reg(0x31,0x00);
//	mt9653_ic1_write_reg(0xFF,0x01);
//    }
	dx_kprintf("adc test mode off\r\n");
	return DX_EOK;
}

int dx_hw_mt9653_ic1_lowpower_mode_on(void)
{
	mt9653_ic1_write_reg(0x5,0x3F);
	mt9653_ic1_write_reg(0x08,0x01);
    mt9653_ic1_write_reg(0xFF,0x01);
    adc_dx_dly_us(500);
    mt9653_ic1_write_reg(0x08,0x01);
    mt9653_ic1_write_reg(0xFF,0x01);
    dx_kprintf("adc1 lowpower mode on\r\n");
}

int dx_hw_mt9653_ic1_lowpower_mode_off(void)
{
	mt9653_ic1_write_reg(0x5,0x3F);
	mt9653_ic1_write_reg(0x08,0x00);
    mt9653_ic1_write_reg(0xFF,0x01);
    adc_dx_dly_us(500);
    mt9653_ic1_write_reg(0x08,0x00);
    mt9653_ic1_write_reg(0xFF,0x01);
    dx_kprintf("adc1 lowpower mode off\r\n");
}

int dx_hw_mt9653_remove_dc_offset(void)
{
	//s16 adc_dc_offset[8]={-360,-215,-230,-270,-270,-200,-180,-310};  //调节前直流功率
	s16 adc_dc_offset[4]={-350,-200,-210,-210};  //调节前直流功率
	mt9653_ic0_write_reg(0x380,0); //通道A[7:0]
	mt9653_ic0_write_reg(0x400,0); //通道B[7:0]
	mt9653_ic0_write_reg(0x401,0); //通道B[5:0]
	mt9653_ic0_write_reg(0x480,0); //通道C[7:0]
	mt9653_ic0_write_reg(0x481,0); //通道C[5:0]
	mt9653_ic0_write_reg(0x500,0); //通道D[7:0]
	mt9653_ic0_write_reg(0x501,0); //通道D[5:0]
	mt9653_ic0_write_reg(0x380,((adc_dc_offset[3]*4)&0xff)); //通道A[7:0]
	mt9653_ic0_write_reg(0x381,((adc_dc_offset[3]*4)>>8  )); //通道A[5:0]
	mt9653_ic0_write_reg(0x400,((adc_dc_offset[2]*4)&0xff)); //通道A[7:0]
	mt9653_ic0_write_reg(0x401,((adc_dc_offset[2]*4)>>8  )); //通道A[5:0]
	mt9653_ic0_write_reg(0x480,((adc_dc_offset[1]*4)&0xff)); //通道A[7:0]
	mt9653_ic0_write_reg(0x481,((adc_dc_offset[1]*4)>>8  )); //通道A[5:0]
	mt9653_ic0_write_reg(0x500,((adc_dc_offset[0]*4)&0xff)); //通道A[7:0]
	mt9653_ic0_write_reg(0x501,((adc_dc_offset[0]*4)>>8  )); //通道A[5:0]
//	mt9653_ic1_write_reg(0x380,((adc_dc_offset[7]*4)&0xff)); //通道A[7:0]
//	mt9653_ic1_write_reg(0x381,((adc_dc_offset[7]*4)>>8  )); //通道A[5:0]
//	mt9653_ic1_write_reg(0x400,((adc_dc_offset[6]*4)&0xff)); //通道A[7:0]
//	mt9653_ic1_write_reg(0x401,((adc_dc_offset[6]*4)>>8  )); //通道A[5:0]
//	mt9653_ic1_write_reg(0x480,((adc_dc_offset[5]*4)&0xff)); //通道A[7:0]
//	mt9653_ic1_write_reg(0x481,((adc_dc_offset[5]*4)>>8  )); //通道A[5:0]
//	mt9653_ic1_write_reg(0x500,((adc_dc_offset[4]*4)&0xff)); //通道A[7:0]
//	mt9653_ic1_write_reg(0x501,((adc_dc_offset[4]*4)>>8  )); //通道A[5:0]

}

int dx_hw_mt9653_delay_init(void)
{
    /*延迟级数初始化*/
	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_LD,0xfffff);
	adc_dx_dly_us(1);
	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_LD,0x00);
	adc_dx_dly_us(1);
	return DX_EOK;
}

int dx_hw_mt9653_delay_inc_init(void)
{
    /*延迟级数初始化*/
	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_INC,0xfffff);
	return DX_EOK;
}

u32 dx_hw_mt9653_dco_delay_cntvalue_get(void)
{
    /*两通道一起调节*/
	u32 dco_cntvalue;
	dco_cntvalue = mcu_to_fpga_read_kgr_reg(ADC_DCO_DELAY_VALUE0);
	return dco_cntvalue&0x3ff;
}

u32 dx_hw_mt9653_dco_delay_cntvalue_step(void)
{
    /*两通道一起调节*/
	int dco_cntvalue[2];
	dco_cntvalue[0]=dx_hw_mt9653_dco_delay_cntvalue_get();
	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_CE, 0x3);
	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_CE, 0);
	dco_cntvalue[1]=dx_hw_mt9653_dco_delay_cntvalue_get();
	dx_kprintf("adc1_dco value old = %d, adc1_dco value new= %d\r\n",dco_cntvalue[0]&0x1f,dco_cntvalue[1]&0x1f);
}

u32 dx_hw_mt9653_dco_samdata_check(void)
{
    /*8通道比对*/
	u32 u32SamCtrlReg;
    u32 u32AdcData;
    //adc同步通道分配排列0~7

    //采集配置，启动
    mcu_to_fpga_write_kgr_reg(100, (0<<16)|(3<<12)|(0<<8)|(0<<4)|1);
    /*                                   2:4ch 3:8ch*/
    mcu_to_fpga_write_kgr_reg(100, (0<<16)|(3<<12)|(0<<8)|(0<<4)|0);
    //等待采集完成
    u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    while((u32SamCtrlReg &((u32)(1)<<21)))
    {
    	adc_dx_dly_us(1);
        u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    }
    //校验
    for(int i=0;i<1024*4;i++)
    {
    	u32AdcData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i);
    	if((u32AdcData!=0x55555555) && (u32AdcData!=0xaaaaaaaa))
    	{
    		dx_kprintf("adc_data_check failed ,error vlaue is  0x%x\r\n",u32AdcData);
    		return 0;
    	}
    }
    //校验成功

    dx_kprintf("adc_data_check success\r\n");
    return 1;
}

u32 dx_hw_mt9653_dco_samdata_adjust(void)
{
	u32 u32CheckOK;
	u32 u32CheckStatus = 0;

	for (int i = 0; i<32; i=i+1 )
	{
		dx_kprintf("[%d] check result is \r\n",i);
		dx_hw_mt9653_dco_delay_cntvalue_step();
		u32CheckOK = dx_hw_mt9653_dco_samdata_check();
		u32CheckStatus = u32CheckStatus | (u32CheckOK << i);
	}
	 dx_kprintf("adc_data_check successful vlaue is = 0x%x \r\n",u32CheckStatus);
	 return u32CheckStatus;
}

u32 dx_hw_mt9653_dco_cntvalue_out(void)
{
	u32 u32CheckStatus=0;
	u32 delay_successful_legnth=0;
	u32 delay_successful_legnth_max=0;
	u32 delay_successful_legnth_index=0;
	u32 delay_successful_legnth_index_max=0;
	u32 delay_out_cntvalue = 0;
	u32 retval;
	u32CheckStatus = dx_hw_mt9653_dco_samdata_adjust();
	for(int i=0;i<32;i++)
	{
		if((u32CheckStatus>>i)&0x1)
		{
			delay_successful_legnth = delay_successful_legnth + 1;
			delay_successful_legnth_index =i;
			if((delay_successful_legnth>4) && (delay_successful_legnth>delay_successful_legnth_max))
			{
				delay_successful_legnth_max = delay_successful_legnth;
				delay_successful_legnth_index_max = delay_successful_legnth_index;
			}
			else
			{
				delay_successful_legnth_max = delay_successful_legnth_max;
				delay_successful_legnth_index_max = delay_successful_legnth_index_max;
			}
		}
		else
		{
			delay_successful_legnth = 0;
			delay_successful_legnth_index = 0;
		}
	}
	delay_out_cntvalue = delay_successful_legnth_index_max - (delay_successful_legnth_max/2);
	dx_kprintf("adc_data_check best successful index = %d \r\n",delay_out_cntvalue);
    for(int i=0;i<=delay_out_cntvalue;i++)
    {
    	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_CE, 0x3);
        mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_CE, 0);
    }
    dx_kprintf("adc_dco successful cntvalue is = %d \r\n",(dx_hw_mt9653_dco_delay_cntvalue_get()&0x1f));

    retval = dx_hw_mt9653_dco_samdata_check();

	return retval;
}
u32 adc_adjust_get_dcofcodlyvalue( u32 u32Chan )//0->dco0 1->dco1 2->fco0 3->fco1
{
	u32 cnt_value;
	cnt_value = mcu_to_fpga_read_kgr_reg( ADC_DCO_DELAY_VALUE0 ) >> (5*u32Chan);

	return cnt_value&0x1f;
}

u32 adc_adjust_dcofcostep( u32 u32Chan )//0->dco0 1->dco1 2->fco0 3->fco1
{
	int cnt_value[2];

	cnt_value[0] = adc_adjust_get_dcofcodlyvalue( u32Chan );

	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_CE, ( 1 << u32Chan ));
	//dx_dly_us(1);
	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_CE, 0);

	cnt_value[1] = adc_adjust_get_dcofcodlyvalue( u32Chan );

	dx_kprintf("adc_adjust_step u32Chan = %d, old=%d new=%d\r\n",u32Chan, cnt_value[0] ,cnt_value[1] );

}


u32 adc_adjust_fcocheck( u32 u32Chan )//0->dco0 1->dco1 2->fco0 3->fco1
{
	u32 u32SamCtrlReg;
	u32 u32AdcData;
	u32 u32Samchan;
	u32 u32AdcSwicth;

	if( u32Chan == 2)
	{
		u32Samchan = 3;
	}
	else
	{
		u32Samchan = 7;
	}

	u32AdcSwicth = mcu_to_fpga_read_kgr_reg(32);
	u32AdcSwicth = u32AdcSwicth | 0x8;
	mcu_to_fpga_write_kgr_reg(32, u32AdcSwicth );

	mcu_to_fpga_write_kgr_reg(100, (u32Samchan<<16)|(0<<12)|(0<<8)|(0<<4)|1 );
	//dx_kprintf("SAM Addr:100, Read Value:0x%x\r\n",  mcu_to_fpga_read_kgr_reg(100));
    mcu_to_fpga_write_kgr_reg(100, (u32Samchan<<16)|(0<<12)|(0<<8)|(0<<4)|0);
    //dx_kprintf("ADC DATA WRITE STAR\r\nT");
	u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    while((u32SamCtrlReg &((u32)(1)<<20)))
    {
    	adc_dx_dly_us(1);
    	u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    }
    //dx_kprintf("ADC DATA WRITE DONE\r\n");

    for(int i=0;i<1024*4;i++){
    	u32AdcData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i);
    	if( (u32AdcData != 0x00f000f0) ){
    		dx_kprintf("adc_adjust_check failed [%d]:%d 0x%x \r\n",u32Chan, i, u32AdcData);
    		return 0;
    	}
    	//dx_kprintf("adc_adjust_check data[%d]:%d 0x%x \r\n",u32AdcChan, i, u32AdcData);
    }

	mcu_to_fpga_write_kgr_reg(32, u32AdcSwicth&0x7 );

    //dx_kprintf("adc_adjust_check succed [%d]: 0x%x \r\n",u32AdcChan, u32AdcData );
    return 1;
}

u32 adc_adjust_get_dlyvalue( u32 u32AdcChan )
{
	u32 cnt_value;

	switch( u32AdcChan ){
		case 0:
			cnt_value = mcu_to_fpga_read_kgr_reg( ADC_DCO_DELAY_VALUE0 ) >> 20;
			break;
		case 1:
			cnt_value = mcu_to_fpga_read_kgr_reg( ADC_DCO_DELAY_VALUE1 );
			break;
		case 2:
			cnt_value = mcu_to_fpga_read_kgr_reg( ADC_DCO_DELAY_VALUE1 ) >> 10;
			break;
		case 3:
			cnt_value = mcu_to_fpga_read_kgr_reg( ADC_DCO_DELAY_VALUE1 ) >> 20;
			break;
		case 4:
			cnt_value = mcu_to_fpga_read_kgr_reg( ADC_DCO_DELAY_VALUE2 );
			break;
		case 5:
			cnt_value = mcu_to_fpga_read_kgr_reg( ADC_DCO_DELAY_VALUE2 ) >> 10;
			break;
		case 6:
			cnt_value = mcu_to_fpga_read_kgr_reg( ADC_DCO_DELAY_VALUE2 ) >> 20;
			break;
		case 7:
			cnt_value = mcu_to_fpga_read_kgr_reg( ADC_DCO_DELAY_VALUE3 );
			break;
		}
	return (cnt_value&0x3ff);
}

u32 adc_adjust_step( u32 u32AdcChan )
{
	int cnt_value[2];

	cnt_value[0] = adc_adjust_get_dlyvalue( u32AdcChan );

	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_CE, ( 1<< ((u32AdcChan+2)*2) ));
	//dx_dly_us(1);
	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_CE, 0);

	cnt_value[1] = adc_adjust_get_dlyvalue( u32AdcChan );

	dx_kprintf("adc_adjust_step adcchan = %d, old=%d new=%d\r\n",u32AdcChan, cnt_value[0] ,cnt_value[1] );
}


u32 adc_adjust_check( u32 u32AdcChan )
{
	u32 u32SamCtrlReg;
	u32 u32AdcData;
	mcu_to_fpga_write_kgr_reg(100, (u32AdcChan<<16)|(0<<12)|(0<<8)|(0<<4)|1 );
	//dx_kprintf("SAM Addr:100, Read Value:0x%x\r\n",  mcu_to_fpga_read_kgr_reg(100));
    mcu_to_fpga_write_kgr_reg(100, (u32AdcChan<<16)|(0<<12)|(0<<8)|(0<<4)|0);
    //dx_kprintf("ADC DATA WRITE STAR\r\nT");
	u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    while((u32SamCtrlReg &((u32)(1)<<20)))
    {
    	adc_dx_dly_us(1);
    	u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    }
    //dx_kprintf("ADC DATA WRITE DONE\r\n");

    for(int i=0;i<1024*4;i++){
    	u32AdcData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i);
    	if( (u32AdcData != 0xaaaa5555) && (u32AdcData != 0x5555aaaa) ){
    		dx_kprintf("adc_adjust_check failed [%d]:%d 0x%x \r\n",u32AdcChan, i, u32AdcData);
    		return 0;
    	}
    	//dx_kprintf("adc_adjust_check data[%d]:%d 0x%x \r\n",u32AdcChan, i, u32AdcData);
    }

    //dx_kprintf("adc_adjust_check succed [%d]: 0x%x \r\n",u32AdcChan, u32AdcData );
    return 1;
}



u32 adc_samdata_jz(u8 ch_val)
{
	u32 u32CheckOK;

	u32 u32CheckStatus = 0;

	for (int i = 0; i<32; i=i+1 )
	{
		adc_adjust_step( ch_val );
		u32CheckOK = adc_adjust_check( ch_val );
		u32CheckStatus = u32CheckStatus | (u32CheckOK << i);
	}
	 dx_kprintf("adc_samdata_jz[%d] = 0x%x \r\n",ch_val, u32CheckStatus);
	 return u32CheckStatus;
}

u32 adc_samdata_fcojz(u8 ch_val)
{
	u32 u32CheckOK;

	u32 u32CheckStatus = 0;

	for (int i = 0; i<32; i=i+1 )
	{
		adc_adjust_dcofcostep( ch_val+2 );
		u32CheckOK = adc_adjust_fcocheck( ch_val+2 );
		u32CheckStatus = u32CheckStatus | (u32CheckOK << i);
	}
	 dx_kprintf("adc_samdata_fcojz[%d] = 0x%x \r\n",ch_val, u32CheckStatus);
	 return u32CheckStatus;
}

int adc_fco_cntvalue_out(u8 ch_val)
{
	u32 u32CheckStatus=0;
	u32 NoZeroNum=0;
	u32CheckStatus =adc_samdata_fcojz(ch_val)&0xffff;
	for(int i=0;i<16;i++)
	{
		if((u32CheckStatus>>i)&0x1)
		{
			NoZeroNum = NoZeroNum+1;
		}
		else
		break;
	}
	dx_kprintf("adc_fco1_cntvalue_nozeronum[%d] = %d \r\n",ch_val,NoZeroNum);
	for(int i=0;i<(NoZeroNum/2 + 1);i++)
	{
		adc_adjust_dcofcostep( ch_val+2 );
	}
}

int adc_data_cntvalue_out(u8 ch_val)
{
	u32 u32CheckStatus=0;
	u32 NoZeroNum=0;
	u32CheckStatus =adc_samdata_jz(ch_val)&0xffff;
	for(int i=0;i<16;i++)
	{
		if((u32CheckStatus>>i)&0x1)
		{
			NoZeroNum = NoZeroNum+1;
		}
		else
		break;
	}
	dx_kprintf("adc_data_cntvalue_nozeronump[%d] = %d \r\n",ch_val,NoZeroNum);
	for(int i=0;i<(NoZeroNum/2 + 1);i++)
	{
		adc_adjust_step( ch_val+2 );
	}
	//return 1;
}



int dx_hw_mt9653_delay_value_adjust(void)
{
	u32 ret = 0;
	dx_hw_mt9653_test_mode_on();
    /*延迟级数初始化*/
	dx_hw_mt9653_delay_init();
	dx_hw_mt9653_delay_inc_init();
	/*调节dco和fco对齐*/
	//mcu_to_fpga_write_kgr_reg(32, ((1<<4)|0x0 ));

	ret = dx_hw_mt9653_dco_cntvalue_out();
	dx_hw_mt9653_test_mode_off();
    dx_hw_mt9653_remove_dc_offset();
	if(ret == 0)
	{
		return 1;
	}
	return DX_EOK;
}

int dx_hw_mt9653_init(void)
{
	/*ADC 初始化*/
	dx_hw_mt9653_ic0_init();

	//关掉ic1 的B、C、D路

	return DX_EOK;
}

u8 dx_hw_mt9653_self_test(void)
{
	if(MT9653_ID != mt9653_ic0_read_reg(MT9653_CHIPID_REG))
	{
		return 1;
	}
	if(MT9653_ID != mt9653_ic1_read_reg(MT9653_CHIPID_REG))
	{
		return 1;
	}
	return 0;
}

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
int adc_ctrl_chn(unsigned int adId,unsigned int chn,unsigned int ctlVal)
{
	if(adId == 0)
	{
		mt9653_ic0_write_reg(0x5,(chn|0x00)&(0xFF));
		mt9653_ic0_write_reg(0x22,ctlVal&(0x1));
		mt9653_ic0_write_reg(0x5,0x3F);
		
	}
	else
	{
		mt9653_ic1_write_reg(0x5,(chn|0x00)&(0xFF));
		mt9653_ic1_write_reg(0x22,ctlVal&(0x1));
		mt9653_ic1_write_reg(0x5,0x3F);
	}

	return 0;
}


