/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file		xuartps_intr_example.c
*
* This file contains a design example using the XUartPs driver in interrupt
* mode. It sends data and expects to receive the same data through the device
* using the local loopback mode.
*
*
* @note
* The example contains an infinite loop such that if interrupts are not
* working it may hang.
*
* MODIFICATION HISTORY:
* <pre>
* Ver   Who    Date     Changes
* ----- ------ -------- ----------------------------------------------
* 1.00a  drg/jz 01/13/10 First Release
* 1.00a  sdm    05/25/11 Modified the example for supporting Peripheral tests
*		        in SDK
* 1.03a  sg     07/16/12 Updated the example for CR 666306. Modified
*			the device ID to use the first Device Id
*			and increased the receive timeout to 8
*			Removed the printf at the start of the main
*			Put the device normal mode at the end of the example
* 3.1	kvn		04/10/15 Added code to support Zynq Ultrascale+ MP.
* 3.1   mus     01/14/16 Added support for intc interrupt controller
* 3.8   adk     10/05/19 Don't update the DeviceId variable in peripheral test
*			 app case.
*
* </pre>
****************************************************************************/

/***************************** Include Files *******************************/

#include "xparameters.h"
#include "xplatform_info.h"
#include "xuartps.h"
#include "xil_exception.h"
#include "xil_printf.h"


#ifdef XPAR_INTC_0_DEVICE_ID
#include "xintc.h"
#else
#include "xscugic.h"
#endif
/************************** Constant Definitions **************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#ifdef XPAR_INTC_0_DEVICE_ID
#define INTC		XIntc
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define UART_INT_IRQ_ID		XPAR_INTC_0_UARTPS_0_VEC_ID
#else
#define INTC		XScuGic
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define UART_INT_IRQ_ID		XPAR_XUARTPS_0_INTR
#endif
/*
 * The following constant controls the length of the buffers to be sent
 * and received with the UART,
 */
#define TEST_BUFFER_SIZE	100


/**************************** Type Definitions ******************************/


/************************** Function Prototypes *****************************/

int UartPsIntrExample(INTC *IntcInstPtr, XUartPs *UartInstPtr,
			u16 DeviceId, u16 UartIntrId);


static int SetupInterruptSystem(INTC *IntcInstancePtr,
				XUartPs *UartInstancePtr,
				u16 UartIntrId);

void Handler(void *CallBackRef, u32 Event, unsigned int EventData);

static void _2ReceiveDataHandler(XUartPs *InstancePtr);
static void _2SendDataHandler(XUartPs *InstancePtr, u32 IsrStatus);
static void _2ReceiveErrorHandler(XUartPs *InstancePtr, u32 IsrStatus);
static void _2ReceiveTimeoutHandler(XUartPs *InstancePtr);
static void _2ModemHandler(XUartPs *InstancePtr);
void _2XUartPs_InterruptHandler(XUartPs *InstancePtr);


/************************** Variable Definitions ***************************/

XUartPs UartPs	;		/* Instance of the UART Device */
INTC gInterruptController;	/* Instance of the Interrupt Controller */
extern INTC gxInterruptController;	/* Instance of the Interrupt Controller */

/*
 * The following buffers are used in this example to send and receive data
 * with the UART.
 */
static u8 SendBuffer[TEST_BUFFER_SIZE];	/* Buffer for Transmitting Data */
static u8 RecvBuffer[TEST_BUFFER_SIZE];	/* Buffer for Receiving Data */

/*
 * The following counters are used to determine when the entire buffer has
 * been sent and received.
 */
volatile int TotalReceivedCount;
volatile int TotalSentCount;
int TotalErrorCount;

int (* gpfUartPsPutByte )(char ch); 


