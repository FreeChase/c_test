/*
 * Copyright (c) 2020-2025, ZS Development Team
 *
 * @file KgrInfo.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2025-05-21     Andy         the first version
 */
#include <math.h>
#include <string.h>
#include "KgrInfo.h"

#include "dev_manager.h"


#define	KGR_B1I_B1C_L1_M        1
#define  KGR_G1__M        1
#define   SECONDE_DESC_LO        (62000)
//#define    LOW_POWER_ALG_ENABLE

#define    KGR_SUB_BAND_CNT         (4)

//#define KGRINFO_DBG    
#define DBG_TAG    "KGRINFO"
#ifdef KGRINFO_DBG
#define DBG_LVL    DBG_LOG
#else
#define DBG_LVL    DBG_INFO
#endif // KGRINFO_DBG


float Kp = 0.003;
float Ki = 0.0;
float Kd = 0.0005;

float integral = 0;
float previous_error = 0;
int init_point = 0;
unsigned char g_KgrFreqId = HW_KGR_LC_B1;


void dx_dly_ms(unsigned int tempDealy)
{
	int i;
	for(i = 0; i < tempDealy*10; i++);
}
#if 0
int PID_Controller(int setpoint, int measured_value, int dt)
{
	float error = (setpoint - measured_value);

	if ((error < 1000) && ((error > -1000)))
		error = 0;
	//计算积分
	integral += error * dt;
	if (integral >= 10000)
		integral = 10000;
	if (integral <= -10000)
		integral = -10000;
	//计算微分
	float derivative = (error - previous_error) / dt;
	//计算控制信号
	float control_signal = (Kp * error + Ki * integral + Kd * derivative);
	float out_signal = init_point - control_signal;

	if (out_signal > 250)
		out_signal = 254;
	if (out_signal <= 0)
		out_signal = 0;
	previous_error = error;
	init_point = (int)out_signal;
	return (int)(out_signal);

}
#endif

#if 0
void read_power(int* value, int* mean)
{
	value[0] = (long long)mcu_to_fpga_read_kgr_reg(12) * 1024 / fip;
	value[1] = (long long)mcu_to_fpga_read_kgr_reg(13) * 1024 / fip;
	value[2] = (long long)mcu_to_fpga_read_kgr_reg(14) * 1024 / fip;
	value[3] = (long long)mcu_to_fpga_read_kgr_reg(15) * 1024 / fip;
	value[4] = (long long)mcu_to_fpga_read_kgr_reg(16) * 1024 / fip;
	value[5] = (long long)mcu_to_fpga_read_kgr_reg(17) * 1024 / fip;
	value[6] = (long long)mcu_to_fpga_read_kgr_reg(18) * 1024 / fip;
	value[7] = (long long)mcu_to_fpga_read_kgr_reg(19) * 1024 / fip;
	value[8] = (long long)mcu_to_fpga_read_kgr_reg(20);

	mean[0] = (s32)mcu_to_fpga_read_kgr_reg(21) * 1024 / fip;
	mean[1] = (s32)mcu_to_fpga_read_kgr_reg(22) * 1024 / fip;
	mean[2] = (s32)mcu_to_fpga_read_kgr_reg(23) * 1024 / fip;
	mean[3] = (s32)mcu_to_fpga_read_kgr_reg(24) * 1024 / fip;
	mean[4] = (s32)mcu_to_fpga_read_kgr_reg(25) * 1024 / fip;
	mean[5] = (s32)mcu_to_fpga_read_kgr_reg(26) * 1024 / fip;
	mean[6] = (s32)mcu_to_fpga_read_kgr_reg(27) * 1024 / fip;
	mean[7] = (s32)mcu_to_fpga_read_kgr_reg(28) * 1024 / fip;
}
#endif

