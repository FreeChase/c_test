#include "zs_printf.h"
#include "shell.h"

#include "antijam_fpga.h"



#include <JamNum_de.h>
#include <com_protcl_mdl.h>

#include "dev_manager.h"
#include "KgrInfo.h"
#include "sys_ctrl.h"
#include "sleep.h"

extern unsigned char g_ublox_bd21_uartTransmit;
#if 1



/*********************************************
* 函数名 boardTempGet
* 函数功能：通过fpga的XADC接口读取板上温度
*			传感器温度信息
*参数：     
*		* tmpvar 获取温度信息的指针变量
*
*返回值：
*		0：获取到的温度信息正常
*		1：获取到的温度信息异常
*********************************************/
int	boardTempGet(float * tmpvar)
{
	*tmpvar = 0;

	return 0;
}


extern volatile uint32_t gTimerTick;
int TimerCntGet(void)
{
    dx_kprintf("gTimerTick %d \r\n", gTimerTick);
		return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), TimerCntGet, TimerCntGet, TimerCntGet(void));

u8 uartThroughSelect(u8 val);
int uartselect(u8 val)
{
	dx_kprintf("uartMap %d \r\n", val);
    uartThroughSelect(val);
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), uartselect, uartselect, uartselect(val));

void uartDelayRemap(u8 val,u32 delay_ms);

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), uartDelayRemap, uartDelayRemap, uartDelayRemap(val ms));

#if 0
int func(int i, char ch, char *str)
{
    dx_kprintf("input int: %d, char: %c, string: %s\r\n", i, ch, str);
		return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), func, func, test);

void AttSet(int icNo, u8 val)//设置通道衰减，有两级可配置，但是不能进行独立通道衰减配置
{
    if(icNo == 0)
    {
        pe43711_ic0_write(val);
        dx_kprintf("PE43711 ic0 set %d\r\n", val);
    }
    else if(icNo == 1){
        pe43711_ic1_write(val);
        dx_kprintf("PE43711 ic1 set %d\r\n", val);
    }
    else
    {
        dx_kprintf("No have the attenuator!\r\n");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), AttSet, AttSet, Attenuator CMD);

void AttDataSet(u8 val)
{

	Att_data=(int)val;
    dx_kprintf("attData:%d!\r\n",Att_data);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), AttDataSet, AttDataSet, AttData Write CMD);
extern int setpoint;
void PointSet(int val)//ad峰峰值检测门限
{
	setpoint=(int)val;
    dx_kprintf("PointSet:%d\r\n",setpoint);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), PointSet, PointSet, Point Set CMD);
void PointRead()//获取ad峰峰值检测门限
{
    dx_kprintf("PointRead:%d\r\n",setpoint);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), PointRead, PointRead, Point Raad CMD);

void RFSet(int icNo, u32 addr, u32 val)//设置射频芯片输入增益，通过配置给出寄存器地址及赋值
{
    if(icNo == 0)
    {
        xnd2260_ic0_write_reg(addr,  val);
        dx_kprintf("XND2260 ic0 Addr:0x%x, set Value:0x%x\r\n", addr, val);
    }
    else if(icNo == 1){
		xnd2260_ic1_write_reg(addr,  val);
		dx_kprintf("XND2260 ic1 Addr:0x%x, set Value:0x%x\r\n", addr, val);
    }
    else
    {
        dx_kprintf("No have the RF!\r\n");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), RFSet, RFSet, RF Write CMD);

void RFGet(int icNo, u32 addr)//获取
{
    if(icNo == 0)
    {
        dx_kprintf("XND2260 ic0 Addr:0x%x, get Value:0x%x\r\n", addr, xnd2260_ic0_read_reg(addr));
    }
    else if(icNo == 1){
    	dx_kprintf("XND2260 ic1 Addr:0x%x, get Value:0x%x\r\n", addr, xnd2260_ic1_read_reg(addr));
    }
    else
    {
        dx_kprintf("No have the RF!\r\n");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), RFGet, RFGet, RF Read CMD);
void VitisSet(u8 val)//设置整个信号处理链路是否被旁路，由fpga实现
{
	if(val==1)
		SystemSwitchRFWorkMode(CTRL_LOW);
	if(val==0)
		SystemSwitchRFWorkMode(CTRL_HIGH);
	dx_kprintf("value:0x%x\r\n",val);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), VitisSet, VitisSet, Vitis Write CMD);
void KGRAnteSwitch(u8 val)
{
	if(val==5)
	{
		KGR_5=1;
		
		/*除去参考阵元，所需阵元数量，在这个基础上减1；*/
		/*延迟拍数，即抽头数为7+1 8个，需要配置为7*/
		/*快拍数，即相关计算所需时间为24800*0.1 = 2480*/
		FpgaWrite(128,(2400<<16)|(7<<8)|3);
		/*矩阵维度 阵元数*延迟拍数 = 4*8 -1 */
		/*子带个数 2 - 1 第16位开始*/
		/*子矩阵个数：sum(1:4)*2 - 1*/
		FpgaWrite(129,(19<<24)|(0x10000)|31);

		FpgaWrite(131,0x70000);
		FpgaWrite(7,0x01237);
	}
	if(val==4)
	{
		KGR_5=0;
		
		/*除去参考阵元，所需阵元数量，在这个基础上减1；*/
		/*延迟拍数，即抽头数为7+1 8个，需要配置为7*/
		/*快拍数，即相关计算所需时间为24800*0.1 = 2480*/
		FpgaWrite(128,(2400<<16)|(7<<8)|2); 
		
		/*矩阵维度 阵元数*延迟拍数 = 3*8 -1 */
		/*子带个数 2 - 1 第16位开始*/
		/*子矩阵个数：sum(1:3)*2 - 1*/
		FpgaWrite(129,(11<<24)|(0x10000)|23);
		

		FpgaWrite(7,0x0123);
		FpgaWrite(131,0x40000);
	}
	Weight_load_init();
	subBandNum_set(1);
	dx_kprintf("KGR module Current is: [%d] Annte Ele\r\n",KGR_5+4);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),AnteSwitch,KGRAnteSwitch, KGR Write CMD);
//void dlySet(u8 val)
//{
//	if(val==1){
//		dx_hw_sl1645_delay_inc(1);
//		dly_data++;
//	}
//	if(val==0){
//		dx_hw_sl1645_delay_inc(0);
//		dly_data--;
//	}
//	dx_kprintf("value:0x%x\r\n",dly_data);
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), dlySet, dlySet, dly Write CMD);

void testmodeSet(u8 val)//配置adc测试模式，用于测试adc是否正常工作，on模式下会输出特定格式数据至adc接口，off模式下正常输出dac采样结果
{
	if(val==1){
		dx_hw_sl1645_test_mode_on();
	}
	if(val==0){
		dx_hw_sl1645_test_mode_off();
	}
	dx_kprintf("value:0x%x\r\n",val);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), testmodeSet, testmodeSet, testmode Write CMD);
void ChSet(u8 val)//设置参考通路选择方式，当为1时自动选择幅度最大的通道为参考通道，当为2时自动选择幅度最小的作为参考通道
{
	Ch_set=val;
	dx_kprintf("value:0x%x\r\n",val);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ChSet, ChSet, Ch Write CMD);
