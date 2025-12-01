/******************************************************************************
* Copyright (C) 2010 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
* @file  xttcps_intr_example.c
*
* This file contains a example using two timer counters in the Triple Timer
* Counter (TTC) module in the Ps block in interrupt mode.
*
* The example proceeds using interleaving interrupt handling from both
* timer counters. One timer counter, Ticker, counts how many interrupts
* has occurred to it, and updates a flag for another timer counter upon
* a given threshold. Another timer counter, PWM, waits for the flag set
* from the Ticker, and increases its duty cycle. When the duty cycle of
* PWM reaches 100, the example terminates.
*
* @note
*  The example may take seconds to minutes to finish. A small setting of
*  PWM_DELTA_DUTY gives a long running time, while a large setting makes
*  the example finishes faster.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver  Who    Date     Changes
* ---- ------ -------- ---------------------------------------------
* 1.00 drg/jz 01/23/10 First release
* 3.01 pkp	  01/30/16 Modified SetupTimer to remove XTtcps_Stop before TTC
*					   configuration as it is added in xttcps.c in
*					   XTtcPs_CfgInitialize
* 3.2  mus    10/28/16 Updated TmrCntrSetup as per prototype of
*                      XTtcPs_CalcIntervalFromFreq
* 3.10 mus    05/20/19 Update example to make it generic to run on any
*                      intended TTC device
*      aru    05/30/19 Updated the exapmle to use XTtcPs_InterruptHandler().
* 3.12 mus    07/13/20 Updated SettingsTable, to hold settings for PWM and tick
*                       timer device setting at correct indices. It fixes
*                       CR#1069191.
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/

#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xstatus.h"
#include "xil_exception.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xil_printf.h"

/************************** Constant Definitions *****************************/
#if defined (PLATFORM_ZYNQ)
#define NUM_DEVICES    9U
#else
#define NUM_DEVICES    12U
#endif

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are only defined here such that a user can easily
 * change all the needed parameters in one place.
 * Note: To run this example on intended TTC device, following changes
 *       needs to be done
 *       - Map constants given below to the intended TTC devices
 *       - Fill SettingsTable array based on the intended device IDs.
 *         e.g. If intended device IDs are 3 and 4, then SettingsTable[3]
 *              and SettingsTable[4] should be set properly.
 */
#define TTC_TICK_DEVICE_ID	XPAR_XTTCPS_1_DEVICE_ID
#define TTC_TICK_INTR_ID	XPAR_XTTCPS_1_INTR

#define TTC_PWM_DEVICE_ID	XPAR_XTTCPS_0_DEVICE_ID
#define TTC_PWM_INTR_ID		XPAR_XTTCPS_0_INTR
#define TTCPS_CLOCK_HZ		XPAR_XTTCPS_0_CLOCK_HZ

#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID

/*
 * Constants to set the basic operating parameters.
 * PWM_DELTA_DUTY is critical to the running time of the test. Smaller values
 * make the test run longer.
 */
#define	TICK_TIMER_FREQ_HZ	100  /* Tick timer counter's output frequency */
#define	PWM_OUT_FREQ		350  /* PWM timer counter's output frequency */

#define PWM_DELTA_DUTY	50 /* Initial and increment to duty cycle for PWM */
#define TICKS_PER_CHANGE_PERIOD TICK_TIMER_FREQ_HZ * 5 /* Tick signals PWM */

/**************************** Type Definitions *******************************/
typedef struct {
	u32 OutputHz;	/* Output frequency */
	XInterval Interval;	/* Interval value */
	u8 Prescaler;	/* Prescaler value */
	u16 Options;	/* Option settings */
} TmrCntrSetup;

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

static int TmrInterruptExample(void);  /* Main test */

/* Set up routines for timer counters */
static int SetupTicker(void);
static int SetupPWM(void);
static int SetupTimer(int DeviceID);

/* Interleaved interrupt test for both timer counters */
static int WaitForDutyCycleFull(void);

static int SetupInterruptSystem(u16 IntcDeviceID, XScuGic *IntcInstancePtr);