int find_max_value(int* value)
{
	int max_value = value[0];
	int max_ch = 0;
	for (int i = 0; i < 7; i++)
	{
		if (value[i] > max_value)
		{
			max_value = value[i];
			if (i <= 3)
				max_ch = i;
		}
	}
	return max_value * 10 + max_ch;
}
int find_min_value(int* value)
{
	int min_value = value[0];
	int min_ch = 0;
	for (int i = 0; i < 4; i++)
	{
		if (value[i] < min_value)
		{
			min_value = value[i];
			min_ch = i;
		}
	}
	return min_ch;
}
int weight_fix(int num)
{
	if (mcu_to_fpga_read_kgr_reg(88) != 3)
	{
		for (int i = 0; i < num; i++) {
			mcu_to_fpga_write_kgr_reg(88, (1 << 17));
			mcu_to_fpga_write_kgr_reg(88, (0 << 17));
			dx_dly_ms(10);
			if (mcu_to_fpga_read_kgr_reg(88) == 3)
				break;
		}
	}
	return mcu_to_fpga_read_kgr_reg(88);
}
int weight_int2x_fix(int num)
{
	if (mcu_to_fpga_read_kgr_reg(87) != 3)
	{
		for (int i = 0; i < num; i++) {
			mcu_to_fpga_write_kgr_reg(87, (1 << 17));
			mcu_to_fpga_write_kgr_reg(87, (0 << 17));
			dx_dly_ms(10);
			if (mcu_to_fpga_read_kgr_reg(87) == 3)
				break;
		}
	}
	return mcu_to_fpga_read_kgr_reg(87);
}
void weight_write(int addr, int* weight, int num)
{
	//weight_fix(48);

	for (int i = 0; i < num; i++) {
		mcu_to_fpga_write_kgr_reg(addr, ((weight[i] & 0x3ffff) | (i << 20) | (0 << 31)));
		mcu_to_fpga_write_kgr_reg(addr, ((weight[i] & 0x3ffff) | (i << 20) | (1 << 31)));
		mcu_to_fpga_write_kgr_reg(addr, ((weight[i] & 0x3ffff) | (0 << 31)));

		mcu_to_fpga_write_kgr_reg(addr, ((weight[i] & 0x3ffff) | ((2 * num - 1 - i) << 20) | (0 << 31)));
		mcu_to_fpga_write_kgr_reg(addr, ((weight[i] & 0x3ffff) | ((2 * num - 1 - i) << 20) | (1 << 31)));
		mcu_to_fpga_write_kgr_reg(addr, ((weight[i] & 0x3ffff) | ((2 * num - 1 - i) << 20) | (0 << 31)));
	}

}

void ddc_config(int Fretmp)
{
	unsigned int configtemp = 0;
	double tempconfig = 0;
	tempconfig = (double)Fretmp / 12400 * 4294967296.0;
	configtemp = (unsigned int)tempconfig;
	dx_kprintf("ddc config 0x%x %d %lf\r\n", configtemp,configtemp,tempconfig);
	mcu_to_fpga_write_kgr_reg(143, (unsigned int)configtemp);
}
int subBandNum_set(unsigned int numVal)
{

	unsigned int setVal;
	setVal = mcu_to_fpga_read_kgr_reg(129); //寄存器进行了修改
	setVal = setVal & (0xFFF8FFFF); //bit 16 17 18 设置成零
	setVal = setVal | ((numVal & 0x7) << 16);
	mcu_to_fpga_write_kgr_reg(129, setVal);
	return  0;

}