int GR_set=0;
void GRSet(u8 val)//计算干扰数量和干扰来向
{
	GR_set=val;
	dx_kprintf("value:0x%x\r\n",val);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), GRSet, GRSet, GRSet Write CMD);

int DBS_set=0;
void DBSset(u8 val)//多波束开关：设置成0，空间自适应滤波；设置成1，波束指向可调。
{
	DBS_set=val;
	if(val==0)
	{
		FpgaWrite(92,0);
		FpgaWrite(93,0);
	}
	dx_kprintf("value:0x%x\r\n",val);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), DBSset, DBSset, DBSset Write CMD);

void PSagSet(int val,int val1)//设置波束指向,第一个为azime,第二个为elev
{
	DBS_set=val;
	if((val>=0)&&(val<=360)&&(val1>=0)&&(val1<=90))
	{
		PS_agl = val * PI / 180;
		PS_agh = val1 * PI / 180;
		dx_kprintf("value:%f  %f\r\n",PS_agl,PS_agh);
	}
	else
		dx_kprintf("ag_error:%d,%d\r\n",val,val1);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), PSagSet, PSagSet, PSagSet Write CMD);

void DACSet(u32 addr, u32 val)//配置dac寄存器接口，9653
{
	ad9747_write_reg(addr,  val);
    dx_kprintf("DAC Addr:0x%x, set Value:0x%x\r\n", addr, val);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), DACSet, DACSet, DAC Write CMD);
int BC_buff[4]={0,0,0,0};
void BCSet(u32 addr, u32 val)
{
	BC_buff[addr]=val;
    dx_kprintf("BC Value:%d %d %d %d\r\n", BC_buff[0], BC_buff[1],BC_buff[2],BC_buff[3]);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), BCSet, BCSet, BC Write CMD);
void PIDSet(u32 val)//设置pid控制开关，0是为手动模式，1是pid自动调节模式，这个函数与AttDataSet结合使用
{
	if(val==0){
		Att_set=0;
	}
	else{
		Att_set=1;
	}
	dx_kprintf("value:0x%x\r\n",val);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), PIDSet, PIDSet, PID Write CMD);
void DACRead(u32 addr)//读dac寄存器
{
    dx_kprintf("DAC Addr:0x%x, read Value:0x%x\r\n", addr, ad9747_read_reg(addr));

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), DACRead, DACRead, DAC Read CMD);
int read_config=0;
void SAMSet(u8 step_sel,u32 sam_length)//采样设置，选择fpga内部处理链路采集节点，目的是用于查看各节点数据，采样点数
{
	u8 sam_chum=0;
	if(step_sel==0){
		sam_chum=2;
		read_config=sam_length/2;
	}
	else if((step_sel==1)||(step_sel==2)){
		if(KGR_5 == 1)
		{
			sam_chum=7;	
		}
		else
		{
			sam_chum=3;		
		}
		read_config=sam_length;
	}
	else if((step_sel==3)||(step_sel==4)||(step_sel==5)||(step_sel==6)){
		sam_chum=1;
		read_config=sam_length/4;
	}
	else if((step_sel==7)||(step_sel==9)){
		sam_chum=0;
		read_config=sam_length/8;
	}
	mcu_to_fpga_write_kgr_reg(101, sam_length);
	mcu_to_fpga_write_kgr_reg(100, (0<<16)|(sam_chum<<12)|(0<<8)|(step_sel<<4)|1);
    dx_kprintf("SAM Addr:100, read Value:0x%x,sam_length is %d\r\n",  mcu_to_fpga_read_kgr_reg(100),sam_length);
    mcu_to_fpga_write_kgr_reg(100, (0<<16)|(sam_chum<<12)|(0<<8)|(step_sel<<4)|0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), SAMSet, SAMSet, SAM Write CMD);
void SAM_data_read()//fpga数据读取
{
	u32 u32SamCtrlReg = 0;
	u32 u32SamData = 0;
	float temptmp = 0;
	u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);

	if( (u32SamCtrlReg &((u32)(1)<<21)) == 0)
	{
		dx_kprintf("SAM_data_read Start!! 0x%x del:%d\r\n", u32SamCtrlReg,(mcu_to_fpga_read_kgr_reg(100)>>4)%16);
		if(((mcu_to_fpga_read_kgr_reg(100)>>4)%16==0))
		{
			for(int i=0;i<read_config*4;i+=2){
				u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i);
				dx_kprintf("%d %d ", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
				u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+1);
				dx_kprintf("%d %d\r\n", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
			}
		}
		else if(((mcu_to_fpga_read_kgr_reg(100)>>4)%16==1)||((mcu_to_fpga_read_kgr_reg(100)>>4)%16==2))
		{
			if(KGR_5 == 1)
			{
				for(int i=0;i<read_config*4;i+=8)
				{
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i);
					dx_kprintf("%d %d ", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+1);
					dx_kprintf("%d %d ", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+2);
					dx_kprintf("%d %d ", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+3);
					dx_kprintf("%d %d ", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+4);
					dx_kprintf("%d %d ", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+5);
					dx_kprintf("%d %d ", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+6);
					dx_kprintf("%d %d ", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+7);
					dx_kprintf("%d %d\r\n", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));

				}
			}
			else
			{
				for(int i=0; i<read_config*4; i+=4)
				{
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i);
					dx_kprintf("%d %d ", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+1);
					dx_kprintf("%d %d ", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+2);
					dx_kprintf("%d %d ", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
					u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+3);
					dx_kprintf("%d %d \r\n", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));

				}			
			}
		}
		else if(((mcu_to_fpga_read_kgr_reg(100)>>4)%16==3)||((mcu_to_fpga_read_kgr_reg(100)>>4)%16==4)||((mcu_to_fpga_read_kgr_reg(100)>>4)%16==5)||((mcu_to_fpga_read_kgr_reg(100)>>4)%16==6))
		{
			for(int i=0;i<read_config*4;i+=1){
				u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i);
				dx_kprintf("%d %d\r\n", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));
			}
		}
		else if(((mcu_to_fpga_read_kgr_reg(100)>>4)%16==7)||((mcu_to_fpga_read_kgr_reg(100)>>4)%16==9))
		{
			for(int i=0;i<read_config*4;i+=1){
				u32SamData = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i);
				dx_kprintf("%d\r\n%d\r\n", (s16)(u32SamData&0xffff) ,(s16)(u32SamData>>16));

			}
		}
		dx_kprintf("SAM_data_read Done!!\r\n");
	}
	else
	{
		dx_kprintf("SAM_data_read Error!! 0x%x\r\n", u32SamCtrlReg);
	}

	if(boardTempGet(&temptmp) == 0)
	{
		dx_kprintf("Board Current Temperaure: %.2f\r\n",temptmp);
	}
	else
	{
		dx_kprintf("Get Board Temperaure ERROR\r\n");		
	}	

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), SAM_data_read, SAM_data_read, SAM_data Read CMD);

u8 PowerShowSet_flag=0;
void PowerShowSet(u8 val )//设置调试信息是否显示：1是显示
{
	PowerShowSet_flag=val;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), PowerShowSet, PowerShowSet, PowerShow Write CMD);