int PSUart0Init(void)
{
	int Status;

	/* Run the UartPs Interrupt example, specify the the Device ID */
	Status = UartPsIntrExample(&gxInterruptController, &UartPs,
				UART_DEVICE_ID, UART_INT_IRQ_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("UART Interrupt Example Test Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully ran UART Interrupt Example Test\r\n");
	return XST_SUCCESS;
}


/**************************************************************************/
/**
*
* This function does a minimal test on the UartPS device and driver as a
* design example. The purpose of this function is to illustrate
* how to use the XUartPs driver.
*
* This function sends data and expects to receive the same data through the
* device using the local loopback mode.
*
* This function uses interrupt mode of the device.
*
* @param	IntcInstPtr is a pointer to the instance of the Scu Gic driver.
* @param	UartInstPtr is a pointer to the instance of the UART driver
*		which is going to be connected to the interrupt controller.
* @param	DeviceId is the device Id of the UART device and is typically
*		XPAR_<UARTPS_instance>_DEVICE_ID value from xparameters.h.
* @param	UartIntrId is the interrupt Id and is typically
*		XPAR_<UARTPS_instance>_INTR value from xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note
*
* This function contains an infinite loop such that if interrupts are not
* working it may never return.
*
**************************************************************************/
int UartPsIntrExample(INTC *IntcInstPtr, XUartPs *UartInstPtr,
			u16 DeviceId, u16 UartIntrId)
{
	int Status;
	XUartPs_Config *Config;
	int Index;
	u32 IntrMask;
	int BadByteCount = 0;

#ifndef TESTAPP_GEN
	if (XGetPlatform_Info() == XPLAT_ZYNQ_ULTRA_MP) {
#ifdef XPAR_XUARTPS_1_DEVICE_ID
		DeviceId = XPAR_XUARTPS_1_DEVICE_ID;
#endif
	}
#endif

	/*
	 * Initialize the UART driver so that it's ready to use
	 * Look up the configuration in the config table, then initialize it.
	 */
	Config = XUartPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(UartInstPtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Check hardware build */
	Status = XUartPs_SelfTest(UartInstPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the UART to the interrupt subsystem such that interrupts
	 * can occur. This function is application specific.
	 */
	Status = SetupInterruptSystem(IntcInstPtr, UartInstPtr, UartIntrId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the handlers for the UART that will be called from the
	 * interrupt context when data has been sent and received, specify
	 * a pointer to the UART driver instance as the callback reference
	 * so the handlers are able to access the instance data
	 */
	XUartPs_SetHandler(UartInstPtr, (XUartPs_Handler)Handler, UartInstPtr);

	/*
	 * Enable the interrupt of the UART so interrupts will occur, setup
	 * a local loopback so data that is sent will be received.
	 */
	IntrMask =
		XUARTPS_IXR_TOUT | XUARTPS_IXR_PARITY | XUARTPS_IXR_FRAMING |
		XUARTPS_IXR_OVER | XUARTPS_IXR_TXEMPTY | XUARTPS_IXR_RXFULL |
		XUARTPS_IXR_RXOVR;

	if (UartInstPtr->Platform == XPLAT_ZYNQ_ULTRA_MP) {
		IntrMask |= XUARTPS_IXR_RBRK;
	}

	XUartPs_SetInterruptMask(UartInstPtr, IntrMask);
	XUartPs_SetRecvTimeout(UartInstPtr, 8);
	#if 0
	XUartPs_SetOperMode(UartInstPtr, XUARTPS_OPER_MODE_LOCAL_LOOP);

	/*
	 * Set the receiver timeout. If it is not set, and the last few bytes
	 * of data do not trigger the over-water or full interrupt, the bytes
	 * will not be received. By default it is disabled.
	 *
	 * The setting of 8 will timeout after 8 x 4 = 32 character times.
	 * Increase the time out value if baud rate is high, decrease it if
	 * baud rate is low.
	 */


	/*
	 * Initialize the send buffer bytes with a pattern and the
	 * the receive buffer bytes to zero to allow the receive data to be
	 * verified
	 */
	for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {

		SendBuffer[Index] = (Index % 26) + 'A';

		RecvBuffer[Index] = 0;
	}

	/*
	 * Start receiving data before sending it since there is a loopback,
	 * ignoring the number of bytes received as the return value since we
	 * know it will be zero
	 */
	XUartPs_Recv(UartInstPtr, RecvBuffer, TEST_BUFFER_SIZE);

	/*
	 * Send the buffer using the UART and ignore the number of bytes sent
	 * as the return value since we are using it in interrupt mode.
	 */
	XUartPs_Send(UartInstPtr, SendBuffer, TEST_BUFFER_SIZE);

	/*
	 * Wait for the entire buffer to be received, letting the interrupt
	 * processing work in the background, this function may get locked
	 * up in this loop if the interrupts are not working correctly.
	 */
	while (1) {
		if ((TotalSentCount == TEST_BUFFER_SIZE) &&
		    (TotalReceivedCount == TEST_BUFFER_SIZE)) {
			break;
		}
	}

	/* Verify the entire receive buffer was successfully received */
	for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
		if (RecvBuffer[Index] != SendBuffer[Index]) {
			BadByteCount++;
		}
	}



	/* Set the UART in Normal Mode */
	XUartPs_SetOperMode(UartInstPtr, XUARTPS_OPER_MODE_NORMAL);


	/* If any bytes were not correct, return an error */
	if (BadByteCount != 0) {
		return XST_FAILURE;
	}
	#endif
	return XST_SUCCESS;
}

/**************************************************************************/
/**
*
* This function is the handler which performs processing to handle data events
* from the device.  It is called from an interrupt context. so the amount of
* processing should be minimal.
*
* This handler provides an example of how to handle data for the device and
* is application specific.
*
* @param	CallBackRef contains a callback reference from the driver,
*		in this case it is the instance pointer for the XUartPs driver.
* @param	Event contains the specific kind of event that has occurred.
* @param	EventData contains the number of bytes sent or received for sent
*		and receive events.
*
* @return	None.
*
* @note		None.
*
***************************************************************************/
void Handler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	
}


/*****************************************************************************/
/**
*
* This function sets up the interrupt system so interrupts can occur for the
* Uart. This function is application-specific. The user should modify this
* function to fit the application.
*
* @param	IntcInstancePtr is a pointer to the instance of the INTC.
* @param	UartInstancePtr contains a pointer to the instance of the UART
*		driver which is going to be connected to the interrupt
*		controller.
* @param	UartIntrId is the interrupt Id and is typically
*		XPAR_<UARTPS_instance>_INTR value from xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
****************************************************************************/
static int SetupInterruptSystem(INTC *IntcInstancePtr,
				XUartPs *UartInstancePtr,
				u16 UartIntrId)
{
	int Status;

#ifdef XPAR_INTC_0_DEVICE_ID
#ifndef TESTAPP_GEN
	/*
	 * Initialize the interrupt controller driver so that it's ready to
	 * use.
	 */
	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif
	/*
	 * Connect the handler that will be called when an interrupt
	 * for the device occurs, the handler defined above performs the
	 * specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(IntcInstancePtr, UartIntrId,
		(XInterruptHandler) _2XUartPs_InterruptHandler, UartInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

#ifndef TESTAPP_GEN
	/*
	 * Start the interrupt controller so interrupts are enabled for all
	 * devices that cause interrupts.
	 */
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif
	/*
	 * Enable the interrupt for uart
	 */
	XIntc_Enable(IntcInstancePtr, UartIntrId);

	#ifndef TESTAPP_GEN
	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler) XIntc_InterruptHandler,
				IntcInstancePtr);
	#endif
#else
#if 0
	XScuGic_Config *IntcConfig; /* Config for interrupt controller */

	/* Initialize the interrupt controller driver */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the interrupt controller interrupt handler to the
	 * hardware interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler) XScuGic_InterruptHandler,
				IntcInstancePtr);