void PLfpga_filter_config(int addr, int* weight, int num)
{
	u32 fir_ready_status = 0;
	unsigned int i = 0;
	mcu_to_fpga_write_kgr_reg(addr, 0);
	mcu_to_fpga_write_kgr_reg(addr + 1, (0 << 8) | 0x0);

	int tempVal = 0;
	u32* ptempVal1 = 0;
	u32 tempVal2 = 0;
	u32 tempVal3 = 0;

	ptempVal1 = &tempVal;
	for (i = 0; i < num; i++)
	{
		/*reload data*/
		/*写数据准备就绪*/
		fir_ready_status = mcu_to_fpga_read_kgr_reg(addr);
		while (((fir_ready_status >> 2) && 0x1) == 0)
		{
			fir_ready_status = mcu_to_fpga_read_kgr_reg(addr);
		}

		tempVal = weight[i];

		tempVal2 = *ptempVal1;
		tempVal3 = ((tempVal2 & 0xFFFFFF) << 8);

		mcu_to_fpga_write_kgr_reg(addr, (tempVal3 | (0x1)));
		mcu_to_fpga_write_kgr_reg(addr, (tempVal3 & (0xFFFFFFFE)));

		tempVal2 = mcu_to_fpga_read_kgr_reg(addr);
		if((tempVal3+4)!=(tempVal2))
		dx_kprintf("reg [%d] 0x%x read 0x%x\r\n", addr, tempVal3, tempVal2);


	}
	dx_kprintf("XILINX fir reload data write done \r\n");

	/*系数写完执行config valid*/
	fir_ready_status = mcu_to_fpga_read_kgr_reg(addr + 1);
	while (((fir_ready_status >> 1) && 0x1) == 0)
	{
		fir_ready_status = mcu_to_fpga_read_kgr_reg(addr + 1);
	}

	dx_dly_ms(1);
	mcu_to_fpga_write_kgr_reg(addr + 1, (0 << 8) | 0x1);
	mcu_to_fpga_write_kgr_reg(addr + 1, 0);
	dx_kprintf("XILINX fir config write done \r\n");

}
void subBandDdc_freqconfig(int Fre, unsigned int subBandNum)
{
	int config = 0;
	config = (double)((double)Fre / (SECONDE_DESC_LO) * 4294967296);
	mcu_to_fpga_write_kgr_reg(61 + subBandNum, (((int)config)));
}
void weight_Set()
{
	unsigned int setVal;


	switch (g_KgrFreqId)
	{
	case HW_KGR_LC_B3:
	{
		Ch_set = 0;
		ddc_config(4652);
		subBandDdc_freqconfig(4000, 0);
		subBandDdc_freqconfig(-4000, 1);


		////weight_write(148,gFilter_coef[0],LPF1_LPF4_NUM);
		//PLfpga_filter_config(180, gFilter_coef_dec[0], LPF1_LPF4_NUM);
		//PLfpga_filter_config(182, gFilter_coef_dec[3], LPF1_LPF4_NUM);//lpf2

		//PLfpga_filter_config(185, gFilter_coef_lpf_38_2x, LPF1_LPF4_NUM);//lpf3

		//PLfpga_filter_config(188, gFilter_coef_lpf_8_5x, LPF1_LPF4_NUM);//lpf4
		//weight_write(153, B3I_weight_dacfit, 48);
		dx_kprintf("Set fre as:54_B3I\r\n");
		break;
	}
	case HW_KGR_LC_B1:
	{
		//1561.098   1575.42    31.098    35.19    39.282   45.42   51.558   
		ddc_config(9300);//第一级
		//subBandDdc_freqconfig(-20500,0); //[-20.5e6 -7.58e6 19e6];
		//subBandDdc_freqconfig(-9000,1);
		//subBandDdc_freqconfig(19000,2);

		subBandDdc_freqconfig(-21902, 0); //[-20.5e6 -7.58e6 19e6];
		subBandDdc_freqconfig(-7580, 1);
		subBandDdc_freqconfig(17000, 2);
		subBandDdc_freqconfig(21000, 3);
		subBandDdc_freqconfig(-16902, 133);
		subBandDdc_freqconfig(-2580, 134);


		//ddc_config(3110);
		//subBandDdc_freqconfig(0, 0);
		//subBandDdc_freqconfig(0, 1);

		////weight_write(148,gFilter_coef[1],LPF1_LPF4_NUM);
		//PLfpga_filter_config(180, gFilter_coef_dec[1], LPF1_LPF4_NUM);
		//PLfpga_filter_config(182, gFilter_coef_dec[2], LPF1_LPF4_NUM);//lpf2 这个要变顺序

		//PLfpga_filter_config(185, gFilter_coef_lpf_2_2x, LPF1_LPF4_NUM);//lpf3
		//PLfpga_filter_config(188, gFilter_coef_lpf_93_5x, LPF1_LPF4_NUM);  //lpf4

		//weight_write(153, B1_weight_dacfit, 48);
		dx_kprintf("Set fre as 54_B1\r\n");
		break;
	}
	}

	//设置截位大小为2：06-10:默认值修改为300 子带抽取截位
	setVal = mcu_to_fpga_read_kgr_reg(65);
	setVal = setVal | (0x200);
	mcu_to_fpga_write_kgr_reg(65, setVal);

	//设置截位大小为2：06-10:默认值修改为3 子带差值截位
	setVal = mcu_to_fpga_read_kgr_reg(67);
	setVal = setVal | (0x4);
	mcu_to_fpga_write_kgr_reg(67, setVal);


	//使能频率控制字
	setVal = mcu_to_fpga_read_kgr_reg(60);
	setVal = setVal & (0xFFFFFFFE);
	mcu_to_fpga_write_kgr_reg(60, setVal);
	dx_dly_ms(1);
	setVal = mcu_to_fpga_read_kgr_reg(60);
	setVal = setVal | (0x1);
	mcu_to_fpga_write_kgr_reg(60, setVal);
	dx_dly_ms(1);
	setVal = mcu_to_fpga_read_kgr_reg(60);
	setVal = setVal & (0xFFFFFFFE);
	mcu_to_fpga_write_kgr_reg(60, setVal);

	mcu_to_fpga_write_kgr_reg(131, 0x8600); //默认处于低刷新频率


	//将自带配置为两个
	subBandNum_set(KGR_SUB_BAND_CNT - 1);

	subBandDiv_enable();

}
unsigned int gSjrSetLenIndex = 0;
int stap_sjr_length_set(int setLenIndex)
{
	unsigned int tmpSetVal = 0;

	gSjrSetLenIndex = setLenIndex;

	//将该寄存器值回读
	tmpSetVal = mcu_to_fpga_read_kgr_reg(156);
	tmpSetVal = tmpSetVal & 0xFFFFFFF8;

	//将该寄存器值或上设置值
	tmpSetVal = tmpSetVal | (setLenIndex & 0x7);
	dx_kprintf("set 0x%x \r\n", tmpSetVal);

	//将该值赋值到寄存器
	mcu_to_fpga_write_kgr_reg(156, tmpSetVal);


	return 0;


}
int stap_sjr_refresh_time(int ustime)
{
	unsigned int tmpSetVal = 0;
	unsigned int caltimetmp = 0;
	float ustimef32 = 0;

	ustimef32 = (float)ustime;

	ustimef32 = ustimef32 * 1000 / 4.033;

	tmpSetVal = (unsigned int)ustimef32;

	dx_kprintf("set %d  %f \r\n", tmpSetVal, ustimef32);

	caltimetmp = 8192 * 2 ^ (gSjrSetLenIndex);

	if (tmpSetVal < caltimetmp * 8)
	{
		//打印异常
		dx_kprintf("refresh time too little !![time] =  %d [timeTick] = %d  [lenIndex] = %d timeTick should> 8192*2^(gSjrSetLenIndex)*8;\r\n",
			(u32)ustime, tmpSetVal, caltimetmp);

		return -1;
	}


	//将该值赋值到寄存器
	mcu_to_fpga_write_kgr_reg(157, tmpSetVal & 0xFFFFFF);


	return 0;
}
int stap_sjr_enable(int setenable)
{
	unsigned int tmpSetVal = 0;


	//将该寄存器值回读
	tmpSetVal = mcu_to_fpga_read_kgr_reg(156);
	tmpSetVal = tmpSetVal & 0x7FFFFFFF;

	//将该寄存器值或上设置值
	tmpSetVal = tmpSetVal | ((setenable & 0x1) << 31);

	//将该值赋值到寄存器
	mcu_to_fpga_write_kgr_reg(156, tmpSetVal);


	return 0;
}


