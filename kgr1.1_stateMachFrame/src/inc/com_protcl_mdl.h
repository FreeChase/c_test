/******************************************************************************
 * Copyright (C) 2024 ZS Development Team, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @file com_protcl_mdl..h 
 * @brief	本模块主要用于按照北斗通信协议进行扩展协议数据打包和解包
 * @version 0.0.1
 * Change Logs:
 * Date           Author       Notes
 * 2025-5-50       syd			0.0.1
******************************************************************************/
#ifndef COM_PROTCL_MDL_ADFSH
#define COM_PROTCL_MDL_ADFSH

#define AD_DATA_TRANS_HEAD     "$JAMAD"

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
int com_data_ad_send(unsigned int tmptotalpackgCnt);


#endif