#endif

	/*
	 * Connect a device driver handler that will be called when an
	 * interrupt for the device occurs, the device driver handler
	 * performs the specific interrupt processing for the device
	 */
	Status = XScuGic_Connect(IntcInstancePtr, UartIntrId,
				  (Xil_ExceptionHandler) _2XUartPs_InterruptHandler,
				  (void *) UartInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Enable the interrupt for the device */
	XScuGic_Enable(IntcInstancePtr, UartIntrId);

#endif
#ifndef TESTAPP_GEN
	/* Enable interrupts */
	 Xil_ExceptionEnable();
#endif

	return XST_SUCCESS;
}




/****************************************************************************/
/**
*
* This function is the interrupt handler for the driver.
* It must be connected to an interrupt system by the application such that it
* can be called when an interrupt occurs.
*
* @param	InstancePtr contains a pointer to the driver instance
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void _2XUartPs_InterruptHandler(XUartPs *InstancePtr)
{
	u32 IsrStatus;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Read the interrupt ID register to determine which
	 * interrupt is active
	 */
	IsrStatus = XUartPs_ReadReg(InstancePtr->Config.BaseAddress,
				   XUARTPS_IMR_OFFSET);

	IsrStatus &= XUartPs_ReadReg(InstancePtr->Config.BaseAddress,
				   XUARTPS_ISR_OFFSET);

	/* Dispatch an appropriate handler. */
	if((IsrStatus & ((u32)XUARTPS_IXR_RXOVR | (u32)XUARTPS_IXR_RXEMPTY |
			(u32)XUARTPS_IXR_RXFULL)) != (u32)0) {
		/* Received data interrupt */
		_2ReceiveDataHandler(InstancePtr);
	}

	if((IsrStatus & ((u32)XUARTPS_IXR_TXEMPTY | (u32)XUARTPS_IXR_TXFULL))
									 != (u32)0) {
		/* Transmit data interrupt */
		_2SendDataHandler(InstancePtr, IsrStatus);
	}

	/* XUARTPS_IXR_RBRK is applicable only for Zynq Ultrascale+ MP */
	if ((IsrStatus & ((u32)XUARTPS_IXR_OVER | (u32)XUARTPS_IXR_FRAMING |
			(u32)XUARTPS_IXR_PARITY | (u32)XUARTPS_IXR_RBRK)) != (u32)0) {
		/* Received Error Status interrupt */
		_2ReceiveErrorHandler(InstancePtr, IsrStatus);
	}

	if((IsrStatus & ((u32)XUARTPS_IXR_TOUT)) != (u32)0) {
		/* Received Timeout interrupt */
		_2ReceiveTimeoutHandler(InstancePtr);
	}

	if((IsrStatus & ((u32)XUARTPS_IXR_DMS)) != (u32)0) {
		/* Modem status interrupt */
		_2ModemHandler(InstancePtr);
	}

	/* Clear the interrupt status. */
	XUartPs_WriteReg(InstancePtr->Config.BaseAddress, XUARTPS_ISR_OFFSET,
		IsrStatus);

}

