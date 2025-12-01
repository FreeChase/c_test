/******************************************************************************
 * Copyright (C) 2024 ZS Development Team, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @file board.c
 * @brief
 * @version 0.1
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-20     Andy      first version
******************************************************************************/
/***************************** Include Files *********************************/
#include "xparameters.h"
#include "xstatus.h"

#include "xil_assert.h"
#include "board.h"
#include "dev_manager.h"
#include "dxdef.h"
#include "xintc.h"
#include "sys_ctrl.h"



/************************** Constant Definitions *****************************/
u8 g_HwModuleVer;/**< 全局变量，当前硬件模块版本*/
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
void PreInit(void);
void PreLaterInit(void);
/************************** Variable Definitions *****************************/

/*****************************************************************************/
/**
* Board Device initialize function .
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void HwBoardInit(void)
{
	//System GPIO Initialize.
	if(SystemGpioInit() == 0)
	{
		dx_kprintf("GPIO INIT OK\r\n");
	}
	//Get hardware version information
	//g_HwModuleVer = SystemGetHwVer();
	PreInit();
	//System Device Initialize.
	PreDevManagerInit();

	PreLaterInit();
}


u8 getHwModuleVer()
{
	return g_HwModuleVer;
}
