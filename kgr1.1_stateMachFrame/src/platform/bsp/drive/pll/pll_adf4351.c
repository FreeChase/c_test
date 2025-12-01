/*
 ******************************************************************************
 * @file    pll_adf4351.c
 * 
 * @brief   ADF4351 driver file
 ******************************************************************************
 * 
 * Copyright (c) 2024, ZS Development Team
 *  
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-20     Andy     the first version
 *
 */
#include <stdio.h>
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"

#include "pll_adf4351.h"

#define ADF4351_SPI_SCLK 		        (17)
#define ADF4351_SPI_DATA		        (18)
#define ADF4351_SPI_LE		            (16)

#define Y                               (10.000000)     //晶振频率

void f4351_dx_dly_us(unsigned int tmptime)
{
	for(int i = 0; i < tmptime*1000 ; i++);
}

/* Private variables ---------------------------------------------------------*/

//模拟SPI总线前，将硬件SPI配置为GPIO
static void adf4351_begin(void)
{
	PERI_DX_PIN_OUTPUT_MODE(ADF4351_SPI_LE);
	PERI_DX_PIN_OUTPUT_MODE(ADF4351_SPI_SCLK);
	PERI_DX_PIN_OUTPUT_MODE(ADF4351_SPI_DATA);
    //
	PERI_DX_PIN_OUPUT_HIGH(ADF4351_SPI_LE);
	PERI_DX_PIN_OUPUT_LOW(ADF4351_SPI_SCLK);
}

/*****************************************************************************************************
*	函 数 名: static void adf4351_end(void)
*	功能说明: 模拟SPI处理完后，将配置为硬件SPI模式
*	形	  参:
*             无
*	返 回 值: 无
******************************************************************************************************/
static void adf4351_end(void)
{
	PERI_DX_PIN_OUPUT_HIGH(ADF4351_SPI_LE);
	PERI_DX_PIN_OUPUT_HIGH(ADF4351_SPI_SCLK);
	PERI_DX_PIN_OUPUT_HIGH(ADF4351_SPI_DATA);
}

/*****************************************************************************************************
*	函 数 名: static void adf4351_write(u32 val)
*	功能说明: adf4351写寄存器
*	形	  参: 
*             u32 val: 写数据
*	返 回 值: 无
******************************************************************************************************/
static void adf4351_write(u32 val)
{
    uint32_t wcnt;
    adf4351_begin();
    u32 dly_time = 1;
    PERI_DX_PIN_OUPUT_LOW(ADF4351_SPI_DATA);

    f4351_dx_dly_us(dly_time);
    PERI_DX_PIN_OUPUT_LOW(ADF4351_SPI_LE);
    f4351_dx_dly_us(dly_time);

    for (wcnt = 0; wcnt < 32; wcnt++)
    {
    	PERI_DX_PIN_OUPUT_LOW(ADF4351_SPI_SCLK);

        if (val & 0x80000000)
        {
        	PERI_DX_PIN_OUPUT_HIGH(ADF4351_SPI_DATA);
        }
        else
        {
        	PERI_DX_PIN_OUPUT_LOW(ADF4351_SPI_DATA);
        }
        val <<= 1;

        f4351_dx_dly_us(dly_time);
        PERI_DX_PIN_OUPUT_HIGH(ADF4351_SPI_SCLK);
    }
    //
    PERI_DX_PIN_OUPUT_LOW(ADF4351_SPI_SCLK);
    f4351_dx_dly_us(10);
    PERI_DX_PIN_OUPUT_HIGH(ADF4351_SPI_LE);
    f4351_dx_dly_us(10);
    PERI_DX_PIN_OUPUT_LOW(ADF4351_SPI_LE);
    adf4351_end();
}