void YZSet(int val )
{
	YZData=val;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), YZSet, YZSet, YZSet Write CMD);
extern int CSSet;
void CSShowSet(int val )
{
	CSSet=val;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CSShowSet, CSShowSet, CSShowSet Write CMD);
u8 DCShowSet_flag=0;
void DCShowSet(u8 val ) //去直流开关
{
	DCShowSet_flag=val;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), DCShowSet, DCShowSet, DCShowSet Write CMD);
void weight_set_hand(int addr,u32 val)//fpga内部权重配置
{
	hand_weight[addr] =val;
	for(int i=0;i<48;i++)
	{
		dx_kprintf("Weight Addr:%d, Value:0x%x\r\n",i,hand_weight[i]);
	}
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), weight_set_hand, weight_set_hand, weight_set_hand Write CMD);
void RfXnd2260Ic1LowPowerSet(u8 val)//设置低功耗模式，把第二片射频芯片关掉。2260
{
	if(val==1)
	{
		dx_hw_xnd2260_ic1_lowpower_mode_on();
	}
	else
	{
		dx_hw_xnd2260_ic1_lowpower_mode_off();
	}
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), RfXnd2260Ic1LowPowerSet, RfXnd2260Ic1LowPowerSet, RfXnd2260Ic1LowPowerSet CMD);

void AdcSl1645LowPowerSet(u8 val)//关闭一片AD
{
	if(val==1)
	{
		dx_hw_sl1645_ic1_lowpower_mode_on();
	}
	else
	{
		dx_hw_sl1645_ic1_lowpower_mode_off();
	}
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), AdcSl1645LowPowerSet, AdcSl1645LowPowerSet, AdcSl1645LowPowerSet CMD);

void weight_test(int fre,int addr,int val,int num)//配置下变频本振的频率，这是不同滤波器
{
	ddc_config(fre);//本振工作频率
	switch(val)
	{
	case 0:dx_kprintf("Set B3I_dec_weight\r\n");    weight_write(addr,gFilter_coef_dec[0],num)   ;break;
	case 1:dx_kprintf("Set B3I_lpf_weight \r\n");   weight_write(addr,gFilter_coef_dec[0],num)   ;break;
	case 2:dx_kprintf("Set B3I_dachit_weight \r\n");weight_write(addr,B3I_weight_dacfit,num);break;
	case 3:dx_kprintf("Set B1_dec_weight \r\n");    weight_write(addr,gFilter_coef_dec[1],num)    ;break;
	case 4:dx_kprintf("Set B1_lpf_weight \r\n");    weight_write(addr,gFilter_coef_dec[1],num)    ;break;
	case 5:dx_kprintf("Set B1_dec_weight \r\n");    weight_write(addr,B1_weight_dacfit,num) ;break;
	case 6:dx_kprintf("Set S_dec_weight \r\n");     weight_write(addr,gFilter_coef_dec[0],num)     ;break;
	case 7:dx_kprintf("Set S_lpf_weight \r\n");     weight_write(addr,gFilter_coef_dec[0],num)     ;break;
	case 8:dx_kprintf("Set S_dachit_weight \r\n");  weight_write(addr,S_weight_dacfit,num)  ;break;
	case 9:dx_kprintf("Set hand_weight   \r\n");    weight_write(addr,hand_weight,num)      ;break;
	}
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), weight_test, weight_test, weight_test Write CMD);


void weight_reset()
{
	weight_fix(48);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), weight_reset, weight_reset, weight_reset Write CMD);
void corr_read( u32 u32Sample_length )//调试用，把相关数据读出来
{
	u32 u32SamCtrlReg ,u32SamCtrlReg1;
	u32 u32SamData[4];
	FpgaWrite(101,u32Sample_length);
	FpgaWrite(130,(1<<24)|(1<<16)|(0<<8)|1);
	FpgaWrite(130,(1<<24)|(1<<16)|(0<<8)|0);
	mcu_to_fpga_write_kgr_reg(100, (0<<16)|(3<<12)|(0<<8)|(8<<4)|2);
	mcu_to_fpga_write_kgr_reg(100, (0<<16)|(3<<12)|(0<<8)|(8<<4)|1);
    mcu_to_fpga_write_kgr_reg(100, (0<<16)|(3<<12)|(0<<8)|(8<<4)|0);
	FpgaWrite(130,(1<<24)|(1<<16)|(0<<8)|0);
	FpgaWrite(130,(1<<24)|(1<<16)|(1<<8)|0);
	FpgaWrite(130,(1<<24)|(1<<16)|(0<<8)|0);

	u32SamCtrlReg1 = mcu_to_fpga_read_kgr_reg(138);
    u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    dx_kprintf("CORR_data_read wait!! 0x%x 0x%x\r\n", u32SamCtrlReg,u32SamCtrlReg1);
	u32SamCtrlReg1 = mcu_to_fpga_read_kgr_reg(138);
    u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    dx_kprintf("CORR_data_read wait!! 0x%x 0x%x\r\n", u32SamCtrlReg,u32SamCtrlReg1);
	u32SamCtrlReg1 = mcu_to_fpga_read_kgr_reg(138);
    u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    dx_kprintf("CORR_data_read wait!! 0x%x 0x%x\r\n", u32SamCtrlReg,u32SamCtrlReg1);
	u32SamCtrlReg1 = mcu_to_fpga_read_kgr_reg(138);
    u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    dx_kprintf("CORR_data_read wait!! 0x%x 0x%x\r\n", u32SamCtrlReg,u32SamCtrlReg1);
	u32SamCtrlReg1 = mcu_to_fpga_read_kgr_reg(138);
    u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    dx_kprintf("CORR_data_read wait!! 0x%x 0x%x\r\n", u32SamCtrlReg,u32SamCtrlReg1);

	if( (u32SamCtrlReg &((u32)(1)<<20)) == 0)
	{

		for(int i=0;i<=u32Sample_length*4;i=i+4){
			u32SamData[0] = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i);
			u32SamData[1] = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+1);
			u32SamData[2] = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+2);
			u32SamData[3] = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i+3);

			dx_kprintf("%d %d %d %d %d %d %d %d\r\n",
									(s16)(u32SamData[0]&0xffff) ,(s16)(u32SamData[0]>>16),
									(s16)(u32SamData[1]&0xffff) ,(s16)(u32SamData[1]>>16),
									(s16)(u32SamData[2]&0xffff) ,(s16)(u32SamData[2]>>16),
									(s16)(u32SamData[3]&0xffff) ,(s16)(u32SamData[3]>>16));
		}
		dx_kprintf("CORR_data_read Done!!\r\n");
	}
	else
	{
		dx_kprintf("CORR_data_read Error!! 0x%x\r\n", u32SamCtrlReg);
	}
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), corr_read, corr_read, corr_read Write CMD);

void StapMatrixNew_regs_read(  )//
{
	for(int i=0; i<13; i++)
	{
		dx_kprintf("%d %x\r\n",128+i, mcu_to_fpga_read_kgr_reg(128+i));
	}
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), StapMatrixNew_regs_read, StapMatrixNew_regs_read, StapMatrixNew_regs_read Read CMD);