void Weight_load_init()
{
	u32 u32Regs1, u32Regs2, Zlset = 0;
	u32 subcnt = 0;
	int i;

	Zlset = 24;

	for (subcnt = 0; subcnt < 2; subcnt++)
	{
		mcu_to_fpga_write_kgr_reg(130, 0 | (1 << 8) | (1 << 16) | (0 << 24));
		u32Regs1 = mcu_to_fpga_read_kgr_reg(132);

		mcu_to_fpga_write_kgr_reg(132, u32Regs1 | (1 << 8));
		//FpgaWrite(132,0x1050100);
		u32Regs2 = mcu_to_fpga_read_kgr_reg(132);
		dx_kprintf("Weight_load_init:%x %x\r\n", u32Regs1, u32Regs2);
		//for (i = 0; i < 128; i++) {

			//if (i == Zlset)
			//{
		mcu_to_fpga_write_kgr_reg(134, 65536);
		mcu_to_fpga_write_kgr_reg(135, 0);
		//	}
			//else
			//{
			//	mcu_to_fpga_write_kgr_reg(134, 0);
			//	mcu_to_fpga_write_kgr_reg(135, 0);
			//}

		mcu_to_fpga_write_kgr_reg(133, ((subcnt & 0x3) << 23) | (Zlset << 16) | (0 << 9) | (0 << 8) | 0);
		mcu_to_fpga_write_kgr_reg(133, ((subcnt & 0x3) << 23) | (Zlset << 16) | (0 << 9) | (1 << 8) | 0);
		mcu_to_fpga_write_kgr_reg(133, ((subcnt & 0x3) << 23) | (Zlset << 16) | (0 << 9) | (0 << 8) | 0);
		//dx_kprintf("%d %d\r\n", (s32)mcu_to_fpga_read_kgr_reg(136),(s32)mcu_to_fpga_read_kgr_reg(137));
	//}
		mcu_to_fpga_write_kgr_reg(132, u32Regs1);
		mcu_to_fpga_write_kgr_reg(130, 0 | (1 << 8) | (1 << 16) | (0 << 24));
	}
}
void subBandWeight_set(unsigned int subBanNum, unsigned int setFlag)
{
	u32 u32Regs1, u32Regs2, Zlset = 0;
	u32Regs1 = mcu_to_fpga_read_kgr_reg(132);

	Zlset = 24;

	mcu_to_fpga_write_kgr_reg(132, u32Regs1 | (1 << 8));
	//FpgaWrite(132,0x1050100);
	u32Regs2 = mcu_to_fpga_read_kgr_reg(132);
	dx_kprintf("Weight_load_init:%x %x\r\n", u32Regs1, u32Regs2);

	if (setFlag == 0)
	{
		mcu_to_fpga_write_kgr_reg(134, 65536);
		mcu_to_fpga_write_kgr_reg(135, 0);
	}
	else
	{
		mcu_to_fpga_write_kgr_reg(134, 0);
		mcu_to_fpga_write_kgr_reg(135, 0);
	}
	mcu_to_fpga_write_kgr_reg(133, ((subBanNum & 0x3) << 23) | (Zlset << 16) | (0 << 9) | (0 << 8) | 0);
	mcu_to_fpga_write_kgr_reg(133, ((subBanNum & 0x3) << 23) | (Zlset << 16) | (0 << 9) | (1 << 8) | 0);
	mcu_to_fpga_write_kgr_reg(133, ((subBanNum & 0x3) << 23) | (Zlset << 16) | (0 << 9) | (0 << 8) | 0);

	mcu_to_fpga_write_kgr_reg(132, u32Regs1);
	mcu_to_fpga_write_kgr_reg(130, 0 | (1 << 8) | (1 << 16) | (0 << 24));


}
void subBandDiv_bypass()
{
	unsigned int setVal;
	//自带配置为1个
	subBandNum_set(KGR_SUB_BAND_CNT - 1);

	//打开所有旁路；
	//打开ddc旁路,写0关闭旁路，写1开启旁路
	setVal = mcu_to_fpga_read_kgr_reg(60);

	setVal = setVal | (0x80000000);
	mcu_to_fpga_write_kgr_reg(60, setVal);

	//打开子带抽取滤波旁路,写0关闭旁路，写1开启旁路
	setVal = mcu_to_fpga_read_kgr_reg(65);
	setVal = setVal | (0x1);
	mcu_to_fpga_write_kgr_reg(65, setVal);

	//打开子带插值滤波旁路,写0关闭旁路，写1开启旁路
	setVal = mcu_to_fpga_read_kgr_reg(67);
	setVal = setVal | (0x80000000);
	mcu_to_fpga_write_kgr_reg(67, setVal);

	//打开udc旁路,写0关闭旁路，写1开启旁路
	mcu_to_fpga_write_kgr_reg(69, 1);


	//权重重新初始化
	subBandWeight_set(1, 1);

}
void StapMatrixNew_init()
{

/******************************链路配置******************************/
	mcu_to_fpga_write_kgr_reg(100, 0);
	mcu_to_fpga_write_kgr_reg(100, 2);
	mcu_to_fpga_write_kgr_reg(100, 0);			//数据采集


	mcu_to_fpga_write_kgr_reg(11, fip);			//ADC增益功率长度	
	mcu_to_fpga_write_kgr_reg(10, 1);			//启动ADC功率统计

	mcu_to_fpga_write_kgr_reg(32, ch);			//抗干扰算法主路选择：设置范围0-7，对应0-7路ADC数据（调零主阵选择）
	mcu_to_fpga_write_kgr_reg(152, 1 | 1 << 4); //DAC滤波控制 （1 | 1 << 4）：启用滤波，滤波启用旁路

	mcu_to_fpga_write_kgr_reg(99, 1 | 0 << 4 | 0 << 8);		//DAC输出控制
	mcu_to_fpga_write_kgr_reg(147, 0x233);					//第二次次抽取滤波截位（5倍抽取）
	mcu_to_fpga_write_kgr_reg(141, 0x800);					//DAC输出增益控制门限值
	mcu_to_fpga_write_kgr_reg(7, 0x3210);					//？
	mcu_to_fpga_write_kgr_reg(145, 0x3210);					//？
	mcu_to_fpga_write_kgr_reg(149, 0x3);					//插值滤波输出截位
	mcu_to_fpga_write_kgr_reg(151, 0x1);					//上变频控制
	mcu_to_fpga_write_kgr_reg(193, 0x33);					//？


/******************************算法配置******************************/
	mcu_to_fpga_write_kgr_reg(128, (0x678 << 16) | (7 << 8) | 2);						//Stap阵元参数寄存器

	/********
	*需要计算的子矩阵个数：
	*sum(1:3) * (3) - 1 = 17 bit24;
	*sum(1:3) * (4) - 1 = 23 bit24;
	**************************************/
	mcu_to_fpga_write_kgr_reg(129, ((6 * KGR_SUB_BAND_CNT - 1) << 24) | 0x10000 | 23);	//Stap矩阵大小寄存器
	//bd21_fpga_att_write(129, (11 << 24) | 0x10000 | 23);


	mcu_to_fpga_write_kgr_reg(130, 0 | (0 << 8) | (0 << 16) | (0 << 24));				//Stap矩阵计算控制寄存器
	mcu_to_fpga_write_kgr_reg(130, 1 | (0 << 8) | (0 << 16) | (0 << 24));
	mcu_to_fpga_write_kgr_reg(130, 0 | (0 << 8) | (0 << 16) | (0 << 24));

	

	mcu_to_fpga_write_kgr_reg(132, 0 | (0 << 8) | (7 << 16) | (1 << 24));				//Stap权值配置控制寄存器

	mcu_to_fpga_write_kgr_reg(133, 0 | (0 << 8) | (0 << 9) | (0 << 16));				//Stap权值读写控制寄存器

	Weight_load_init();

	weight_Set();

	/*数据输出延时，用于与权重进行同步*/
	mcu_to_fpga_write_kgr_reg(155, 3198);
	mcu_to_fpga_write_kgr_reg(155, 0);

	/*权重更新时间*/
	mcu_to_fpga_write_kgr_reg(131, 0x8600);

}
void subBandDiv_enable()
{
	unsigned int setVal = 0;
	//自带配置为1个
	subBandNum_set(KGR_SUB_BAND_CNT - 1);

	//关闭所有旁路；
	//关闭ddc旁路,写0关闭旁路，写1开启旁路
	setVal = mcu_to_fpga_read_kgr_reg(60);
	setVal = setVal & (0x7FFFFFFF);
	mcu_to_fpga_write_kgr_reg(60, setVal);

	//关闭子带抽取滤波旁路,写0关闭旁路，写1开启旁路
	setVal = mcu_to_fpga_read_kgr_reg(65);
	setVal = setVal & (0xFFFFFFFE);
	mcu_to_fpga_write_kgr_reg(65, setVal);

	//关闭子带插值滤波旁路,写0关闭旁路，写1开启旁路
	setVal = mcu_to_fpga_read_kgr_reg(67);
	setVal = setVal & (0x7FFFFFFF);
	mcu_to_fpga_write_kgr_reg(67, setVal);

	//关闭udc旁路,写0关闭旁路，写1开启旁路
	mcu_to_fpga_write_kgr_reg(69, 0);
	//重新初始化权重
	Weight_load_init();
}