/*****************************************************************************************************
*	函 数 名: void adf4351_set_freq(float Fre)
*	功能说明: adf4351设置频率(35 MHz to 4400 MHz)
*	形	  参: 
*               freq: 设置的频率(单位MHz -> (xx.x) M Hz)
*	返 回 值: 成功返回:DX_EOK	失败返回:-DX_EIO
*   Reference frequency: 25MHz;Output frequency: 200MHz;VCO frequency: 3200MHz;Prescaler: 8/9;
    RF divider: 16;VCO channel spacing frequency: 200KHz;PFD frequency: 10MHz;
    INT: 320;FRAC: 0;MOD: 100;R: 1;Lock Clk Div: 6; bank clk div: 200; Phase: 1
    RFout = [INT + (FRAC/MOD)] * (Fpfd/RF Divider)
    Fpfd = REFIN * [(1 + D)/(R * (1 + T))] //default:25M
    Fvco = RF divider * Output frequency   2.2G-4.4G
******************************************************************************************************/
void adf4351_set_freq(double freq)		//	fre
{
    u8 RF_Div = 0;      //DB20 DB21 DB22 DB23   （VCO细分器）
    u16 INT = 0;        //DB30-DB15             （分频因子证书部分）
    u16 FRAC = 0;       //DB14-DB3              （分频因子小数部分分子）
    u16 MOD = 0;        //DB14-DB3              （分频因子小数部分分母）
    adf4351_r0_t r0Val = { 0 };
    adf4351_r1_t r1Val = { 0 };
    adf4351_r2_t r2Val = { 0 };
    adf4351_r3_t r3Val = { 0 };
    adf4351_r4_t r4Val = { 0 };
    adf4351_r5_t r5Val = { 0 };

    u8 RF_Div_sum = 1;
    u32 VCO_Freq = freq;

    double N = 0, num = 0;

    while ((VCO_Freq < 2200) || (VCO_Freq > 4400))   //计算VCO细分系数（VCO工作在2.2GHz至4.4GHz）
    {
        RF_Div++;
        if (RF_Div > 6) return;
        VCO_Freq *= 2;
        RF_Div_sum *= 2;
    }

    N = freq / (Y / RF_Div_sum);    //计算分频系数
    INT = N;        //得到分频系数整数部分
    N = N - INT;    //得到分频系数小数部分

    for (MOD = 2; MOD < 4096; MOD++)   //计算小数分频的分子与分母
    {
        for (FRAC = 0; FRAC < MOD; FRAC++)
        {
            num = FRAC * 1.0 / MOD;
            if ((num - N) > -0.0001 && (num - N) < 0.0000000001) break;
        }
        if ((num - N) > -0.0001 && (num - N) < 0.0000000001) break;
    }
    if (MOD == 4096) return;

    r5Val.Control = ADF4351_R5_ADDR_BASE;
    r5Val.Lock_Detect = 1;
    adf4351_write(r5Val.r5_data);

    r4Val.Control = ADF4351_R4_ADDR_BASE;
    r4Val.Feedback = 1;
    r4Val.RF_Div = RF_Div;
    r4Val.B_Clock_Div_Val = 80;
    r4Val.VCO = 0;
    r4Val.MTLD = 0;
    r4Val.AuxOutput = 0;
    r4Val.AuxOutput_En = 1;
    r4Val.AuxOutpu_Power = 3;
    r4Val.RF_Output_En = 1;
    r4Val.Output_Power = 3;
    adf4351_write(r4Val.r4_data);

    r3Val.Control = ADF4351_R3_ADDR_BASE;
    r3Val.Band_Select_Clock = 0;
    r3Val.ABP = 0;
    r3Val.Cancelation = 0; //syd 由原来0修改为1
    r3Val.CSR = 0;
    r3Val.Clock_Div_Mod = 0;
    r3Val.Clock_Div_Val = 150;
    adf4351_write(r3Val.r3_data);

    r2Val.Control = ADF4351_R2_ADDR_BASE;
    r2Val.L_Noise_Spur = 0;
    r2Val.MuxOut = 0;
    r2Val.Ref_Doubler = 1;//改成1 syd
    r2Val.Rdiv2 = 0;
    r2Val.R_Counter = 1;
    r2Val.D_Buffer = 0;
    r2Val.C_Pump = 7;
    r2Val.LDF = 0;
    r2Val.LDP = 0;
    r2Val.Phase_Detector = 1;
    r2Val.PD = 0;
    r2Val.Three_State = 0;
    r2Val.Counter_Reset = 0;
    adf4351_write(r2Val.r2_data);

    r1Val.Control = ADF4351_R1_ADDR_BASE;
    r1Val.Modulus = MOD;
    r1Val.Phase = 0;
    r1Val.Prescaler = 1;
    r1Val.Phase_Adjust = 0;
    adf4351_write(r1Val.r1_data);

    r0Val.Control = ADF4351_R0_ADDR_BASE;
    r0Val.Fractional = FRAC/2;//除2 syd
    r0Val.Integer = INT/2;//除2 syd
    adf4351_write(r0Val.r0_data);
}

void Frequency_66MHz(void)
{
    //write communication register 0x00580005 to control the progress 
    //to write Register 5 to set digital lock detector
    adf4351_write(0x580005);
    f4351_dx_dly_us(1000);
    //(DB23=1)The signal is taken from the VCO directly;(DB22-20:4H)the RF divider is 16;(DB19-12:50H)R is 80
    //(DB11=0)VCO powerd up;
    //(DB5=1)RF output is enabled;(DB4-3=3H)Output power level is 5
    adf4351_write(0xE5003C);
    f4351_dx_dly_us(1000);
    //(DB14-3:96H)clock divider value is 150.
    adf4351_write(0x000004B3);
    f4351_dx_dly_us(1000);
    //(DB6=1)set PD polarity is positive;(DB7=1)LDP is 6nS;
    //(DB8=0)enable fractional-N digital lock detect;
    //(DB12-9:7H)set Icp 2.50 mA;
    adf4351_write(0x00004E42);		//(DB23-14:1H)R counter is 1
    f4351_dx_dly_us(1000);
    //(DB14-3:6H)MOD counter is 6;
    //(DB26-15:6H)PHASE word is 1,neither the phase resync 
    adf4351_write(0x08008029);	   //nor the spurious optimization functions are being used
    //(DB27=1)prescaler value is 8/9
    f4351_dx_dly_us(1000);
    //(DB14-3:0H)FRAC value is 0;
    adf4351_write(0x00D30010);		//(DB30-15:140H)INT value is 320;
    f4351_dx_dly_us(1000);
}

