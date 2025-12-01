/******************************************************************************
 * Copyright (C) 2024 ZS Development Team, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @file board.h
 * @brief
 * @version 0.1
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-20     Andy      first version
******************************************************************************/
#ifndef _SRC_BOARD_H_	/* prevent circular inclusions */
#define _SRC_BOARD_H_		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif
#include "xparameters.h"
//#include "xgpio.h"
#include "xil_printf.h"

#include "xintc.h"

extern XIntc IntcInstancePtr; /* The Instance of the Interrupt Controller Driver */

void HwBoardInit(void);

u8 getHwModuleVer();


#ifdef __cplusplus
}
#endif

#endif /* _SRC_BOARD_H_ */