void Weight_read(unsigned int subBandNum)//获取计算得到的权重
{
	u32 u32Regs;
	u32 maxNum = 0;
	if(KGR_5 == 1)
	{
		maxNum = 40;
	}
	else
	{
		maxNum = 32;
	}
	u32Regs = mcu_to_fpga_read_kgr_reg(132);
	//FpgaWrite(132,u32Regs|(1<<8));
	for(int i=0;i<maxNum;i++)
	{
		FpgaWrite(133,(subBandNum<<23)|(i<<16)|(0<<9)|(0<<8)|0);
		FpgaWrite(133,(subBandNum<<23)|(i<<16)|(1<<9)|(0<<8)|0);
		FpgaWrite(133,(subBandNum<<23)|(i<<16)|(0<<9)|(0<<8)|0);

		dx_kprintf("%d %d %d %x\r\n", i,(s32)mcu_to_fpga_read_kgr_reg(136),(s32)mcu_to_fpga_read_kgr_reg(137),u32Regs);
	}
	//FpgaWrite(132,u32Regs);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), Weight_read, Weight_read, Weight_read Read CMD);



void Weight_hand_write(unsigned int subBnum, u32 Addr,s32 data_i, s32 data_q )
{
	u32 u32Regs1,u32Regs2;
	u32Regs1 = mcu_to_fpga_read_kgr_reg(132);
	FpgaWrite(132,u32Regs1|(1<<8));
	u32Regs2 = mcu_to_fpga_read_kgr_reg(132);

	FpgaWrite(134,data_i);
	FpgaWrite(135,data_q);
	FpgaWrite(133,(subBnum<<23)|(Addr<<16)|(0<<9)|(0<<8)|0);
	FpgaWrite(133,(subBnum<<23)|(Addr<<16)|(0<<9)|(1<<8)|0);
	FpgaWrite(133,(subBnum<<23)|(Addr<<16)|(0<<9)|(0<<8)|0);
	dx_kprintf("%d %d %d\r\n", Addr,(s32)mcu_to_fpga_read_kgr_reg(134),(s32)mcu_to_fpga_read_kgr_reg(135));
	FpgaWrite(132,u32Regs1);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), Weight_hand_write, Weight_hand_write, Weight_hand_write Write CMD);






void Weight_write_debug()
{
	u32 u32Regs1,u32Regs2;
	u32Regs1 = mcu_to_fpga_read_kgr_reg(132);
	FpgaWrite(132,u32Regs1|(1<<8));
	u32Regs2 = mcu_to_fpga_read_kgr_reg(132);
	dx_kprintf("Weight_write_debug:%x %x\r\n", u32Regs1 ,u32Regs2);
	for(int i=0;i<40;i++){

		FpgaWrite(134,0);
		FpgaWrite(135,0);

		FpgaWrite(133,(i<<16)|(0<<9)|(0<<8)|0);
		FpgaWrite(133,(i<<16)|(0<<9)|(1<<8)|0);
		FpgaWrite(133,(i<<16)|(0<<9)|(0<<8)|0);

		//dx_kprintf("%d %d\r\n", (s32)mcu_to_fpga_read_kgr_reg(136),(s32)mcu_to_fpga_read_kgr_reg(137));
	}
	FpgaWrite(132,u32Regs1);



	u32Regs1 = mcu_to_fpga_read_kgr_reg(132);
	FpgaWrite(132,u32Regs1|(1<<8));
	u32Regs2 = mcu_to_fpga_read_kgr_reg(132);
	dx_kprintf("Weight_write_debug:%x %x\r\n", u32Regs1 ,u32Regs2);
	for(int i=0;i<40;i++){

		FpgaWrite(134,0);
		FpgaWrite(135,0);

		FpgaWrite(133,((1&0x3)<<23)|(i<<16)|(0<<9)|(0<<8)|0);
		FpgaWrite(133,((1&0x3)<<23)|(i<<16)|(0<<9)|(1<<8)|0);
		FpgaWrite(133,((1&0x3)<<23)|(i<<16)|(0<<9)|(0<<8)|0);

		//dx_kprintf("%d %d\r\n", (s32)mcu_to_fpga_read_kgr_reg(136),(s32)mcu_to_fpga_read_kgr_reg(137));
	}
	FpgaWrite(132,u32Regs1);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), Weight_write_debug, Weight_write_debug, Weight_write_debug Read CMD);



void SAM_data_read_AD_8CH(unsigned int tmpsamlen)//读取adc给到fpga的数据
{
	u32 u32SamCtrlReg;
	u32 u32SamData[4]   ;
	float tmpTemp;
	unsigned int j;

	mcu_to_fpga_write_kgr_reg(100, (0<<16)|(3<<12)|(0<<8)|(0<<4)|1);
	dx_kprintf("SAM Addr:100, Read Value:0x%x\r\n",  mcu_to_fpga_read_kgr_reg(100));
    mcu_to_fpga_write_kgr_reg(100, (0<<16)|(3<<12)|(0<<8)|(0<<4)|0);
    dx_kprintf("ADC DATA WRITE STAR\r\nT");
	u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    while((u32SamCtrlReg &((u32)(1)<<21)))
    {
    	u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    }
    dx_kprintf("ADC DATA WRITE DONE\r\n");

	dx_kprintf("SAM_data_read Start!! 0x%x\r\n", u32SamCtrlReg);
	for(int i=0;i<tmpsamlen;i++){
		for (int j=0; j<4; j++)
		{
			u32SamData[j] = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i*4 + j);
		}
		dx_kprintf("%d %d %d %d %d %d %d %d\r\n",
				(short)(u32SamData[0]&0xffff), (short) ((u32SamData[0]>>16)&0xffff) ,
				(short)(u32SamData[1]&0xffff), (short) ((u32SamData[1]>>16)&0xffff) ,
				(short)(u32SamData[2]&0xffff), (short) ((u32SamData[2]>>16)&0xffff) ,
				(short)(u32SamData[3]&0xffff), (short) ((u32SamData[3]>>16)&0xffff)) ;
#if 0
		if(i% == 0)
		{
			vTaskDelay(50);
		}
#endif
	}
	if(boardTempGet(&tmpTemp) == 0)
	{
		dx_kprintf("BCT: %.2f 0x%x\r\n",tmpTemp,mcu_to_fpga_read_kgr_reg(FPGA_STAP_BYPASSSET_REG));
	}
	else
	{
		dx_kprintf("BCT ERROR\r\n");		
	}	
	dx_kprintf("SAM_data_read Done!!\r\n");
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), SAM_data_read_AD_8CH, SAM_data_read_AD_8CH, SAM_data_read_AD_8CH CMD);





void ADC1_DATAA1_CNTVALUE_ADD()
{
	u32 adc1_data1_cnt_vlaue;
	adc1_data1_cnt_vlaue = ((mcu_to_fpga_read_kgr_reg(ADC_DCO_DELAY_VALUE0)>>20)&0x1f);
	dx_kprintf("current adc1_dataa1 delaycntvalue is %d\r\n", adc1_data1_cnt_vlaue);
	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_CE,0x010);
	dx_dly_us(1);
	mcu_to_fpga_write_kgr_reg(ADC_DCO_DELAY_CE,0x00);
	adc1_data1_cnt_vlaue = ((mcu_to_fpga_read_kgr_reg(ADC_DCO_DELAY_VALUE0)>>20)&0x1f);
	dx_kprintf("next adc1_dataa1 delaycntvalue is %d\r\n", adc1_data1_cnt_vlaue);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ADC1_DATAA1_CNTVALUE_ADD, ADC1_DATAA1_CNTVALUE_ADD, ADC1_DATAA1_CNTVALUE_ADD CMD);

