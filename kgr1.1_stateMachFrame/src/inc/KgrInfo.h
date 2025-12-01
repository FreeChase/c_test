/*
 * Copyright (c) 2020-2025, ZS Development Team
 *
 * @file KgrInfo.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2024-05-21     Andy         the first version
 */

#ifndef __KGR_INFO_H__
#define __KGR_INFO_H__


#ifdef __cplusplus
extern "C" {
#endif
#define ADDR_INIT 0x3d1f80
#define fip 12799999
#define LPF1_LPF4_NUM   (80) 
#define PI  3.1415926

//#define	KGR_B1I_B1C_L1_M        1
#define  KGR_G1__M        1

/* Anti interference frequency information */
#define HW_KGR_LC_B3                    0x0 /*低成本四通道抗干扰模块 B3抗干扰*/
#define HW_KGR_LC_B1                    0x1 /*低成本四通道抗干扰模块 B1抗干扰*/
#define HW_KGR_LC_S                     0x2 /*低成本四通道抗干扰模块 S抗干扰*/


/*寄存器地址*/
#define FPGA_STAP_BYPASSSET_REG			   (132)

	void Collect();
	extern int YZData;
	extern int GL_flag;
	int PID_Controller(int setpoint, int measured_value, int dt);
	void ATT_ALL_SET(int value);
	void ADC_DATA_display(int* Value_data);
	void read_power(int* value, int* mean);
	int find_max_value(int* value);
	int find_min_value(int* value);
	int weight_fix(int num);
	void weight_write(int addr, int* weight, int num);
	void ddc_config(int Fre);
	void weight_Set();
	void StapMatrixNew_init();
	int stap_sjr_length_set(int setLenIndex);
	int stap_sjr_refresh_time(int ustime);
	int stap_sjr_enable(int setenable);
	void subBandDiv_bypass();
	void subBandDiv_enable();
	void Weight_load_init();


void  Weight_hand_write_tst();


	extern int BC_buff[4];
	static int B3I_weight_dacfit[LPF1_LPF4_NUM] = { 0 };
	static int B1_weight_dacfit[LPF1_LPF4_NUM] = { 0 };



	static int S_weight_dacfit[48] = { 0 };
	static int hand_weight[48] = { 0 };
	extern int Att_set;
	extern int ic_data[2];
	extern int Att_data;
	extern unsigned char PowerShowSet_flag;
	extern unsigned char DCShowSet_flag;
	extern int ch;
	extern int Ch_set;
	extern int GXB;

#ifdef __cplusplus
}
#endif

#endif
