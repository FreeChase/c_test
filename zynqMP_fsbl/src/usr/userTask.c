#include <stdio.h>

//#include "antijam_fpga.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
//#include "shell.h"

//#include "uart_service.h"
//#include "zs_printf.h"
//#include "KgrInfo.h"
#include "userTask.h"
#include "Ymodem/Ymodem.h"
//#include "userStateMach.h"


int _putchar_shell(char a);
int _isr_putchar_pc(char a);
int _isr_putchar_ublox(char a);

int _putchar(void* p,char a);
int _getchar(void* p,char *a);
int _sendbyte(char a);


static int _uart_pl_putchar(void *p, char a);
static int _uart_pl_putcharMulti(void *p, char * pdata, int length);
static int _uart_pl_putcharISR(void *p, char a);
static int _uart_pl_getchar(void *p,char *a);



extern T_SMCtrl gCtrl;

#if 1
//*******************************/
extern int (* gpfUartPutByte[7] )(char ch); 
extern int (* gpfUartPsPutByte )(char ch); 
// T_UartCtrl gUartPL[7];
T_UartCtrl gUartPSCtrl;

static char uartPsbuf[4096] = {0};
u8 u8buf_strsplit[128]={0};




extern void ShellAsciiInit(void);

extern void StateMachInit(void);
void PreInit(void)
{

	
    memset((char*)&gUartPSCtrl,0,sizeof(gUartPSCtrl));

	// memset((char*)&gUartPL2,0,sizeof(gUartPL2));
    initializeBuffer(&gUartPSCtrl.rx.cirbuf, uartPsbuf, sizeof(uartPsbuf));
    // initializeBuffer(&gUartPSCtrl.tx.cirbuf, uartbuf_tx_pc, sizeof(uartbuf_tx_pc));
    gUartPSCtrl.rx.putByte = _uart_pl_putchar;
    gUartPSCtrl.rx.putMultiByte = _uart_pl_putcharMulti;
    gUartPSCtrl.rx.getByte = _uart_pl_getchar;
    gUartPSCtrl.rx.putByteIsr = _uart_pl_putcharISR;
   
    // gUartPSCtrl.tx.putByte = _uart_pl_putchar;
    // gUartPSCtrl.tx.putMultiByte = _uart_pl_putcharMulti;
    // gUartPSCtrl.tx.getByte = _uart_pl_getchar;
    // gUartPSCtrl.tx.putByteIsr = _uart_pl_putcharISR;
    // gUartPL2.pfSendByte = _uart_pl2_sendbyte;
    // gUartPL2.pfRecvByte = _uart_pl2_getchar; //

    gpfUartPsPutByte = _isr_putchar_pc;


	//* State Machine Init
	StateMachInit();
	YmodemInit();
	ShellAsciiInit();
	
}

void PreLaterInit(void)
{
	gCtrl.lv1sm_usr_data.u32PowerUptick = tick_get();
}


void TaskProcess(void)
{
	int ret,event;
	while (1)
	{
#if 1
		// 1. 检查外部输入并生成事件
		ret = Event_Get(&event);
		// printf("event : %d\n",event);
		if ( ret ) {
			//* get valid event
			Lv1ProcessEvent(&gCtrl.lv1sm, event);

			if(event == EVENT_CMD_POWERUP_TIMEOUT)
			{
				//* continue jump app logic
				xil_printf("Timeout Event\r\n");
				return;
			}
		}
		else
		{
			//*periodly handle handle
			event = EVENT_IDLE;
			Lv1ProcessEvent(&gCtrl.lv1sm, event);
		}

#endif

		// Lv1ProcessEvent( &gCtrl.lv1sm, 0);
		// shellTask((void *)&shell);
	}
}



static int _uart_pl_putcharISR(void *p, char a)
{
    return writeBufferNoMutex( p, a);
}

static int _uart_pl_putchar(void *p, char a)
{
    return writeBufferMutex( p, a);
}

static int _uart_pl_putcharMulti(void *p, char * pdata, int length)
{
    return writeBufferMultipleMutex( p, pdata, length);
}


static int _uart_pl_getchar(void *p,char *a)
{
    return readBufferMutex( p, a);
}


int _isr_putchar_pc(char a)
{
    return writeBufferNoMutex(&gUartPSCtrl.rx.cirbuf, a);
}


#endif