void ModeSwitch(int modeType)//ook协议
{

	switch(modeType)
	{
		case 0:
		{
			//直通模式
			SystemSwitchRFWorkMode(CTRL_LOW);
			dx_kprintf("Switch direct connection!\r\n");
		}
			break;
		case 1:
		{
			//抗干扰模式
			SystemSwitchRFWorkMode(CTRL_HIGH);
			dx_kprintf("Switch anti-interference!\r\n");
		}
			break;
		default:
			dx_kprintf("Switch mode error!\r\n");
			break;
	}
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ModeSwitch, ModeSwitch, Mode Switch CMD);


void testgetboardTemp()
{
	float temptmp = 0;

	if(boardTempGet(&temptmp) == 0)
	{
		dx_kprintf("Board Current Temperaure: %.2f\r\n",temptmp);
	}
	else
	{
		dx_kprintf("Get Board Temperaure ERROR\r\n");		
	}

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), testgetboardTemp, testgetboardTemp, Get BoardTempe CMD);


extern unsigned char gJamDetectFlag ;
void testget_JamDetectFlag()
{


	dx_kprintf("gJamDetectFlag %d!\r\n",gJamDetectFlag);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), get_JamDetectFlag, testget_JamDetectFlag, testget_JamDetectFlag CMD);


/*********************************************
* 函数名 testsetstapalgbypass
* 函数功能：将stap 算法设置为旁路模式
*参数：     
*		setval 0: 表示为bypass
*			1：表示为正常使用算法     
*
*
*/
void testsetstapalgbypass(unsigned int setval)
{
	if(setval == 0)
	{
		mcu_to_fpga_write_kgr_reg(FPGA_STAP_BYPASSSET_REG,0x1050001);
		dx_kprintf("STAP alg Has set bypass \r\n");
	}
	else
	{
		mcu_to_fpga_write_kgr_reg(FPGA_STAP_BYPASSSET_REG,0x1050000);
		dx_kprintf("STAP alg Can Work\r\n");		
	}
	
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), setstapalgbypass, testsetstapalgbypass, set set stap alg byPass CMD);




/*********************************************
* 函数名 testgetstapalgbypass
* 函数功能：将stap 算法设置为旁路模式
*参数：     
*		setval 0: 表示为bypass
*			1：表示为正常使用算法     
*
*
*/
void testgetstapalgbypass()
{
	unsigned int tmpval =0;
	tmpval = mcu_to_fpga_read_kgr_reg(FPGA_STAP_BYPASSSET_REG);

	dx_kprintf("STAP alg ctrl reg Val = 0x%x \r\n",tmpval);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), getstapalgbypass, testgetstapalgbypass, set get stap alg byPass CMD);



/*********************************************
* 函数名 boardTempGet
* 函数功能：通过fpga的XADC接口读取板上温度
*			传感器温度信息
*	温度换算关系是：
*			温度值在32位寄存器的低16，其中高12位有效数据
* 			temvar = reg_valule*503.975/4096-273.15;
*参数：     
*		* tmpvar 获取温度信息的指针变量
*
*返回值：
*		0：获取到的温度信息正常
*		1：获取到的温度信息异常
*********************************************/
int	boardTempGet(float * tmpvar)
{
	float temptmpf32 = 0;
	unsigned int 	temptmpuint32 = 0;
	//读取fpga对应值
	temptmpuint32 = mcu_to_fpga_read_kgr_reg(FPGA_XADC_TEMP_REG);
	temptmpuint32 = (temptmpuint32 >> 4) &(0xFFF);
	//如果读取的温度值超过相应的边界，则认为温度异常
	//则返回异常	
	if(0)
	{
		return -1;
	}

	//将无符号整型转换为浮点数据
	temptmpf32 = (float)temptmpuint32;
	
	//将该值按照温度转换协议，转换成温度
	temptmpf32 = temptmpf32*503.975/4096-273.15;

	//返回温度
	*tmpvar = temptmpf32;
	
	return 0;
	
}


/*********************************************
* 函数名 testrf_xnd2260_regRead
* 函数功能：xnd2260寄存器读接口测试
*参数：     
*		socId: 芯片序号，0,1
*	addrtmp：地址
*
*
*/
void testrf_xnd2260_regRead(unsigned int socId,unsigned int addrtmp)
{
	unsigned int tmpval =0;
	

	if(socId == 0)
	{
		tmpval = xnd2260_ic0_read_reg(addrtmp);
		dx_kprintf("%d #  reg[0x%x]  = 0x%x \r\n",socId+1,addrtmp,tmpval);
		return ;
	}

	if(socId == 1)
	{
		tmpval = xnd2260_ic1_read_reg(addrtmp);
		dx_kprintf("%d #  reg[0x%x]  = 0x%x \r\n",socId+1,addrtmp,tmpval);
		return ;
	}

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), rf_xnd2260_regRead, testrf_xnd2260_regRead, testrf xnd2260 regRead CMD);


/*********************************************
* 函数名 testrf_xnd2260_regWrite
* 函数功能：xnd2260寄存器写接口测试
*参数：     
*		socId: 芯片序号，0,1
*	addrtmp：地址
*
*
*/
void testrf_xnd2260_regWrite(unsigned int socId,unsigned int addrtmp,unsigned int Value)
{
	unsigned int tmpval =0;
	

	if(socId == 0)
	{
		xnd2260_ic0_write_reg(addrtmp,Value);
		vTaskDelay(50);
		tmpval = xnd2260_ic0_read_reg(addrtmp);
		dx_kprintf("%d #  reg[0x%x]  = 0x%x \r\n",socId+1,addrtmp,tmpval);
		
		return ;
	}

	if(socId == 1)
	{
		xnd2260_ic1_write_reg(addrtmp,Value);
		vTaskDelay(50);
		tmpval = xnd2260_ic1_read_reg(addrtmp);
		dx_kprintf("%d #  reg[0x%x]  = 0x%x \r\n",socId+1,addrtmp,tmpval);
		return ;
	}

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), rf_xnd2260_regWrite, testrf_xnd2260_regWrite, testrf xnd2260 regWrite CMD);


/*********************************************
* 函数名 testad_sl1645_regRead
* 函数功能：sl1645寄存器读接口测试
*参数：     
*		socId: 芯片序号，0,1
*	addrtmp：地址
*
*
*/
void testad_sl1645_regRead(unsigned int socId,unsigned int addrtmp)
{
	unsigned int tmpval =0;
	

	if(socId == 0)
	{
		tmpval = sl1645_ic0_read_reg(addrtmp);
		dx_kprintf("%d #  reg[0x%x]  = 0x%x \r\n",socId+1,addrtmp,tmpval);
		return ;
	}

	if(socId == 1)
	{
		tmpval = xnd2260_ic1_read_reg(addrtmp);
		dx_kprintf("%d #  reg[0x%x]  = 0x%x \r\n",socId+1,addrtmp,tmpval);
		return ;
	}

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ad_sl1645_regRead, testad_sl1645_regRead, testad sl1645 regRead CMD);


