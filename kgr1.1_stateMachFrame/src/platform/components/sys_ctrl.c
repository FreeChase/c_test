/******************************************************************************
 * Copyright (C) 2024 ZS Development Team, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @file sys_ctrl.c
 * @brief
 * @version 0.1
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-25     Andy      first version
 * 2024-12-18     Andy      modify
******************************************************************************/
#include <stdio.h>
#include <xil_io.h>
#include "xparameters.h"
#include "platform.h"
#include "sys_ctrl.h"
#include "sleep.h"
#include "dxdef.h"

#include "antijam_fpga.h"


/* Private variables ---------------------------------------------------------*/
XGpio gPeriGpioSpi; /* The driver instance for GPIO  Device configured as SPI */
XGpio gCtrlGpio; /* The driver instance for GPIO  Device configured as SPI */
u8 u8KgrMode = 3;				/* 抗干扰模组模式控制字段：B3/B1/S模组模式 00b:低功耗;11b:抗干扰 */
u8 u8KgrStatus = 1;				/* B3/B1/S频点抗干扰指示:0:无干扰;1:有干扰 */
/*****************************************************************************/
/**
* System GPIO Initialize function .
*
* @param	None.
*
* @return
*		- XST_SUCCESS if successful
*		- XST_FAILURE if unsuccessful
*
* @note		None.
*
******************************************************************************/
int SystemGpioInit(void)
{
	int Status;
	/*
	 * Initialize the GPIO driver so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	 Status = XGpio_Initialize(&gPeriGpioSpi, GPIO_PERI_ID);
	 if (Status != XST_SUCCESS)  {
		  return XST_FAILURE;
	 }

	 /* Set the direction for all signals to be outputs */
	 XGpio_SetDataDirection(&gPeriGpioSpi, PERI_FUNCTION_CHANNEL, 0x0);

	 /* Set the GPIO outputs to low */
	 XGpio_DiscreteWrite(&gPeriGpioSpi, PERI_FUNCTION_CHANNEL, 0x0);

	 /*
	 * Initialize the GPIO driver so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	 Status = XGpio_Initialize(&gCtrlGpio, GPIO_CTRL_ID);
	 if (Status != XST_SUCCESS)  {
		 return XST_FAILURE;
	 }

	 /* Set the direction for all signals to be outputs */
	 XGpio_SetDataDirection(&gCtrlGpio, PERI_FUNCTION_CHANNEL, 0x0);

	 /* Set the GPIO outputs to low */
	 XGpio_DiscreteWrite(&gCtrlGpio, PERI_FUNCTION_CHANNEL, 0x0);



	return XST_SUCCESS;
}





/******************************************************************************/
/**
*
* This function disables the interrupts for the INTC
*
* @param	IntcInstancePtr is a pointer to the Interrupt Controller
*		driver Instance
* @param	IntrId is XPAR_<INTC_instance>_<GPIO_instance>_VEC
*		value from xparameters.h
*
* @return	None
*
* @note		None.
*
******************************************************************************/
void SystemIntrDisable(XIntc *IntcInstancePtr, u16 IntrId)
{
#ifdef XPAR_INTC_0_DEVICE_ID
	XIntc_Disable(IntcInstancePtr, IntrId);
#endif
	return;
}

/******************************************************************************/
/**
*
* This function enable the interrupts for the INTC
*
* @param	IntcInstancePtr is a pointer to the Interrupt Controller
*		driver Instance
* @param	IntrId is XPAR_<INTC_instance>_<GPIO_instance>_VEC
*		value from xparameters.h
*
* @return	None
*
* @note		None.
*
******************************************************************************/
void SystemIntrEnable(XIntc *IntcInstancePtr, u16 IntrId)
{
#ifdef XPAR_INTC_0_DEVICE_ID
	XIntc_Enable(IntcInstancePtr, IntrId);
#endif
	return;
}

/******************************************************************************/
/**
*
* This function performs the Soft Interrupt set up for Interrupts
*
* @param	IntcInstancePtr is a reference to the Interrupt Controller
*		driver Instance
* @param	DeviceId is the XPAR_<GPIO_instance>_DEVICE_ID value from
*		xparameters.h
* @param	IntrId is XPAR_<INTC_instance>_<Soft_Inter_instance>_IP2INTC_IRPT_INTR
*		value from xparameters.h
* @param	Handler to the handler for that interrupt.
*
* @return	XST_SUCCESS if the Test is successful, otherwise XST_FAILURE
*
* @note		None.
*
******************************************************************************/
int SystemSetupSoftIntr(XIntc *IntcInstancePtr, u16 IntrId, XInterruptHandler Handler)
{
	int Result;

	/* Hook up interrupt service routine */
	XIntc_Connect(IntcInstancePtr, IntrId,
			  Handler, NULL);

	/* Enable the interrupt vector at the interrupt controller */
	XIntc_Enable(IntcInstancePtr, IntrId);

	/*
	 * Start the interrupt controller such that interrupts are recognized
	 * and handled by the processor
	 */
	Result = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Result != XST_SUCCESS) {
		return Result;
	}
	/* Enable non-critical exceptions */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