/****************************************************************************/
/*
*
* This function handles interrupts for receive errors which include
* overrun errors, framing errors, parity errors, and the break interrupt.
*
* @param	InstancePtr is a pointer to the XUartPs instance.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void _2ReceiveErrorHandler(XUartPs *InstancePtr, u32 IsrStatus)
{
	u32 EventData;
	u32 Event;

	InstancePtr->is_rxbs_error = 0;

	if ((InstancePtr->Platform == XPLAT_ZYNQ_ULTRA_MP) &&
		(IsrStatus & ((u32)XUARTPS_IXR_PARITY | (u32)XUARTPS_IXR_RBRK
					| (u32)XUARTPS_IXR_FRAMING))) {
		InstancePtr->is_rxbs_error = 1;
	}
	/*
	 * If there are bytes still to be received in the specified buffer
	 * go ahead and receive them. Removing bytes from the RX FIFO will
	 * clear the interrupt.
	 */

	(void)XUartPs_ReceiveBuffer(InstancePtr);

	if (!(InstancePtr->is_rxbs_error)) {
		Event = XUARTPS_EVENT_RECV_ERROR;
		EventData = InstancePtr->ReceiveBuffer.RequestedBytes -
			InstancePtr->ReceiveBuffer.RemainingBytes;

		/*
		 * Call the application handler to indicate that there is a receive
		 * error or a break interrupt, if the application cares about the
		 * error it call a function to get the last errors.
		 */
		InstancePtr->Handler(InstancePtr->CallBackRef,
					Event,
					EventData);
	}
}