/*********************************************
* 函数名 testrf_xnd2260_regWrite
* 函数功能：xnd2260寄存器写接口测试
*参数：     
*		socId: 芯片序号，0,1
*	addrtmp：地址
*
*
*/
void testad_sl1645_regWrite(unsigned int socId,unsigned int addrtmp,unsigned int Value)
{
	unsigned int tmpval =0;
	

	if(socId == 0)
	{
		sl1645_ic0_write_reg(addrtmp,Value);
		vTaskDelay(50);
		tmpval = sl1645_ic0_read_reg(addrtmp);
		dx_kprintf("%d #  reg[0x%x]  = 0x%x \r\n",socId+1,addrtmp,tmpval);
		
		return ;
	}

	if(socId == 1)
	{
		sl1645_ic1_write_reg(addrtmp,Value);
		vTaskDelay(50);
		tmpval = sl1645_ic1_read_reg(addrtmp);
		dx_kprintf("%d #  reg[0x%x]  = 0x%x \r\n",socId+1,addrtmp,tmpval);
		return ;
	}

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ad_sl1645_regWrite, testad_sl1645_regWrite, testad sl1645 regWrite CMD);



/*********************************************
* 函数名 test_ver
* 函数功能：版本打印
*参数：     
*		socId: 芯片序号，0,1
*	addrtmp：地址
*
*
*/
void test_ver()
{

	module_version_printf();


	return ;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), kgrver, test_ver, kgr ver CMD);






/*********************************************
* 函数名       testset_weight_refresh_tc
* 函数功能：设置权重更新周期
*参数：     
*		usTime: 更新周期
*
*
*/
void testset_weight_refresh_tc(unsigned int usTime)
{

	set_weight_refresh_tc(usTime);

	return ;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),set_weight_refreshtc, testset_weight_refresh_tc, testset weight refresh_tc ver CMD);


/*********************************************
* 函数名       testget_weight_refresh_tc
* 函数功能：设置权重更新周期
*参数：     
*		usTime: 更新周期
*
*
*/
void testget_weight_refresh_tick()
{
	unsigned int tmpval = 0;

	tmpval = get_weight_refresh_tick();
	dx_kprintf("weight refresh tick reg[131] :  %d\r\n",tmpval);
	return ;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),get_weight_refreshtick, testget_weight_refresh_tick,testget weight refresh tick CMD);


u8 gDebugJSR_flag = 0;
unsigned int gDebugJsr_msTime =0;
float gBeforeSJRvalThreshold = 100000;


/*********************************************
* 函数名       testset_debug_sjr
* 函数功能：设置是否通过串口打印sjr信息
*参数：     
*		usTime: 更新周期
*
*
*/
void testset_debug_sjr(unsigned int flagtmp,unsigned int jamDenflag,unsigned int mstime)
{
	unsigned int tmpval = 0;
	gDebugJsr_msTime = mstime;
	dx_kprintf("set flag %d time %d\r\n",gDebugJSR_flag,gDebugJsr_msTime);

	gDebugJSR_flag = flagtmp;

	gJamDetectEnableFlag = jamDenflag;

	
	
	return ;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),set_debug_sjr, testset_debug_sjr,testset debug sjr CMD);

/*********************************************
* 函数名       testset_difsjr_debug_threshold
* 函数功能：设置是否通过串口打印sjr信息
*参数：     
*		usTime: 更新周期
*
*
*/
void testset_difsjr_debug_threshold(unsigned int tmpthreshold)
{
	float tmpval = 0;
	tmpval = (float)tmpthreshold;
	gBeforeSJRvalThreshold = tmpval;
	
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),set_difsjr_debug_threshold, testset_difsjr_debug_threshold,set_difsjr_debug_threshold CMD);


/*********************************************
* 函数名       testset_Jam_detect_sjrthreshold
* 函数功能：设置是否通过串口打印sjr信息
*参数：     
*		usTime: 更新周期
*
*
*/
void testset_Jam_detect_sjrthreshold(int thresholdtmp)
{
	float tmpval = 0;
	tmpval = (float)thresholdtmp;
	gJamDetectSJRThreshold = tmpval;

	dx_kprintf("set threshold %d time %d\r\n",gJamDetectSJRThreshold);	


	
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),set_Jam_detect_sjrthreshold, testset_Jam_detect_sjrthreshold,set_difsjr_debug_threshold CMD);




/*********************************************
* 函数名       testget_sjr
* 函数功能：获取sjr
*参数：     
*		无
*
*
*/
void testget_sjr()
{
	unsigned int tmpval1 = 0;
	unsigned int tmpval2 = 0;
	float tmpval = 0;
	tmpval = stap_sjr_get(&tmpval1,&tmpval2);
	dx_kprintf("sjr value = [%.3f] dB  bef = %d ----  aft = %d\r\n",tmpval,tmpval1,tmpval2);
	return ;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),get_sjr, testget_sjr,testget sjr CMD);


/*********************************************
* 函数名       testset_sjr_freshtime
* 函数功能：获取sjr
*参数：     
*		输入 ustime 时间us
*
*
*/
void testset_sjr_freshtime(int ustime)
{
	stap_sjr_refresh_time(ustime);
	return ;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),set_sjr_freshtime, testset_sjr_freshtime,testset sjr freshtime CMD);


/*********************************************
* 函数名       testset_sjr_enable
* 函数功能：获取sjr
*参数：     
*		无
*
*
*/
void testset_sjr_enable(int flag)
{
	stap_sjr_enable(flag);
	return ;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),set_sjr_enable, testset_sjr_enable,testset sjr enableCMD);


/*********************************************
* 函数名       testset_sjr_chn
* 函数功能：设置参考通道
*参数：     
*		无
*
*
*/
void  testset_sjr_chn(int chn)
{
	stap_sjr_refchn_set(chn);
	return ;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),set_sjr_chn, testset_sjr_chn,testset sj chn CMD);

/*********************************************
* 函数名       testset_sjr_length
* 函数功能：设置统计长度
*参数：     
*		无
*
*
*/
void  testset_sjr_length(int setLenIndex)
{
	stap_sjr_length_set(setLenIndex);
	return ;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),set_sjr_length, testset_sjr_length,testset sjr length CMD);




/*********************************************
* 函数名       start_phaseadjust()
* 函数功能：设置统计长度
*参数：     
*		无
*
*
*/
void  start_phaseadjust(int adjustCnt)
{
	float phasebuffertmp[5] = {0};

	dx_kprintf("adjustCnt = %d \r\n",adjustCnt);
	
	mult_channel_atuo_phaseadjust(adjustCnt,&phasebuffertmp[0]);

	dx_kprintf("Adjust Phase  ：[AD-7: %.2f]  [AD-0: %.2f] [AD-1: %.2f] [AD-2: %.2f] [AD-3: %.2f] \r\n",
		phasebuffertmp[0],
		phasebuffertmp[1],
		phasebuffertmp[2],
		phasebuffertmp[3],
		phasebuffertmp[4]);
	
	return ;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),start_phaseadjust, start_phaseadjust,start phasea djustCMD);



