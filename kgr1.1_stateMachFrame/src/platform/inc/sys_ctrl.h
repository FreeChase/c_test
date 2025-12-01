/******************************************************************************
 * Copyright (C) 2024 ZS Development Team, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @file sys_ctrl.h
 * @brief
 * @version 0.1
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-20     Andy      first version
******************************************************************************/
#ifndef _SRC_SYS_GPIO_CTRL_H_		/* prevent circular inclusions */
#define _SRC_SYS_GPIO_CTRL_H_		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif
#include "xparameters.h"
#include "xgpio.h"
#include "xil_types.h"
#include "xil_printf.h"
#ifdef XPAR_INTC_0_DEVICE_ID
 #include "xintc.h"
 #include <stdio.h>
#endif
/************************** Constant Definitions *****************************/
extern XGpio gPeriGpioSpi; /* The driver instance for GPIO  Device configured as SPI */
extern XGpio gCtrlGpio; /* The driver instance for GPIO  Device configured as SPI */
extern XIntc IntcInstancePtr; /* The Instance of the Interrupt Controller Driver */


/* Anti interference frequency information */
#define HW_KGR_LC_B3                    0x0 /*低成本四通道抗干扰模块 B3抗干扰*/
#define HW_KGR_LC_B1                    0x1 /*低成本四通道抗干扰模块 B1抗干扰*/
#define HW_KGR_LC_S                     0x2 /*低成本四通道抗干扰模块 S抗干扰*/



/***************** Macros (Inline Functions) Definitions *********************/
#define GPIO_CTRL_ID			(XPAR_GPIO_CTRL_DEVICE_ID)
#define INTC_SOFT_INTERRUPT_ID	XPAR_GPIO_CTRL_INTERRUPT_PRESENT


#define GPIO_PERI_ID			(XPAR_GPIO_PERI_DEVICE_ID)
#define INTC_SOFT_PERI_INTERRUPT_ID	XPAR_GPIO_PERI_INTERRUPT_PRESENT

/* The following constant is used to determine which channel of the GPIO is
 * used if there are 2 channels supported in the GPIO.
 */
#define PERI_FUNCTION_CHANNEL    (1)

/* UBLOX 复位引脚*/
#define KGR_GPIO_UBLOX_RSTN 		(21)

/* UBLOX 复位引脚*/
#define KGR_GPIO_ADC_MODE0 		(4)
#define KGR_GPIO_ADC_MODE1 		(5)

#define KGR_CTRL_HW_SWITCH_DIRECT  (10)
#define KGR_CTRL_HW_ANT_LNA1  (12)
#define KGR_CTRL_HW_ANT_LNA2  (13)
#define KGR_CTRL_HW_ANT_LNA3  (14)
#define KGR_CTRL_HW_ANT_LNA4  (15)

#define KGR_CTRL_UART_REMAP_BASE  (18)

#define BD21_CTRL_HW_RESET  (11)

#define KGR_CTRL_HW_SWITCH_DIRECT  (10)

#undef SYS_SETBIT
#define SYS_SETBIT(addr, data)     			(Xil_Out32(addr, Xil_In32(addr) | (data)))

#undef SYS_CLRBIT
#define SYS_CLRBIT(addr, data)     			(Xil_Out32(addr, Xil_In32(addr) & ~(data)))

#undef PERI_DX_PIN_INTPUT_MODE
#define PERI_DX_PIN_INTPUT_MODE(pin)     		(XGpio_SetDataDirection(&gPeriGpioSpi, PERI_FUNCTION_CHANNEL, (XGpio_GetDataDirection(&gPeriGpioSpi, PERI_FUNCTION_CHANNEL) | (1 << (pin)))))

#undef PERI_DX_PIN_OUTPUT_MODE
#define PERI_DX_PIN_OUTPUT_MODE(pin)     		(XGpio_SetDataDirection(&gPeriGpioSpi, PERI_FUNCTION_CHANNEL, (XGpio_GetDataDirection(&gPeriGpioSpi, PERI_FUNCTION_CHANNEL) & (~(1 << (pin))))))

#undef PERI_DX_PIN_OUPUT_HIGH
#define PERI_DX_PIN_OUPUT_HIGH(pin)     			(XGpio_DiscreteSet(&gPeriGpioSpi, PERI_FUNCTION_CHANNEL, (1 << pin)))

#undef PERI_DX_PIN_OUPUT_LOW
#define PERI_DX_PIN_OUPUT_LOW(pin)     			(XGpio_DiscreteClear(&gPeriGpioSpi, PERI_FUNCTION_CHANNEL, (1 << pin)))

#undef PERI_DX_PIN_INTPUT
#define PERI_DX_PIN_INTPUT(pin)     				((XGpio_DiscreteRead(&gPeriGpioSpi, PERI_FUNCTION_CHANNEL) >> (pin)) & 0x1)




#define CTRL_DX_PIN_INTPUT_MODE(pin)     		(XGpio_SetDataDirection(&gCtrlGpio, PERI_FUNCTION_CHANNEL, (XGpio_GetDataDirection(&gCtrlGpio, PERI_FUNCTION_CHANNEL) | (1 << (pin)))))

#define CTRL_DX_PIN_OUTPUT_MODE(pin)     		(XGpio_SetDataDirection(&gCtrlGpio, PERI_FUNCTION_CHANNEL, (XGpio_GetDataDirection(&gCtrlGpio, PERI_FUNCTION_CHANNEL) & (~(1 << (pin))))))

#define CTRL_DX_PIN_OUPUT_HIGH(pin)     			(XGpio_DiscreteSet(&gCtrlGpio, PERI_FUNCTION_CHANNEL, (1 << pin)))

#define CTRL_DX_PIN_OUPUT_LOW(pin)     			(XGpio_DiscreteClear(&gCtrlGpio, PERI_FUNCTION_CHANNEL, (1 << pin)))

#define CTRL_DX_PIN_INTPUT(pin)     				((XGpio_DiscreteRead(&gCtrlGpio, PERI_FUNCTION_CHANNEL) >> (pin)) & 0x1)



int SystemGpioInit(void);

int SystemSetupSoftIntr(XIntc *IntcInstancePtr, u16 IntrId, XInterruptHandler Handler);

void SystemIntrDisable(XIntc *IntcInstancePtr, u16 IntrId);

void SystemIntrEnable(XIntc *IntcInstancePtr, u16 IntrId);

u8 SystemGetHwVer(void);

void SystemReset(void);

void SystemSwitchRFWorkMode(int RFCtrl);

u8 SystemGetKgrMode(void);

u8 SystemGetKgrStatus(void);

u8 SystemGetKgrSelfTestStatus(void);

/*
设置当前干扰检测状态
  0：无干扰
  1：有干扰
*/
u8 SystemSetKgrStatus(u8 statusVal);

u8 readPeriGpioVal(u8 gpioPIn);

u8 writePeriGpioVal(u8 gpioPin,u8 valtmp);

u8 readCtrlGpioVal(u8 gpioPIn);

u8 writeCtrlGpioVal(u8 gpioPin ,u8 valtmp);

u8 enableBD21();
u8 enableUblox();

u8 getAdc_hw_ver();

#ifdef __cplusplus
}
#endif

#endif /* _SRC_SYS_GPIO_CTRL_H_ */
