/******************************************************************************
* Copyright (c) 2015 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
*
* @file xfsbl_board.h
*
* This is the header file which contains board specific definitions for FSBL.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  ssc  01/20/16 Initial release
* 2.0   bv   12/05/16 Made compliance to MISRAC 2012 guidelines
*                     Added ZCU106 support
* 3.0	bkm  04/18/18 Added Board specific code w.r.t VADJ
* 4.0   bsv  11/12/19 Added support for ZCU216 board
*       bsv  02/05/20 Added support for ZCU208 board
*
* </pre>
*
* @note
*
******************************************************************************/

#ifndef _XBSP_H_
#define _XBSP_H_

#include "xparameters.h"
#include "xstatus.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xplatform_info.h"
#include "xuartps.h"

#include "xscugic.h"


#ifdef __cplusplus
extern "C" {
#endif

extern XScuGic gInterruptController;
extern XUartPs gUartPs	;

int IntrInit(void);

int UartPs0Init(void);
#ifdef __cplusplus
}
#endif

#endif  /* XFSBL_BOARD_H */