static void TickHandler(void *CallBackRef, u32 StatusEvent);
static void PWMHandler(void *CallBackRef, u32 StatusEvent);

/************************** Variable Definitions *****************************/

static XTtcPs TtcPsInst[NUM_DEVICES];	/* Number of available timer counters */

static TmrCntrSetup SettingsTable[NUM_DEVICES] = {
	{200, 0, 0, 0}, /* PWM timer counter initial setup, only output freq */
	{100, 0, 0, 0},	/* Ticker timer counter initial setup, only output freq */
};

// XScuGic gxInterruptController;  /* Interrupt controller instance */
extern XScuGic gxInterruptController;


static volatile u8 ErrorCount;		/* Errors seen at interrupt time */
static volatile u32 TickCount;		/* Ticker interrupts between PWM change */


/*****************************************************************************/
/**
*
* This function calls the Ttc interrupt example.
*
* @param	None
*
* @return
*		- XST_SUCCESS to indicate Success
*		- XST_FAILURE to indicate Failure.
*
* @note		None
*
*****************************************************************************/
int TimerInit(void)
{
	int Status;

	Status = TmrInterruptExample();
	if (Status != XST_SUCCESS) {
		xil_printf("TTC Init Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("TTC Init Successfully\r\n");
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function sets up the interrupt example.
*
* @param	None.
*
* @return
*		- XST_SUCCESS to indicate Success
*		- XST_FAILURE to indicate Failure.
*
* @note		None
*
****************************************************************************/
static int TmrInterruptExample(void)
{
	int Status;

	/*
	 * Make sure the interrupts are disabled, in case this is being run
	 * again after a failure.
	 */

	/*
	 * Connect the Intc to the interrupt subsystem such that interrupts can
	 * occur. This function is application specific.
	 */
	#if 0
	Status = SetupInterruptSystem(INTC_DEVICE_ID, &gxInterruptController);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	#endif
	/*
	 * Set up the Ticker timer
	 */
	Status = SetupTicker();
	if (Status != XST_SUCCESS) {
		return Status;
	}


	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function sets up the Ticker timer.
*
* @param	None
*
* @return	XST_SUCCESS if everything sets up well, XST_FAILURE otherwise.
*
* @note		None
*
*****************************************************************************/
int SetupTicker(void)
{
	int Status;
	TmrCntrSetup *TimerSetup;
	XTtcPs *TtcPsTick;

	TimerSetup = &(SettingsTable[TTC_TICK_DEVICE_ID]);

	/*
	 * Set up appropriate options for Ticker: interval mode without
	 * waveform output.
	 */
	TimerSetup->Options |= (XTTCPS_OPTION_INTERVAL_MODE |
					      XTTCPS_OPTION_WAVE_DISABLE);

	/*
	 * Calling the timer setup routine
	 *  . initialize device
	 *  . set options
	 */
	Status = SetupTimer(TTC_TICK_DEVICE_ID);
	if(Status != XST_SUCCESS) {
		return Status;
	}

	TtcPsTick = &(TtcPsInst[TTC_TICK_DEVICE_ID]);

	/*
	 * Connect to the interrupt controller
	 */
	Status = XScuGic_Connect(&gxInterruptController, TTC_TICK_INTR_ID,
		(Xil_ExceptionHandler)XTtcPs_InterruptHandler, (void *)TtcPsTick);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XTtcPs_SetStatusHandler(&(TtcPsInst[TTC_TICK_DEVICE_ID]), &(TtcPsInst[TTC_TICK_DEVICE_ID]),
		              (XTtcPs_StatusHandler)TickHandler);

	/*
	 * Enable the interrupt for the Timer counter
	 */
	XScuGic_Enable(&gxInterruptController, TTC_TICK_INTR_ID);

	/*
	 * Enable the interrupts for the tick timer/counter
	 * We only care about the interval timeout.
	 */
	XTtcPs_EnableInterrupts(TtcPsTick, XTTCPS_IXR_INTERVAL_MASK);

	/*
	 * Start the tick timer/counter
	 */
	XTtcPs_Start(TtcPsTick);

	return Status;
}


/****************************************************************************/
/**
*
* This function sets up a timer counter device, using the information in its
* setup structure.
*  . initialize device
*  . set options
*  . set interval and prescaler value for given output frequency.
*
* @param	DeviceID is the unique ID for the device.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
*****************************************************************************/
int SetupTimer(int DeviceID)
{
	int Status;
	XTtcPs_Config *Config;
	XTtcPs *Timer;
	TmrCntrSetup *TimerSetup;

	TimerSetup = &SettingsTable[DeviceID];

	Timer = &(TtcPsInst[DeviceID]);

	/*
	 * Look up the configuration based on the device identifier
	 */
	Config = XTtcPs_LookupConfig(DeviceID);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	/*
	 * Initialize the device
	 */
	Status = XTtcPs_CfgInitialize(Timer, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the options
	 */
	XTtcPs_SetOptions(Timer, TimerSetup->Options);

	/*
	 * Timer frequency is preset in the TimerSetup structure,
	 * however, the value is not reflected in its other fields, such as
	 * IntervalValue and PrescalerValue. The following call will map the
	 * frequency to the interval and prescaler values.
	 */
	XTtcPs_CalcIntervalFromFreq(Timer, TimerSetup->OutputHz,
		&(TimerSetup->Interval), &(TimerSetup->Prescaler));

	/*
	 * Set the interval and prescale
	 */
	XTtcPs_SetInterval(Timer, TimerSetup->Interval);
	XTtcPs_SetPrescaler(Timer, TimerSetup->Prescaler);

	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function setups the interrupt system such that interrupts can occur.
* This function is application specific since the actual system may or may not
* have an interrupt controller.  The TTC could be directly connected to a
* processor without an interrupt controller.  The user should modify this
* function to fit the application.
*
* @param	IntcDeviceID is the unique ID of the interrupt controller
* @param	IntcInstacePtr is a pointer to the interrupt controller
*		instance.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
*****************************************************************************/
static int SetupInterruptSystem(u16 IntcDeviceID,
				    XScuGic *IntcInstancePtr)
{
	int Status;
	XScuGic_Config *IntcConfig; /* The configuration parameters of the
					interrupt controller */

	/*
	 * Initialize the interrupt controller driver
	 */
	IntcConfig = XScuGic_LookupConfig(IntcDeviceID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
			IntcInstancePtr);

	/*
	 * Enable interrupts in the ARM
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}
volatile uint32_t gTimerTick = 0;
/***************************************************************************/
/**
*
* This function is the handler which handles the periodic tick interrupt.
* It updates its count, and set a flag to signal PWM timer counter to
* update its duty cycle.
*
* This handler provides an example of how to handle data for the TTC and
* is application specific.
*
* @param	CallBackRef contains a callback reference from the driver, in
*		this case it is the instance pointer for the TTC driver.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void TickHandler(void *CallBackRef, u32 StatusEvent)
{
	if (0 != (XTTCPS_IXR_INTERVAL_MASK & StatusEvent)) {
		TickCount++;
		gTimerTick++;

	}
	else {
		/*
		 * The Interval event should be the only one enabled. If it is
		 * not it is an error
		 */
		ErrorCount++;
	}
}

void tick_intr_enable(void)
{
	XTtcPs_EnableInterrupts(&(TtcPsInst[TTC_TICK_DEVICE_ID]), XTTCPS_IXR_INTERVAL_MASK);
}
void tick_intr_disable(void)
{
	XTtcPs_DisableInterrupts(&(TtcPsInst[TTC_TICK_DEVICE_ID]), XTTCPS_IXR_INTERVAL_MASK);
}


uint32_t tick_diff(uint32_t old)
{
	return gTimerTick-old;
}

uint32_t tick_get(void)
{
	return gTimerTick;
}

uint32_t tick_delay_ms(uint32_t ms)
{
	uint32_t current;

	current=tick_get();
	
	while (1)
	{
		if( tick_diff(current) > ms)
		{
			break;
		}
	}
	return 0;
}