/*****************************************************************************************************
*	函 数 名: static err_t adf4351_init(dx_device_t dev)
*	功能说明: adf4351初始化
*	形	  参: 无
*	返 回 值: 成功返回:DX_EOK	失败返回:-DX_EIO
******************************************************************************************************/
static err_t adf4351_init(void)
{
    adf4351_r0_t r0Val = { 0 };
    adf4351_r1_t r1Val = { 0 };
    adf4351_r2_t r2Val = { 0 };
    adf4351_r3_t r3Val = { 0 };
    adf4351_r4_t r4Val = { 0 };
    adf4351_r5_t r5Val = { 0 };

    /*to write Register 5 to set digital lock detector*/
    r5Val.Control = ADF4351_R5_ADDR_BASE;
    r5Val.Lock_Detect = 1;
    adf4351_write(r5Val.r5_data);
    /*(DB23=1)The signal is taken from the VCO directly;
    (DB22-20:4H)the RF divider is 16;
    (DB19-12:50H)R is 80
    (DB11=0)VCO powerd up;
    (DB5=1)RF output is enabled;(DB4-3=3H)Output power level is 5*/
    r4Val.Control = ADF4351_R4_ADDR_BASE;
    r4Val.Feedback = 1;
    r4Val.RF_Div = 4;
    r4Val.B_Clock_Div_Val = 80;
    r4Val.VCO = 0;
    r4Val.MTLD = 0;
    r4Val.AuxOutput = 0;
    r4Val.AuxOutput_En = 1;
    r4Val.AuxOutpu_Power = 3;
    r4Val.RF_Output_En = 1;
    r4Val.Output_Power = 3;
    adf4351_write(r4Val.r4_data);
    /*(DB14-3:96H)clock divider value is 150.*/
    r3Val.Control = ADF4351_R3_ADDR_BASE;
    r3Val.Band_Select_Clock = 0;
    r3Val.ABP = 0;
    r3Val.Cancelation = 0;
    r3Val.CSR = 0;
    r3Val.Clock_Div_Mod = 0;
    r3Val.Clock_Div_Val = 150;
    adf4351_write(r3Val.r3_data);
    /*(DB6=1)set PD polarity is positive;
    (DB7=1)LDP is 6nS;
    (DB8=0)enable fractional-N digital lock detect;
    (DB12-9:7H)set Icp 2.50 mA;
    (DB23-14:1H)R counter is 1*/
    r2Val.Control = ADF4351_R2_ADDR_BASE;
    r2Val.L_Noise_Spur = 0;
    r2Val.MuxOut = 0;
    r2Val.Ref_Doubler = 0;//改成1
    r2Val.Rdiv2 = 0;
    r2Val.R_Counter = 1;
    r2Val.D_Buffer = 0;
    r2Val.C_Pump = 7;
    r2Val.LDF = 0;
    r2Val.LDP = 1;
    r2Val.Phase_Detector = 1;
    r2Val.PD = 0;
    r2Val.Three_State = 0;
    r2Val.Counter_Reset = 0;       
    adf4351_write(r2Val.r2_data);
    /*(DB14-3:6H)MOD counter is 6;
    (DB26-15:6H)PHASE word is 1,neither the phase resync 
     nor the spurious optimization functions are being used
    (DB27=1)prescaler value is 8/9*/
    r1Val.Control = ADF4351_R1_ADDR_BASE;
    r1Val.Modulus = 5;
    r1Val.Phase = 1;
    r1Val.Prescaler = 1;
    r1Val.Phase_Adjust = 0;
    adf4351_write(r1Val.r1_data);
    /*(DB14-3:0H)FRAC value is 0;
    (DB30-15:140H)INT value is 320;*/
    r0Val.Control = ADF4351_R0_ADDR_BASE;
    r0Val.Fractional = 0;
    r0Val.Integer = 320;
    adf4351_write(r0Val.r0_data);
    f4351_dx_dly_us(1000);
    /*默认输出62MHz*/
    adf4351_set_freq(124);
    f4351_dx_dly_us(1000);
    return DX_EOK;
}

/*****************************************************************************************************
*	函 数 名: void dx_hw_adf4351_init(void)
*	功能说明: adf4351模块初始化
*	形	  参: 无
*	返 回 值: 无
******************************************************************************************************/
void dx_hw_adf4351_init(void)
{
    adf4351_init();
}