/****************************************************************************************
* 函数原型: int SystemGetHwVer(void)
* 功能描述: 获取当前硬件模块实际版本
* 参数:
*   无
* 返回:
*   当前硬件模块版本号
****************************************************************************************/
u8 SystemGetHwVer(void)
{
	u8 curHwvs = 0xf;
    /**KGR_HWVS_D0_PORT~KGR_HWVS_D2_PORT配置为GPIO输入**/
#if 0
	DX_PIN_INTPUT_MODE(KGR_HWVS_D0_PORT);
	DX_PIN_INTPUT_MODE(KGR_HWVS_D1_PORT);
	DX_PIN_INTPUT_MODE(KGR_HWVS_D2_PORT);
    curHwvs = 0;
    curHwvs |= (DX_PIN_INTPUT(KGR_HWVS_D0_PORT) << 2);
    curHwvs |= (DX_PIN_INTPUT(KGR_HWVS_D1_PORT) << 1);
    curHwvs |= (DX_PIN_INTPUT(KGR_HWVS_D2_PORT) << 0);
#endif
    return curHwvs;
}


u8 enableUblox()
{
	writePeriGpioVal(KGR_GPIO_UBLOX_RSTN,1);

	return 0;
}


u8 enableBD21()
{
	writeCtrlGpioVal(BD21_CTRL_HW_RESET,1);

	return 0;
}
extern void dx_kprintf(const char *fmt, ...);
u8 uartThroughSelect(u8 val)
{
	char table[4][3] ={
		{0,0,0},	//*0-mcu-0
		{1,0,0},	//*1-ublox-1
		{0,1,0},	//*2-bd21_0-2
		{1,1,0},	//*3-bd21_0-3
	} ;

	if(val > 3)
	{
		return 1;
	}
	for (size_t i = 0; i < 3; i++)
	{
		dx_kprintf("%d gpip %d value %d\r\n",i,KGR_CTRL_UART_REMAP_BASE + i ,table[val][i]);
	}
	for (size_t i = 0; i < 3; i++)
	{
		writeCtrlGpioVal(KGR_CTRL_UART_REMAP_BASE + i, table[val][i]);
	}
	

	return 0;
}

uint32_t tick_delay_ms(uint32_t ms);

void uartDelayRemap(u8 val,u32 delay_ms)
{
	dx_kprintf("Remap %d Delay %d ms\r\n",val ,delay_ms);
	tick_delay_ms(delay_ms);
	uartThroughSelect(val);
	tick_delay_ms(100);
	writeCtrlGpioVal(BD21_CTRL_HW_RESET,0);
	tick_delay_ms(100);
	writeCtrlGpioVal(BD21_CTRL_HW_RESET,1);
}



u8 readPeriGpioVal(u8 gpioPIn)
{
	u8 ret = 0;
	PERI_DX_PIN_INTPUT_MODE(gpioPIn);
	ret = PERI_DX_PIN_INTPUT(gpioPIn);

	return ret;

}

u8 writePeriGpioVal(u8 gpioPin ,u8 valtmp)
{
	u8 ret = 0;
	PERI_DX_PIN_OUTPUT_MODE(KGR_GPIO_UBLOX_RSTN);
	if(valtmp == 1)
	{
		PERI_DX_PIN_OUPUT_HIGH(gpioPin);
	}
	else
	{
		PERI_DX_PIN_OUPUT_LOW(gpioPin);
	}

	return ret;

}


u8 readCtrlGpioVal(u8 gpioPIn)
{
	u8 ret = 0;
	CTRL_DX_PIN_INTPUT_MODE(gpioPIn);
	ret = CTRL_DX_PIN_INTPUT(gpioPIn);

	return ret;

}

u8 writeCtrlGpioVal(u8 gpioPin ,u8 valtmp)
{
	u8 ret = 0;
	CTRL_DX_PIN_OUTPUT_MODE(gpioPin);
	if(valtmp == 1)
	{
		CTRL_DX_PIN_OUPUT_HIGH(gpioPin);
	}
	else
	{
		CTRL_DX_PIN_OUPUT_LOW(gpioPin);
	}

	return ret;

}

u8 getAdc_hw_ver()
{
	u8 curHwvs = 0xf;
    /**KGR_HWVS_D0_PORT~KGR_HWVS_D2_PORT配置为GPIO输入**/
#if 1
	CTRL_DX_PIN_INTPUT_MODE(KGR_GPIO_ADC_MODE0);
	CTRL_DX_PIN_INTPUT_MODE(KGR_GPIO_ADC_MODE1);

    curHwvs = 0;
    curHwvs |= (CTRL_DX_PIN_INTPUT(KGR_GPIO_ADC_MODE1) << 1);
    curHwvs |= (CTRL_DX_PIN_INTPUT(KGR_GPIO_ADC_MODE0) << 0);
#endif
    return curHwvs;

}
