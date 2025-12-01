/******************************************************************************
 * Copyright (C) 2024 ZS Development Team, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @file uart_service.c
 * @brief
 * @version 0.1
 * Change Logs:
 * Date           Author       Notes
 * 2024-12-11     Andy      first version
******************************************************************************/
#include "uart_service.h"
#include "zs_printf.h"
#include "string.h"
#include "xparameters.h"



#include "xparameters.h"
#include "xplatform_info.h"
#include "xuartlite.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xuartlite_l.h"
#include "board.h"
#include "fifo.h"
#include "../usr/userTask.h"

#ifdef XPAR_INTC_0_DEVICE_ID
#include "xintc.h"
#else
#include "xscugic.h"
#endif


/*UART0为与MCU通信交互串口*/
#define MCU_COM_UART_ADDR		(XPAR_UARTLITE_0_BASEADDR)
#define MCU_COM_DEVICE_ID	  	XPAR_UARTLITE_0_DEVICE_ID
#define MCU_COM_IRPT_INTR	  	XPAR_INTC_0_UARTLITE_0_VEC_ID

/*UART1为调试串口*/
#define DBG_UART_ADDR			(XPAR_FPGA_DEBUG_UART1_BASEADDR)
#define DBG_UART_DEVICE_ID	  	XPAR_FPGA_DEBUG_UART1_DEVICE_ID
#define DBG_UART_IRPT_INTR	  	XPAR_AXI_INTC_0_FPGA_DEBUG_UART1_INTERRUPT_INTR


/*Ublox 串口*/
#define UBLOX_UART_ADDR			(XPAR_UBLOX_UART_BASEADDR)
#define UBLOX_UART_DEVICE_ID	XPAR_UBLOX_UART_DEVICE_ID
#define UBLOX_UART_IRPT_INTR	XPAR_AXI_INTC_0_UBLOX_UART_INTERRUPT_INTR


/*BD21 串口0*/
#define BD21_0_UART_ADDR			(XPAR_BD21_UART0_BASEADDR)
#define BD21_0_UART_DEVICE_ID	XPAR_BD21_UART0_DEVICE_ID
#define BD21_0_UART_IRPT_INTR	XPAR_AXI_INTC_0_BD21_UART0_INTERRUPT_INTR


/*BD21 串口1*/
#define BD21_1_UART_ADDR			(XPAR_BD21_UART1_BASEADDR)
#define BD21_1_UART_DEVICE_ID	XPAR_BD21_UART1_DEVICE_ID
#define BD21_1_UART_IRPT_INTR	XPAR_AXI_INTC_0_BD21_UART1_INTERRUPT_INTR


unsigned char g_ublox_bd21_uartTransmit = 0;


static XUartLite UartLiteBD21_0Inst;  /* 0he instance of the UartLite Device */
static XUartLite UartLiteBD21_1Inst;  /* The instance of the UartLite Device */
static XUartLite UartLiteDbgInst;  /* The instance of the UartLite Device */
static XUartLite UartUbloxInst;

unsigned char gdbgGetchar = 0;