/*********************************************
* 函数名       testcomplex_calphase()
* 函数功能：计算相位
*参数：     
*		无
*
*
*/
void   testcomplex_calphase(int realInt,int imageInt)
{
	double realf64tmp = 0 ; 
	double imagef64tmp = 0;

	Complex tempCri  = {0,0};
	float retPhasef32 = 0;

	realf64tmp = (double)realInt;
	imagef64tmp = (double)imageInt;

	tempCri.real = realf64tmp;
	tempCri.imag = imagef64tmp;
	
	retPhasef32 = phase_Complex(tempCri);
	
	dx_kprintf("coplex[%.4f   %.4f]  phase = %.2f  \r\n",tempCri.real,tempCri.imag,retPhasef32);
	return 0;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),complex_calphase, testcomplex_calphase,test complex calphase CMD);


/*********************************************
* 函数名       testcomplex_calphase()
* 函数功能：计算相位
*参数：     
*		无
*
*
*/
void   testphasecompensate()
{

	stap_multchn_phasecompensate();
	
	return 0;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),phasecompensate, testphasecompensate,test phase compensate CMD);



/*********************************************
* 函数名       testflashrw()
* 函数功能：flash 读写测试
*参数：     
*		无
*
*
*/
void   testflashrw(unsigned int rwflag,unsigned int addr,unsigned int val)
{
	unsigned char readBuf[4];
	unsigned char writeBuf[4];

	if(rwflag == 0)
	{
		spi_flash_read_n(addr,1,&readBuf[0]);
		dx_kprintf("flash read [addr = 0x%x]  Val = 0x%d \r\n",addr,readBuf[0]);
		
	}
	else
	{
		writeBuf[0] = (unsigned char)val&0xFF;
		
		spi_flash_write_n(&writeBuf[0],addr,1);
	}
	
	return 0;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),testflashrw, testflashrw,test flash rw CMD);



/*********************************************
* 函数名       testsaveparam()
* 函数功能：保存参数
*参数：     
*		无
*
*
*/
void   testsaveparam()
{
	saveAll_paramToflash();
	
	return 0;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),saveparam, testsaveparam,test save param CMD);


/*********************************************
* 函数名       testloadparam()
* 函数功能：从flash中加载参数
*参数：     
*		无
*
*
*/
void   testloadparam()
{
	getAll_paramFromflash();
	
	return 0;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),loadparam, testloadparam,test get param CMD);



/*********************************************
* 函数名       testloadparam()
* 函数功能：从flash中加载参数
*参数：     
*		无
*
*
*/
void   testsubbanddivctl(unsigned int flag)
{
	if(flag == 0)
	{
		dx_kprintf("Enable subBand Div\r\n");
		subBandDiv_enable();
	}
	else
	{
		dx_kprintf("Disable subBand Div\r\n");
		subBandDiv_bypass();		
	}
	
	return 0;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),subbanddivctl, testsubbanddivctl,test subband div ctl CMD);




/*********************************************
* 函数名       testPLfilter_select()
* 函数功能：选择PL端固定滤波器系数
*参数：     
*		无
*
*
*/
void   testPLfilter_select(unsigned int regVal,unsigned int tmpVal ,unsigned int valIndex,unsigned int maxNum,unsigned int setflag)
{
	int testdata[80] = {0};

	if(setflag == 0)
	{
		PLfpga_filter_select(regVal,tmpVal);

		dx_kprintf("PL select filter  reg = %d  filter Num = %d \r\n",regVal,tmpVal);
	}

	if(setflag == 1)
	{
		testdata[valIndex] = tmpVal;
		PLfpga_filter_config(regVal,testdata,maxNum);
		dx_kprintf("PL filter config reg = %d  first coef = %d \r\n",regVal,tmpVal);
	}

	
	return 0;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),testPLfilter_select, testPLfilter_select,tes tPLfilter select CMD);




/*********************************************
* 函数名       testPLfilter_select()
* 函数功能：选择PL端固定滤波器系数
*参数：     
*		无
*
*
*/
void   testSetPidDebugFuncFlag(unsigned int setflag)
{
	gdegPidfuncFlag = setflag;

	dx_kprintf("gdegPidfuncFlag = %d \r\n",gdegPidfuncFlag);
	
	return 0;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),SetPidDebugFuncFlag, testSetPidDebugFuncFlag,test Set Pid Debug FuncFlagCMD);


#endif

/*********************************************
* 函数名 test_ver
* 函数功能：版本打印
*参数：
*		socId: 芯片序号，0,1
*	addrtmp：地址
*
*
*/
void test_verAll()
{

	module_versionAll_printf();


	return ;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),verAll, test_verAll, kgr ver CMD);


void testloadRamData(unsigned int tempTotalPacke)//读取fpga缓冲区中的数据
{
	u32 u32SamCtrlReg;
	u32 u32SamData[4]   ;
	float tmpTemp;
	unsigned int j;


	com_data_ad_send(tempTotalPacke);

	if(boardTempGet(&tmpTemp) == 0)
	{
		dx_kprintf("$JAMTE,%.2f,0x%x,0x0D0A\r\n",tmpTemp,mcu_to_fpga_read_kgr_reg(FPGA_STAP_BYPASSSET_REG));
	}
	else
	{
		dx_kprintf("$JAMTE,error,0x%x,0x0D0A\r\n",mcu_to_fpga_read_kgr_reg(FPGA_STAP_BYPASSSET_REG));
	}

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), loadRamData, testloadRamData, testloadRamData CMD);


void FpgaWrite(u32 addr, u32 val)//
{
	mcu_to_fpga_write_kgr_reg(addr, val);
	//dx_kprintf("FPGA write addr:0x%x,  value:0x%x\r\n",addr, val);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), FpgaWrite, FpgaWrite, Fpga Write CMD);

void FpgaRead(u32 addr)
{
	dx_kprintf("FPGA read addr:0x%x,  value:0x%x\r\n",addr, mcu_to_fpga_read_kgr_reg(addr));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), FpgaRead, FpgaRead, Fpga Read CMD);


void FpgaWriteRaw(u32 addr, u32 val)//
{
	//mcu_to_fpga_write_kgr_reg(addr, val);
	*(volatile u32*)(addr) = val;
	//dx_kprintf("FPGA write addr:0x%x,  value:0x%x\r\n",addr, val);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), FpgaWriteRaw, FpgaWriteRaw, Fpga Write CMD);

void FpgaReadRaw(u32 addr)
{

	dx_kprintf("FPGA read addr:0x%x,  value:0x%x\r\n",addr, *(volatile u32*)(addr));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), FpgaReadRaw, FpgaReadRaw, Fpga Read CMD);



