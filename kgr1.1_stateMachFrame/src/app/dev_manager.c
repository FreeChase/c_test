/******************************************************************************
 * Copyright (C) 2024 ZS Development Team, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @file dev_manager.c
 * @brief
 * @version 0.1
 * Change Logs:
 * Date           Author       Notes
 * 2024-12-11     Andy      first version
******************************************************************************/

#include <stdio.h>


#include "dev_manager.h"
#include "uart_service.h"


#include "antijam_fpga.h"

#include "zs_printf.h"


#include <JamNum_de.h>
#include "zsversion.h"

#include "board.h"

#include "KgrInfo.h"

#include "rf_rx1922.h"
#include "pll_adf4351.h"
#include "adc_mt9653.h"

void ShellInit(void);
int timerInit(void);
dx_handle hCmdProcSem = DX_NULL;
u32 tt = 1;


void module_versionAll_printf()
{
	u8 verHw = 0;
	xil_printf("KGR ver:  \r\n");

	verHw = getHwModuleVer();
	// if(verHw == HW_KGR_B3_VER_4_54)
	{
			//mcu 版本号
		xil_printf("    MCU :  %s  (B3) [%s][%s]\r\n",APP_VERSION_ALL,__DATE__,__TIME__);
	}
	
	if(g_HwModuleVer == HW_KGR_B1_VER_4_54)
	{
		//mcu 版本号
		xil_printf("	MCU :  %s (B1) [%s][%s]\r\n",APP_VERSION_ALL,__DATE__,__TIME__);
	}		
	
	if(g_HwModuleVer == HW_KGR_S_VER_4_54)
	{
		//mcu 版本号
		xil_printf("	MCU :  %s (S) [%s][%s]\r\n",APP_VERSION_ALL,__DATE__,__TIME__);
	}


	
	//fpga版本号	
	xil_printf("   FPGA :  AntiInterferenceCircuit-V%x-%x \r\n",mcu_to_fpga_read_kgr_reg(8),mcu_to_fpga_read_kgr_reg(9));

}

void module_version_printf()
{
	xil_printf("KGR ver:  \r\n");
	//mcu 版本号
	xil_printf("    MCU :  %s [%s][%s]\r\n",APP_VERSION,__DATE__,__TIME__);
	
	//fpga版本号	
	//xil_printf("   FPGA :  AntiInterferenceCircuit-V%x-%x \r\n",mcu_to_fpga_read_kgr_reg(8),mcu_to_fpga_read_kgr_reg(9));

	return ;
	
}

void adc_pll_status(void)
{
	u8 ret = 0;
	ret = readCtrlGpioVal(16);
	while(ret==0){
	ret = readCtrlGpioVal(16);
	}
	dx_kprintf("adc pll locked status is %d\r\n",ret);
}

void KGR_RST(void)
{
	u8 ret = 0;
	writeCtrlGpioVal(17,1);
	ret = readCtrlGpioVal(17);
	dx_kprintf("KGR RST VALUE is %d\r\n",ret);
}

void HARDWARE_DIRECT(u8 value)
{
	u8 ret = 0;
    if(value==1)
    {
    	writeCtrlGpioVal(10,0);
    	ret = readCtrlGpioVal(10);
    	dx_kprintf("hardware direct,value is %d\r\n",ret);
    }
    if(value==0)
    {
        writeCtrlGpioVal(10,1);
        ret = readCtrlGpioVal(10);
        dx_kprintf("kgr mode,value is %d\r\n",ret);
    }
}


void PreDevManagerInit(void)
{

	u32 hw_vertmp;
	u32 ret = 0;
	//uart device Initialize
    UartDevInit();

	module_version_printf();

	timerInit();
    ShellInit();
#if 1
    rx1922_init();										//射频初始化
    HARDWARE_DIRECT(0);									//射频板控制，射频直通模式控制接口，默认高电平，抗干扰模式  ---  reg[10]
	pe43711_ic0_write(0);								//步进衰减器配置
    dx_hw_adf4351_init();								//频率合成器初始化
    writeCtrlGpioVal(KGR_CTRL_HW_SWITCH_DIRECT,1);		//配置为抗干扰模式

    writeCtrlGpioVal(KGR_CTRL_HW_ANT_LNA1,1);			//开启1号天线电源
	writeCtrlGpioVal(KGR_CTRL_HW_ANT_LNA2,1);			//开启2号天线电源
    writeCtrlGpioVal(KGR_CTRL_HW_ANT_LNA3,1);			//开启3号天线电源
    writeCtrlGpioVal(KGR_CTRL_HW_ANT_LNA4,1);			//开启4号天线电源

	enableBD21();										//启动BD21基带
	enableUblox();										//启动Ublox基带

    hw_vertmp = getAdc_hw_ver();						//获取ADC版本
    dx_kprintf("ADC HW VERSION %d \r\n",hw_vertmp);
    switch(hw_vertmp)
    {
		case 0:
			dx_hw_mt9653_init();						//初始化ADC
			adc_pll_status();							//ADC时钟环路锁定判断
			dx_kprintf("MT9653 ini done\r\n");
			break;
		default:
			break;
    }
    KGR_RST();											//复位抗干扰算法
    ret = dx_hw_mt9653_delay_value_adjust();
#endif


}