int (* gpfUartPutByte[7] )(char ch) = {0}; //uart pl0 put byte
/****************************************************************************/
/**
*
* This function setups the interrupt system such that interrupts can occur
* for the UartLite. This function is application specific since the actual
* system may or may not have an interrupt controller. The UartLite could be
* directly connected to a processor without an interrupt controller. The
* user should modify this function to fit the application.
*
* @param	IntcInstancePtr is a pointer to the instance of INTC driver.
* @param	UartLiteInstPtr is a pointer to the instance of UartLite driver.
*		XPAR_<UARTLITE_instance>_DEVICE_ID value from xparameters.h.
* @param	UartLiteIntrId is the Interrupt ID and is typically
*		XPAR_<INTC_instance>_<UARTLITE_instance>_VEC_ID
*		value from xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
****************************************************************************/
static int UartSetupIntrSystem(XIntc *IntcPtr,
				XUartLite *UartLiteInstPtr,
				u16 UartLiteIntrId)
{
	int Status;
#if 0
	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	Status = XIntc_Initialize(&IntcPtr, UartLiteIntrId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the specific
	 * interrupt processing for the device.
	 */
	Status = XIntc_Connect(IntcPtr, UartLiteIntrId,
			(XInterruptHandler)XUartLite_InterruptHandler,
			(void *)UartLiteInstPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts, specific real mode so that
	 * the UART can cause interrupts through the interrupt controller.
	 */
	Status = XIntc_Start(IntcPtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/*
	 * Enable the interrupt for the UartLite.
	 */
	XIntc_Enable(IntcPtr, UartLiteIntrId);
	/*
	 * Enable exceptions.
	 */
	Xil_ExceptionEnable();


	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function is the handler which performs processing to receive data from
* the UartLite. It is called from an interrupt context such that the amount of
* processing performed should be minimized. It is called when any data is
* present in the receive FIFO of the UartLite such that the data can be
* retrieved from the UartLite. The amount of data present in the FIFO is not
* known when this function is called.
*
* This handler provides an example of how to handle data for the UartLite, but
* is application specific.
*
* @param	CallBackRef contains a callback reference from the driver,
*		in this case it is the instance pointer for the UartLite driver.
* @param	EventData contains the number of bytes sent or received for sent
*		and receive events.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
static void Bd210ComRecvHandler(void *CallBackRef, unsigned int EventData)
{
	err_t err;
	uint8_t res;
	res = XUartLite_RecvByte(XPAR_BD21_UART0_BASEADDR);
	if(gpfUartPutByte[PLUART_INDEX_2_BD21_1])
		gpfUartPutByte[PLUART_INDEX_2_BD21_1](res);
	if(g_ublox_bd21_uartTransmit == 3)
	{
		XUartLite_SendByte(XPAR_FPGA_DEBUG_UART1_BASEADDR,res);
	}

}

static void Bd211ComRecvHandler(void *CallBackRef, unsigned int EventData)
{
	err_t err;
	uint8_t res;
	res = XUartLite_RecvByte(XPAR_BD21_UART1_BASEADDR);
	if(gpfUartPutByte[PLUART_INDEX_3_BD21_2])
		gpfUartPutByte[PLUART_INDEX_3_BD21_2](res);

}

/****************************************************************************/
/**
*
* This function is the handler which performs processing to receive data from
* the UartLite. It is called from an interrupt context such that the amount of
* processing performed should be minimized. It is called when any data is
* present in the receive FIFO of the UartLite such that the data can be
* retrieved from the UartLite. The amount of data present in the FIFO is not
* known when this function is called.
*
* This handler provides an example of how to handle data for the UartLite, but
* is application specific.
*
* @param	CallBackRef contains a callback reference from the driver,
*		in this case it is the instance pointer for the UartLite driver.
* @param	EventData contains the number of bytes sent or received for sent
*		and receive events.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
static void UartDbgRecvHandler(void *CallBackRef, unsigned int EventData)
{
	uint8_t res;
	gdbgGetchar = XUartLite_RecvByte(DBG_UART_ADDR);
	enqueue(gdbgGetchar);

	if(gpfUartPutByte[PLUART_INDEX_0_PC])
		gpfUartPutByte[PLUART_INDEX_0_PC](gdbgGetchar);
	if(g_ublox_bd21_uartTransmit == 1)
	{
		XUartLite_SendByte(XPAR_UBLOX_UART_BASEADDR,gdbgGetchar);
	}

	if(g_ublox_bd21_uartTransmit == 3)
	{
		XUartLite_SendByte(XPAR_BD21_UART0_BASEADDR,gdbgGetchar);
	}
}


static void UartUbloxRecvHandler(void *CallBackRef, unsigned int EventData)
{
	uint8_t res;
	res = XUartLite_RecvByte(UBLOX_UART_ADDR);

	if(gpfUartPutByte[PLUART_INDEX_1_UBLOX])
		gpfUartPutByte[PLUART_INDEX_1_UBLOX](res);
	if(g_ublox_bd21_uartTransmit == 1)
	{
		XUartLite_SendByte(DBG_UART_ADDR,res);
	}

	//enqueue(gdbgGetchar);
}


/****************************************************************************/
/**
*
* This function does a minimal test on the UartLite device and driver as a
* design example. The purpose of this function is to illustrate how to use
* the XUartLite component.
*
* This function sends data through the UartLite.
*
* This function uses the interrupt driver mode of the UartLite.  The calls to
* the  UartLite driver in the interrupt handlers, should only use the
* non-blocking calls.
*
* @param	None
*		is the XPAR_<UARTLITE_instance>_DEVICE_ID value from
*		xparameters.h.
* @param	UartLiteIntrId is the Interrupt ID and is typically
*		XPAR_<INTC_instance>_<UARTLITE_instance>_VEC_ID value from
*		xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
* This function contains an infinite loop such that if interrupts are not
* working it may never return.
*
****************************************************************************/
static int Bd210UartInit(void)
{
	int Status;

	/*
	 * Initialize the UartLite driver so that it's ready to use.
	 */
	Status = XUartLite_Initialize(&UartLiteBD21_0Inst, BD21_0_UART_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XUartLite_SelfTest(&UartLiteBD21_0Inst);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the UartLite to the interrupt subsystem such that interrupts
	 * can occur. This function is application specific.
	 */
	Status = UartSetupIntrSystem(&IntcInstancePtr,
					 &UartLiteBD21_0Inst,
					 BD21_0_UART_IRPT_INTR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the handlers for the UartLite that will be called from the
	 * interrupt context when data has been sent and received,
	 * specify a pointer to the UartLite driver instance as the callback
	 * reference so the handlers are able to access the instance data.
	 */
	XUartLite_SetRecvHandler(&UartLiteBD21_0Inst, Bd210ComRecvHandler,
							 &UartLiteBD21_0Inst);

	/*
	 * Enable the interrupt of the UartLite so that the interrupts
	 * will occur.
	 */
	XUartLite_EnableInterrupt(&UartLiteBD21_0Inst);

	return XST_SUCCESS;
}

static int Bd211UartInit(void)
{
	int Status;

	/*
	 * Initialize the UartLite driver so that it's ready to use.
	 */
	Status = XUartLite_Initialize(&UartLiteBD21_1Inst, BD21_1_UART_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XUartLite_SelfTest(&UartLiteBD21_1Inst);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the UartLite to the interrupt subsystem such that interrupts
	 * can occur. This function is application specific.
	 */
	Status = UartSetupIntrSystem(&IntcInstancePtr,
					 &UartLiteBD21_1Inst,
					 BD21_1_UART_IRPT_INTR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the handlers for the UartLite that will be called from the
	 * interrupt context when data has been sent and received,
	 * specify a pointer to the UartLite driver instance as the callback
	 * reference so the handlers are able to access the instance data.
	 */
	XUartLite_SetRecvHandler(&UartLiteBD21_1Inst, Bd211ComRecvHandler,
							 &UartLiteBD21_1Inst);

	/*
	 * Enable the interrupt of the UartLite so that the interrupts
	 * will occur.
	 */
	XUartLite_EnableInterrupt(&UartLiteBD21_1Inst);

	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function does a minimal test on the UartLite device and driver as a
* design example. The purpose of this function is to illustrate how to use
* the XUartLite component.
*
* This function sends data through the UartLite.
*
* This function uses the interrupt driver mode of the UartLite.  The calls to
* the  UartLite driver in the interrupt handlers, should only use the
* non-blocking calls.
*
* @param	None
*		is the XPAR_<UARTLITE_instance>_DEVICE_ID value from
*		xparameters.h.
* @param	UartLiteIntrId is the Interrupt ID and is typically
*		XPAR_<INTC_instance>_<UARTLITE_instance>_VEC_ID value from
*		xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
* This function contains an infinite loop such that if interrupts are not
* working it may never return.
*
****************************************************************************/
static int DbgUartInit(void)
{
	int Status;

	/*
	 * Initialize the UartLite driver so that it's ready to use.
	 */
	Status = XUartLite_Initialize(&UartLiteDbgInst, DBG_UART_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XUartLite_SelfTest(&UartLiteDbgInst);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the UartLite to the interrupt subsystem such that interrupts
	 * can occur. This function is application specific.
	 */
	Status = UartSetupIntrSystem(&IntcInstancePtr,
					 &UartLiteDbgInst,
					 DBG_UART_IRPT_INTR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the handlers for the UartLite that will be called from the
	 * interrupt context when data has been sent and received,
	 * specify a pointer to the UartLite driver instance as the callback
	 * reference so the handlers are able to access the instance data.
	 */
	XUartLite_SetRecvHandler(&UartLiteDbgInst, UartDbgRecvHandler,
							 &UartLiteDbgInst);

	/*
	 * Enable the interrupt of the UartLite so that the interrupts
	 * will occur.
	 */
	XUartLite_EnableInterrupt(&UartLiteDbgInst);

	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function does a minimal test on the UartLite device and driver as a
* design example. The purpose of this function is to illustrate how to use
* the XUartLite component.
*
* This function sends data through the UartLite.
*
* This function uses the interrupt driver mode of the UartLite.  The calls to
* the  UartLite driver in the interrupt handlers, should only use the
* non-blocking calls.
*
* @param	None
*		is the XPAR_<UARTLITE_instance>_DEVICE_ID value from
*		xparameters.h.
* @param	UartLiteIntrId is the Interrupt ID and is typically
*		XPAR_<INTC_instance>_<UARTLITE_instance>_VEC_ID value from
*		xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
* This function contains an infinite loop such that if interrupts are not
* working it may never return.
*
****************************************************************************/
static int UbloxUartInit(void)
{
	int Status;

	/*
	 * Initialize the UartLite driver so that it's ready to use.
	 */
	Status = XUartLite_Initialize(&UartUbloxInst, UBLOX_UART_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XUartLite_SelfTest(&UartUbloxInst);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the UartLite to the interrupt subsystem such that interrupts
	 * can occur. This function is application specific.
	 */
	Status = UartSetupIntrSystem(&IntcInstancePtr,
					 &UartUbloxInst,
					 UBLOX_UART_IRPT_INTR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the handlers for the UartLite that will be called from the
	 * interrupt context when data has been sent and received,
	 * specify a pointer to the UartLite driver instance as the callback
	 * reference so the handlers are able to access the instance data.
	 */
	XUartLite_SetRecvHandler(&UartUbloxInst, UartUbloxRecvHandler,
			&UartUbloxInst);

	/*
	 * Enable the interrupt of the UartLite so that the interrupts
	 * will occur.
	 */
	XUartLite_EnableInterrupt(&UartUbloxInst);

	return XST_SUCCESS;
}



void UartDevInit(void)
{
	initQueue(64);
	//调试串口硬件初始化
	if(DbgUartInit() == XST_SUCCESS)
	{
		dx_kprintf("Init dbg Uart  OK !!!!\r\n");
	}
	if(UbloxUartInit() == XST_SUCCESS)
	{
		dx_kprintf("Init UBLOX Uart  OK !!!!\r\n");
	}
	if (Bd210UartInit() == XST_SUCCESS)
	{
		dx_kprintf("Init Bd21 Uart0  OK !!!!\r\n");
	}
	#if 1
	if (Bd211UartInit() == XST_SUCCESS)
	{
		dx_kprintf("Init Bd21 Uart1  OK !!!!\r\n");
	}
	#endif
}