void Weight_read(unsigned int subBandNum)//获取计算得到的权重
{
	u32 u32Regs;
	u32 maxNum = 0;

	maxNum = 32;

	u32Regs = mcu_to_fpga_read_kgr_reg(132);
	//FpgaWrite(132,u32Regs|(1<<8));
	for(int i=0;i<maxNum;i++)
	{
		FpgaWrite(133,(subBandNum<<23)|(i<<16)|(0<<9)|(0<<8)|0);
		FpgaWrite(133,(subBandNum<<23)|(i<<16)|(1<<9)|(0<<8)|0);
		FpgaWrite(133,(subBandNum<<23)|(i<<16)|(0<<9)|(0<<8)|0);

		dx_kprintf("%d %d %d %x\r\n", i,(s32)mcu_to_fpga_read_kgr_reg(136),(s32)mcu_to_fpga_read_kgr_reg(137),u32Regs);
	}
	//FpgaWrite(132,u32Regs);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), Weight_read, Weight_read, Weight_read Read CMD);


void PLramRegisterPrint(unsigned int registerNum)//获取计算得到的权重
{

	for(int i=0;i<registerNum;i++)
	{
		dx_kprintf("Reg[%03d] = 0x%08x\r\n",i,mcu_to_fpga_read_kgr_reg(i));
	}
	//FpgaWrite(132,u32Regs);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), regprint, PLramRegisterPrint, PL ram Register Print Read CMD);


void pergpiotest(u8 setflag,u8 gpioPin,u8 valtemp)//获取计算得到的权重
{
	u8 ret = 0;
	if(setflag == 1)
	{
		writePeriGpioVal(gpioPin,valtemp);
		dx_kprintf("set peri GPIO [%d] = %d\r\n",gpioPin,valtemp);
	}
	ret = readPeriGpioVal(gpioPin);
	dx_kprintf("read peri GPIO [%d] = %d\r\n",gpioPin,ret);
	//FpgaWrite(132,u32Regs);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), pergpiotest, pergpiotest, per gpio test CMD);

void ctrlgpiotest(u8 setflag,u8 gpioPin,u8 valtemp)
{
	u8 ret = 0;
	if(setflag == 1)
	{
		writeCtrlGpioVal(gpioPin,valtemp);
		dx_kprintf("set peri GPIO [%d] = %d\r\n",gpioPin,valtemp);
	}
	ret = readCtrlGpioVal(gpioPin);
	dx_kprintf("read peri GPIO [%d] = %d\r\n",gpioPin,ret);
	//FpgaWrite(132,u32Regs);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ctrlgpiotest, ctrlgpiotest, ctrl gpio test CMD);


void setUbloxComTrans(u8 valtemp)//获取计算得到的权重
{
	u8 ret = 0;
	g_ublox_bd21_uartTransmit = valtemp;

	dx_kprintf("g_ubloxuartTransmit = %d\r\n",g_ublox_bd21_uartTransmit);
	//FpgaWrite(132,u32Regs);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), setUbloxComTrans, setUbloxComTrans, per setUbloxComTrans test CMD);

/*********************************************
* 函数名 testrf_xnd2260_regWrite
* 函数功能：xnd2260寄存器写接口测试
*参数：
*		socId: 芯片序号，0,1
*	addrtmp：地址
*
*
*/
void testad_mt9653_regWrite(unsigned int addrtmp,unsigned int Value)
{
	unsigned int tmpval =0;


	mt9653_ic0_write_reg(addrtmp,Value);
	for(int i = 0 ;i < 100000; i++);
	tmpval = mt9653_ic0_read_reg(addrtmp);
	dx_kprintf("mt9653 Write reg[0x%x]  = 0x%x \r\n",addrtmp,tmpval);


}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), mt9653_regWrite, testad_mt9653_regWrite, testad sl1645 regWrite CMD);


void testad_mt9653_regRead(unsigned int addrtmp)
{
	unsigned int tmpval =0;

	tmpval = mt9653_ic0_read_reg(addrtmp);
	dx_kprintf("mt9653 Read reg[0x%x]  = 0x%x \r\n",addrtmp,tmpval);


}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), mt9653_regRead, testad_mt9653_regRead, testad sl1645 regWrite CMD);


/*********************************************
* 函数名 testrf_xnd2260_regWrite
* 函数功能：xnd2260寄存器写接口测试
*参数：
*		socId: 芯片序号，0,1
*	addrtmp：地址
*
*
*/
void testrf_1922_regWrite(unsigned char u32flag,unsigned int addrtmp,unsigned int Value)
{
	unsigned int tmpval =0;


	if(u32flag == 1)
	{
		rx1922_write_reg(addrtmp,Value);
		for(int i = 0; i < 10000; i++);
		tmpval = rx1922_read_reg(addrtmp);
		dx_kprintf("1922 Write  #  reg[0x%x]  = 0x%x \r\n",addrtmp,tmpval);

		return ;
	}
	else
	{
		tmpval = rx1922_read_reg(addrtmp);
		dx_kprintf("1922 read  #  reg[0x%x]  = 0x%x \r\n",addrtmp,tmpval);

		return ;
	}




}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), testrf_1922_regWrite, testrf_1922_regWrite, testrf xnd1922 regWrite CMD);

void testrf_ctrl(unsigned char u32flag,unsigned char chno)
{
	unsigned int tmpval =0;


	if(u32flag == 1)
	{
		rx1922_open_chn(chno);
	}
	else
	{

		rx1922_close_chn(chno);
		return ;
	}




}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), testrf_ctrl, testrf_ctrl, testrf xnd1922 regWrite CMD);


void SAM_data_read_AD_4CH(unsigned int tmpsamlen)//读取adc给到fpga的数据
{
	u32 u32SamCtrlReg;
	u32 u32SamData[4];

	unsigned int j;

	mcu_to_fpga_write_kgr_reg(100, (0<<16)|(2<<12)|(0<<8)|(0<<4)|1);
	/*                                      2:4ch                 */
	dx_kprintf("SAM Addr:100, Read Value:0x%x\r\n",  mcu_to_fpga_read_kgr_reg(100));
    mcu_to_fpga_write_kgr_reg(100, (0<<16)|(2<<12)|(0<<8)|(0<<4)|0);
    dx_kprintf("ADC DATA WRITE STAR\r\nT");
	u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    while((u32SamCtrlReg &((u32)(1)<<21)))
    {
    	u32SamCtrlReg = mcu_to_fpga_read_kgr_reg(100);
    }
    dx_kprintf("ADC DATA WRITE DONE\r\n");

	dx_kprintf("SAM_data_read Start!! 0x%x\r\n", u32SamCtrlReg);
	for(int i=0;i<tmpsamlen;i++){
		for (int j=0; j<4; j++)
		{
			u32SamData[j] = mcu_to_fpga_read_kgr_reg(ANTI4B3_SAM_ADDR_BASE + i*4 + j);
		}
		dx_kprintf("%d %d %d %d \r\n",
				(short)(u32SamData[0]&0xffff), (short) ((u32SamData[0]>>16)&0xffff) ,
				(short)(u32SamData[1]&0xffff), (short) ((u32SamData[1]>>16)&0xffff) );
		dx_kprintf("%d %d %d %d \r\n",
				(short)(u32SamData[2]&0xffff), (short) ((u32SamData[2]>>16)&0xffff) ,
				(short)(u32SamData[3]&0xffff), (short) ((u32SamData[3]>>16)&0xffff)) ;
}
	dx_kprintf("SAM_data_read Done!! 0x%x\r\n");
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), SAM_data_read_AD_4CH, SAM_data_read_AD_4CH, SAM_data_read_AD_4CH CMD);


#endif
