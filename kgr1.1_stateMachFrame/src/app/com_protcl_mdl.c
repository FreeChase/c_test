/******************************************************************************
 * Copyright (C) 2024 ZS Development Team, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @file com_protcl_mdl..c 
 * @brief	本模块主要用于按照北斗通信协议进行扩展协议数据打包和解包
 * @version 0.0.1
 * Change Logs:
 * Date           Author       Notes
 * 2025-5-50       syd			0.0.1
******************************************************************************/

#include "zs_printf.h"
#include "shell.h"

#include "antijam_fpga.h"


#include "com_protcl_mdl.h"

#include "string.h"
#include "xparameters.h"

/*
*函数名称： com_data_ad_head_send()
*函数功能： 进行AD数据帧头发送
*输入参数：                    
*			samNum: 数据点数；
*			chnNum: 通道个数；
*			signlebytnum: 单包数据字节长度；
*			totalpackgCnt: 总包数；
*			curpackgCnt: 当前包数；
*			
*返回值：   
*			0: 数据发送成功
*			1: 数据发送失败
**********************************
*/
static unsigned char gSendBufText[64];
static int com_data_ad_head_send(unsigned int samNum,unsigned int chnNum,
									unsigned int signlebytnum,
									unsigned int totalpackgCnt,
									unsigned curpackgCnt)
{

	unsigned char sizeBuf = 0;
	unsigned char i;
	//进行包头数据发送
	sprintf(gSendBufText,"%s,%d,%d,%d,%d,%d,",AD_DATA_TRANS_HEAD,signlebytnum,
		totalpackgCnt,
		curpackgCnt,
		samNum,
		chnNum);
	sizeBuf = strlen(gSendBufText);
	for(i = 0;i < sizeBuf ; i++)
	{
		XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR,gSendBufText[i]);
	}

	return 0;
	
}


/*
*函数名称： com_data_ad_end_send()
*函数功能： 进行AD数据帧尾发送
*输入参数：                    
*			checksum: 校验和
*			
*返回值：   
*			0: 数据发送成功
*			1: 数据发送失败
**********************************
*/
static int com_data_ad_end_send(unsigned short checksum)
{
	unsigned char tempBuf[64];

	//进行包头数据发送
	dx_kprintf("0x%x,0x0D0A\r\n",checksum);

	return 0;
	
}									


/*
*函数名称： com_data_ad_send()
*函数功能： 进行AD数据发送
*输入参数：                    
*			samNum: 数据点数；
*			chnNum: 通道个数；
*			
*返回值：   
*			0: 数据发送成功
*			1: 数据发送失败
**********************************
*/
int com_data_ad_send(unsigned int tmptotalpackgCnt)
{
	unsigned int tmpsignleByte;
	unsigned int tmpcurrentCnt;
	unsigned int i,j;
	u32 u32SamCtrlReg;
	u32 u32SamData  ;
	u32 u32SamData1[4]  ;
	u8 * pu8databuf;
	u16 * pu16databuf;
	u16 crcchecksum = 0;
	float tmpTemp;

	//
	pu8databuf = &u32SamData;
	pu16databuf = &u32SamData;
	tmpsignleByte = 2*8*32; 	//单包数据512字节；
	u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);

	while((u32SamCtrlReg &((u32)(1)<<21)))
	{
		u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
	}

		//进行包头数据发送
	dx_kprintf("$JAMRG,0x%x,0x0D0A \r\n",u32SamCtrlReg);

	for(i = 0; i < tmptotalpackgCnt;  i++)
	{
		com_data_ad_head_send(0,0,tmpsignleByte,tmptotalpackgCnt,i);
		for(j = 0;j < tmpsignleByte/4;j++)
		{
			u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i*tmpsignleByte/4 + j);
			crcchecksum = crcchecksum^pu16databuf[0];
			crcchecksum = crcchecksum^pu16databuf[1];
			outbyte(pu8databuf[0]);
			outbyte(pu8databuf[1]);
			outbyte(pu8databuf[2]);
			outbyte(pu8databuf[3]);

			
		}
		com_data_ad_end_send(crcchecksum);
		
	}
#if 0

	for(int i=0;i<2048;i++)
	{
		for (int j=0; j<4; j++)
		{
			u32SamData1[j] = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i*4 + j);
		}
		dx_kprintf("%d %d %d %d %d %d %d %d\r\n",
				(short)(u32SamData1[0]&0xffff), (short) ((u32SamData1[0]>>16)&0xffff) ,
				(short)(u32SamData1[1]&0xffff), (short) ((u32SamData1[1]>>16)&0xffff) ,
				(short)(u32SamData1[2]&0xffff), (short) ((u32SamData1[2]>>16)&0xffff) ,
				(short)(u32SamData1[3]&0xffff), (short) ((u32SamData1[3]>>16)&0xffff)) ;

	}
#endif
	
}