typedef struct
{
	u8 m_TestMode;    //测试模式//0:正常模式。1:测试模式
	u8 m_mode;        //启停方式//0:手动启停。1:根据数据长度自动停止
	u8 m_position;    //采集位置// 0:重定向后的数据             
	// 1：镜像滤波后的数据
	// 2:抗脉冲后的数据
	// 3:抗窄带后的数据
	// 4:多波束输入数据
	// 5:多波束2bit数据
	// 6:多波束4bit数据
	// 7:重量化后的数据
	u8 m_dec_rate;   //抽取率   // 0:1抽 1:2抽 2:3抽 3:4抽 4:6抽 5:8抽 6:16抽
	u8 m_data_width; //采数位宽 // 4:4bit 8:8bit 16:16bit
	u8 m_requan_width;//重量化位宽//0:2bit 1:4bit 2:12bit
	u8 m_IQ;          //采集支路  // 1:I路 2:Q路 3:I路和Q路
	u8 m_chValid;     //默认7
	u8 m_ch_num;      //采集通道数//1,2,4,8
	u8 m_ch[8];       //采集通道
	u32 m_ppcnt;
	u32 m_intcnt;
	u32 startaddr;
	u32 endaddr;
}sample_param_t;

#if 0
void Collect()
{
	u32 PPCNTRegCurCount = 0;

	u32 val = 0;
	char data_buff[8192] = { 0 };
	int collect_num = 0;
	int addr_frist = 0;
	sample_param_t p_sample_param;


	HW_WRITE_4((0x01900000 + (0x90)), (1 << 4));
	HW_WRITE_4((0x01900000 + (0x90)), 0x0);

	p_sample_param.m_TestMode = 0;
	p_sample_param.m_mode = 1;
	p_sample_param.m_position = 0;
	p_sample_param.m_dec_rate = 0;
	p_sample_param.m_data_width = 4;
	p_sample_param.m_requan_width = 2;
	p_sample_param.m_IQ = 1;
	p_sample_param.m_ch_num = 1;

	p_sample_param.m_chValid = 1;
	p_sample_param.m_ch[0] = 4;


	val = (p_sample_param.m_TestMode << 20) | (p_sample_param.m_position << 4) | (p_sample_param.m_requan_width << 18) |
		(p_sample_param.m_IQ << 12) | (p_sample_param.m_ch_num << 14) | (p_sample_param.m_data_width << 7) | (p_sample_param.m_chValid << 1) |
		(p_sample_param.m_mode);
	HW_WRITE_4((0x01900000 + (0x94)), val);
	val = 0;
	for (int i = 0; i < p_sample_param.m_ch_num; i++)
	{
		val |= (p_sample_param.m_ch[i] & 0x7) << (i * 3);
	}
	HW_WRITE_4((0x01900000 + (0x98)), val);
	HW_WRITE_4((0x01900000 + (0xA0)), 1);
	HW_WRITE_4((0x01900000 + (0xA4)), 1);
	HW_WRITE_4((0x01900000 + (0xA8)), ADDR_INIT);
	HW_WRITE_4((0x01900000 + (0xAC)), ADDR_INIT + 8192);
	HW_WRITE_4((0x01900000 + (0xB0)), 1);
	HW_WRITE_4((0x01900000 + (0xd4)), 0);

	dx_kprintf("\r\nSAMPLE TEST: \r\n0x%X = 0x%08X,\r\n0x%X = 0x%08X,\r\n0x%X = 0x%08X,\r\n0x%X = 0x%08X,\r\n0x%X = 0x%08X,\r\n0x%X = 0x%08X,\r\n0x%X = 0x%08X,\r\n0x%X = 0x%08X,\r\n0x%X = 0x%08X,\r\n0x%X = 0x%08X\r\n",
		0x94, HW_READ_4(0x01900000 + (0x94)),
		0x98, HW_READ_4(0x01900000 + (0x98)),
		0x9c, HW_READ_4(0x01900000 + (0x9c)),
		0xa0, HW_READ_4(0x01900000 + (0xa0)),
		0xa4, HW_READ_4(0x01900000 + (0xa4)),
		0xa8, HW_READ_4(0x01900000 + (0xa8)),
		0xac, HW_READ_4(0x01900000 + (0xac)),
		0xb0, HW_READ_4(0x01900000 + (0xb0)),
		0x90, HW_READ_4(0x01900000 + (0x90)),
		0xcc, HW_READ_4(0x01900000 + (0xcc)));
	if (HW_READ_4((0x01900000 + (0xd4))))
	{
		HW_WRITE_4((0x01900000 + (0xd4)), 0);
	}
	HW_WRITE_4((0x01900000 + (0x90)), 0);
	HW_WRITE_4((0x01900000 + (0x90)), 1);
	HW_WRITE_4((0x01900000 + (0x90)), 0);
	while (HW_READ_4((0x01900000 + (0xd4))) == 0);
	dx_kprintf("!!!!!!!!!!!!!!Collect_start!!!!!!!!!!");

	for (int i = 0; i < 8192; i++) {
		data_buff[i] = HW_READ_4(ADDR_INIT + i);
	}
	HW_WRITE_4((0x01900000 + (0x90)), 0);
	HW_WRITE_4((0x01900000 + (0x90)), 1 << 1);
	HW_WRITE_4((0x01900000 + (0x90)), 0);
	HW_WRITE_4((0x01900000 + (0x98)), 0);
	HW_WRITE_4((0x01900000 + (0xA0)), 0);
	HW_WRITE_4((0x01900000 + (0xA4)), 0);
	HW_WRITE_4((0x01900000 + (0xA8)), 0);
	HW_WRITE_4((0x01900000 + (0xAC)), 0);
	HW_WRITE_4((0x01900000 + (0xB0)), 0);
	HW_WRITE_4((0x01900000 + (0x94)), 0);
	HW_WRITE_4((0x01900000 + (0xd4)), 0);
	for (int j = 0; j < 512; j++) {
		dx_dly_ms(10U);
		dx_kprintf("ADAC[%d]=%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\r\n",j,
			data_buff[j * 16], data_buff[j * 16+1], data_buff[j * 16 + 2], data_buff[j * 16 + 3],
			data_buff[j * 16 + 4], data_buff[j * 16 + 5], data_buff[j * 16 + 6], data_buff[j * 16 + 7],
			data_buff[j * 16 + 8], data_buff[j * 16 + 9], data_buff[j * 16 + 10], data_buff[j * 16 + 11],
			data_buff[j * 16 + 12], data_buff[j * 16 + 13], data_buff[j * 16 + 14], data_buff[j * 16 + 15]
			);
	}
}

#endif


/**********************************************/
void Weight_hand_write_tst( )
{
	u32 u32Regs1,u32Regs2;
	u32Regs1 = mcu_to_fpga_read_kgr_reg(132);
	mcu_to_fpga_write_kgr_reg(132,u32Regs1|(1<<8));
	u32Regs2 = mcu_to_fpga_read_kgr_reg(132);

	mcu_to_fpga_write_kgr_reg(134,65536);
	mcu_to_fpga_write_kgr_reg(135,0);
	mcu_to_fpga_write_kgr_reg(133,(0<<23)|(24<<16)|(0<<9)|(0<<8)|0);
	mcu_to_fpga_write_kgr_reg(133,(0<<23)|(24<<16)|(0<<9)|(1<<8)|0);
	mcu_to_fpga_write_kgr_reg(133,(0<<23)|(24<<16)|(0<<9)|(0<<8)|0);
	dx_kprintf("%d %d %d\r\n", 24,(s32)mcu_to_fpga_read_kgr_reg(134),(s32)mcu_to_fpga_read_kgr_reg(135));
	mcu_to_fpga_write_kgr_reg(132,u32Regs1);
}





