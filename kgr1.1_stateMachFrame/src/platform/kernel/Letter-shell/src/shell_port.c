#include "shell.h"
#include "stdint.h"
#include "xparameters.h"
#include "xuartlite_l.h"
#include "uart_service.h"
#include "xuartlite.h"
#include "fifo.h"
#include "../usr/userTask.h"

char shellBuffer[512];

Shell shell;


static short sshellRead(char * buf, unsigned short len )
{
	char ret = 1;
	char ch;
	//ret = gDbgUart.pfRecvByte(&ch);
	//注意，只实现单字节访问
	// ret = dequeue(&ch);
	ret = PLUART_RX_BYTE_GET(PLUART_INDEX_0_PC,ch);
	if(ret == TRUE)
	{
		*buf=ch;
		return 1;
	}
	return 0;
}

static short sshellWrite(char * buf, unsigned short len )
{
    uint16_t send  = 0;
    while (send < len)
    {
    	XUartLite_SendByte(XPAR_FPGA_DEBUG_UART1_BASEADDR, buf[send++]);
        // FUartPs_write(&g_UART, buf[send++]);
        //         while((FUartPs_getLineStatus(&g_UART) & Uart_line_thre) != Uart_line_thre);
        // gUartShell.pfSendByte(buf[send++]);   

    }

    return len;
}



void ShellInit(void)
{
    shell.read = sshellRead;
    shell.write = sshellWrite;
    shellInit(&shell, shellBuffer, 512);
}