/****************************************************************************/
/**
*
* This function handles the receive timeout interrupt. This interrupt occurs
* whenever a number of bytes have been present in the RX FIFO and the receive
* data line has been idle for at lease 4 or more character times, (the timeout
* is set using XUartPs_SetrecvTimeout() function).
*
* @param	InstancePtr is a pointer to the XUartPs instance
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void _2ReceiveTimeoutHandler(XUartPs *InstancePtr)
{
	u32 CsrRegister;
	u32 u32val;
	u32 ReceivedCount = 0U;
	u32 ByteStatusValue, EventData;


	/*
	 * Read the Channel Status Register to determine if there is any data in
	 * the RX FIFO
	 */
	CsrRegister = XUartPs_ReadReg(InstancePtr->Config.BaseAddress,
				XUARTPS_SR_OFFSET);

	/*
	 * Loop until there is no more data in RX FIFO or the specified
	 * number of bytes has been received
	 */
	while((((CsrRegister & XUARTPS_SR_RXEMPTY) == (u32)0))){

		u32val = XUartPs_ReadReg(InstancePtr->Config.BaseAddress, XUARTPS_FIFO_OFFSET);
		if(gpfUartPsPutByte)
			gpfUartPsPutByte(u32val&0xff);
		CsrRegister = XUartPs_ReadReg(InstancePtr->Config.BaseAddress,XUARTPS_SR_OFFSET);
	}

}
/****************************************************************************/
/**
*
* This function handles the interrupt when data is in RX FIFO.
*
* @param	InstancePtr is a pointer to the XUartPs instance
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void _2ReceiveDataHandler(XUartPs *InstancePtr)
{
	u32 CsrRegister;
	u32 u32val;
	u32 ReceivedCount = 0U;
	u32 ByteStatusValue, EventData;


	/*
	 * Read the Channel Status Register to determine if there is any data in
	 * the RX FIFO
	 */
	CsrRegister = XUartPs_ReadReg(InstancePtr->Config.BaseAddress,
				XUARTPS_SR_OFFSET);

	/*
	 * Loop until there is no more data in RX FIFO or the specified
	 * number of bytes has been received
	 */
	while((((CsrRegister & XUARTPS_SR_RXEMPTY) == (u32)0))){

		u32val = XUartPs_ReadReg(InstancePtr->Config.BaseAddress, XUARTPS_FIFO_OFFSET);
		if(gpfUartPsPutByte)
			gpfUartPsPutByte(u32val&0xff);
		CsrRegister = XUartPs_ReadReg(InstancePtr->Config.BaseAddress,XUARTPS_SR_OFFSET);
	}

}

/****************************************************************************/
/**
*
* This function handles the interrupt when data has been sent, the transmit
* FIFO is empty (transmitter holding register).
*
* @param	InstancePtr is a pointer to the XUartPs instance
* @param	IsrStatus is the register value for channel status register
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void _2SendDataHandler(XUartPs *InstancePtr, u32 IsrStatus)
{

	/*
	 * If there are not bytes to be sent from the specified buffer then disable
	 * the transmit interrupt so it will stop interrupting as it interrupts
	 * any time the FIFO is empty
	 */
	if (InstancePtr->SendBuffer.RemainingBytes == (u32)0) {
		XUartPs_WriteReg(InstancePtr->Config.BaseAddress,
				XUARTPS_IDR_OFFSET,
				((u32)XUARTPS_IXR_TXEMPTY | (u32)XUARTPS_IXR_TXFULL));

		/* Call the application handler to indicate the sending is done */
		InstancePtr->Handler(InstancePtr->CallBackRef,
					XUARTPS_EVENT_SENT_DATA,
					InstancePtr->SendBuffer.RequestedBytes -
					InstancePtr->SendBuffer.RemainingBytes);
	}

	/* If TX FIFO is empty, send more. */
	else if((IsrStatus & ((u32)XUARTPS_IXR_TXEMPTY)) != (u32)0) {
		(void)XUartPs_SendBuffer(InstancePtr);
	}
	else {
		/* Else with dummy entry for MISRA-C Compliance.*/
		;
	}
}

/****************************************************************************/
/**
*
* This function handles modem interrupts.  It does not do any processing
* except to call the application handler to indicate a modem event.
*
* @param	InstancePtr is a pointer to the XUartPs instance
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void _2ModemHandler(XUartPs *InstancePtr)
{
	u32 MsrRegister;

	/*
	 * Read the modem status register so that the interrupt is acknowledged
	 * and it can be passed to the callback handler with the event
	 */
	MsrRegister = XUartPs_ReadReg(InstancePtr->Config.BaseAddress,
			  XUARTPS_MODEMSR_OFFSET);

	/*
	 * Call the application handler to indicate the modem status changed,
	 * passing the modem status and the event data in the call
	 */
	InstancePtr->Handler(InstancePtr->CallBackRef,
				  XUARTPS_EVENT_MODEM,
				  MsrRegister);

}
/** @} */
