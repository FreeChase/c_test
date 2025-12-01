#include <stdio.h>

#include "antijam_fpga.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "shell.h"

#include "uart_service.h"
#include "zs_printf.h"
#include "KgrInfo.h"
#include "userTask.h"
#include "userStateMach.h"


int _putchar_shell(char a);
int _isr_putchar_fpga(char a);
int _isr_putchar_bd21_1(char a);
int _isr_putchar_bd21_2(char a);
int _isr_putchar_pc(char a);
int _isr_putchar_ublox(char a);

int _putchar(void* p,char a);
int _getchar(void* p,char *a);
int _sendbyte(char a);


static int _uart_pl_putchar(void *p, char a);
static int _uart_pl_putcharMulti(void *p, char * pdata, int length);
static int _uart_pl_putcharISR(void *p, char a);
static int _uart_pl_getchar(void *p,char *a);
static int _uart_pl_sendbyte(char a);


int Att_set=1;
int Ch_set=0;
int ic_data[2]={0};
int Att_data=0;
int setpoint=8000;
int ch=7;
int max_ch=0;
int max_ch1=5;
int max_value=0;
int max_value_PID=0;
int GXB;
int YZData=500;
int CSSet= 0;
unsigned char gJamDetectFlag = 0;
float GNB=0;
double PS_agl = 45 * PI / 180;
double PS_agh = 45 * PI / 180;
//unsigned char KGR_5 = 0;
int GR_set = 0;
u8 uartSendDataBuffer[128] = { 0 };

u8 gJamDetectEnableFlag = 2;
unsigned char gDebugJSR_flag = 0;
unsigned char PowerShowSet_flag = 0;
int BC_buff[4]={0,0,0,0};
unsigned char DCShowSet_flag = 0;
int DBS_set = 0;
unsigned int gDebugJsr_msTime = 0;

#if 1
//*******************************/
extern int (* gpfUartPutByte[7] )(char ch); 
T_UartCtrl gUartPL[7];

static char uartbuf[512] = {0};

static char uartbuf_rx_bd21_1[4096] = {0};
static char uartbuf_tx_bd21_1[4096] = {0};

static char uartbuf_rx_bd21_2[4096] = {0};
static char uartbuf_tx_bd21_2[4096] = {0};

static char uartbuf_rx_fpga[4096] = {0};
static char uartbuf_tx_fpga[4096] = {0};

static char uartbuf_rx_pc[4096] = {0};
static char uartbuf_tx_pc[4096] = {0};

static char uartbuf_rx_ublox[4096] = {0};
static char uartbuf_tx_ublox[4096] = {0};


extern T_SMCtrl gCtrl;

extern void StateMachInit(void);
void PreInit(void)
{


    memset((char*)&gUartPL,0,sizeof(gUartPL));

	// memset((char*)&gUartPL2,0,sizeof(gUartPL2));
    initializeBuffer(&gUartPL[PLUART_INDEX_0_PC].rx.cirbuf, uartbuf_rx_pc, sizeof(uartbuf_rx_pc));
    initializeBuffer(&gUartPL[PLUART_INDEX_0_PC].tx.cirbuf, uartbuf_tx_pc, sizeof(uartbuf_tx_pc));
    gUartPL[PLUART_INDEX_0_PC].rx.putByte = _uart_pl_putchar;
    gUartPL[PLUART_INDEX_0_PC].rx.putMultiByte = _uart_pl_putcharMulti;
    gUartPL[PLUART_INDEX_0_PC].rx.getByte = _uart_pl_getchar;
    gUartPL[PLUART_INDEX_0_PC].rx.putByteIsr = _uart_pl_putcharISR;
   
    gUartPL[PLUART_INDEX_0_PC].tx.putByte = _uart_pl_putchar;
    gUartPL[PLUART_INDEX_0_PC].tx.putMultiByte = _uart_pl_putcharMulti;
    gUartPL[PLUART_INDEX_0_PC].tx.getByte = _uart_pl_getchar;
    gUartPL[PLUART_INDEX_0_PC].tx.putByteIsr = _uart_pl_putcharISR;
    // gUartPL2.pfSendByte = _uart_pl2_sendbyte;
    // gUartPL2.pfRecvByte = _uart_pl2_getchar; //

    gpfUartPutByte[PLUART_INDEX_0_PC] = _isr_putchar_pc;

	// memset((char*)&gUartPL2,0,sizeof(gUartPL2));
    initializeBuffer(&gUartPL[PLUART_INDEX_1_UBLOX].rx.cirbuf, uartbuf_rx_ublox, sizeof(uartbuf_rx_ublox));
    initializeBuffer(&gUartPL[PLUART_INDEX_1_UBLOX].tx.cirbuf, uartbuf_tx_ublox, sizeof(uartbuf_tx_ublox));
    gUartPL[PLUART_INDEX_1_UBLOX].rx.putByte = _uart_pl_putchar;
    gUartPL[PLUART_INDEX_1_UBLOX].rx.putMultiByte = _uart_pl_putcharMulti;
    gUartPL[PLUART_INDEX_1_UBLOX].rx.getByte = _uart_pl_getchar;
    gUartPL[PLUART_INDEX_1_UBLOX].rx.putByteIsr = _uart_pl_putcharISR;
   
    gUartPL[PLUART_INDEX_1_UBLOX].tx.putByte = _uart_pl_putchar;
    gUartPL[PLUART_INDEX_1_UBLOX].tx.putMultiByte = _uart_pl_putcharMulti;
    gUartPL[PLUART_INDEX_1_UBLOX].tx.getByte = _uart_pl_getchar;
    gUartPL[PLUART_INDEX_1_UBLOX].tx.putByteIsr = _uart_pl_putcharISR;
    // gUartPL2.pfSendByte = _uart_pl2_sendbyte;
    // gUartPL2.pfRecvByte = _uart_pl2_getchar; //

    gpfUartPutByte[PLUART_INDEX_1_UBLOX] = _isr_putchar_ublox;

	// memset((char*)&gUartPL0,0,sizeof(gUartPL0));
    initializeBuffer(&gUartPL[PLUART_INDEX_2_BD21_1].rx.cirbuf, uartbuf_rx_bd21_1, sizeof(uartbuf_rx_bd21_1));
    initializeBuffer(&gUartPL[PLUART_INDEX_2_BD21_1].tx.cirbuf, uartbuf_tx_bd21_1, sizeof(uartbuf_tx_bd21_1));
    gUartPL[PLUART_INDEX_2_BD21_1].rx.putByte = _uart_pl_putchar;
    gUartPL[PLUART_INDEX_2_BD21_1].rx.putMultiByte = _uart_pl_putcharMulti;
    gUartPL[PLUART_INDEX_2_BD21_1].rx.getByte = _uart_pl_getchar;
    gUartPL[PLUART_INDEX_2_BD21_1].rx.putByteIsr = _uart_pl_putcharISR;
    
    gUartPL[PLUART_INDEX_2_BD21_1].tx.putByte = _uart_pl_putchar;
    gUartPL[PLUART_INDEX_2_BD21_1].tx.putMultiByte = _uart_pl_putcharMulti;
    gUartPL[PLUART_INDEX_2_BD21_1].tx.getByte = _uart_pl_getchar;
    gUartPL[PLUART_INDEX_2_BD21_1].tx.putByteIsr = _uart_pl_putcharISR;
    // gUartPL0.pfSendByte = _uart_pl0_sendbyte;
    // gUartPL0.pfRecvByte = _uart_pl0_getchar; //

    gpfUartPutByte[PLUART_INDEX_2_BD21_1] = _isr_putchar_bd21_1;

    //Uart PL2
    // memset((char*)&gUartPL2,0,sizeof(gUartPL2));
    initializeBuffer(&gUartPL[PLUART_INDEX_3_BD21_2].rx.cirbuf, uartbuf_rx_bd21_2, sizeof(uartbuf_rx_bd21_2));
    initializeBuffer(&gUartPL[PLUART_INDEX_3_BD21_2].tx.cirbuf, uartbuf_tx_bd21_2, sizeof(uartbuf_tx_bd21_2));
    gUartPL[PLUART_INDEX_3_BD21_2].rx.putByte = _uart_pl_putchar;
    gUartPL[PLUART_INDEX_3_BD21_2].rx.putMultiByte = _uart_pl_putcharMulti;
    gUartPL[PLUART_INDEX_3_BD21_2].rx.getByte = _uart_pl_getchar;
    gUartPL[PLUART_INDEX_3_BD21_2].rx.putByteIsr = _uart_pl_putcharISR;
   
    gUartPL[PLUART_INDEX_3_BD21_2].tx.putByte = _uart_pl_putchar;
    gUartPL[PLUART_INDEX_3_BD21_2].tx.putMultiByte = _uart_pl_putcharMulti;
    gUartPL[PLUART_INDEX_3_BD21_2].tx.getByte = _uart_pl_getchar;
    gUartPL[PLUART_INDEX_3_BD21_2].tx.putByteIsr = _uart_pl_putcharISR;
    // gUartPL2.pfSendByte = _uart_pl2_sendbyte;
    // gUartPL2.pfRecvByte = _uart_pl2_getchar; //

    gpfUartPutByte[PLUART_INDEX_3_BD21_2] = _isr_putchar_bd21_2;

    //mcu uart
    // memset((char*)&gUartMcu,0,sizeof(gUartMcu));
    initializeBuffer(&gUartPL[PLUART_INDEX_4_FPGA].rx.cirbuf, uartbuf_rx_fpga, sizeof(uartbuf_rx_fpga));
    initializeBuffer(&gUartPL[PLUART_INDEX_4_FPGA].tx.cirbuf, uartbuf_tx_fpga, sizeof(uartbuf_tx_fpga));
    gUartPL[PLUART_INDEX_4_FPGA].rx.putByte = _uart_pl_putchar;
    gUartPL[PLUART_INDEX_4_FPGA].rx.putMultiByte = _uart_pl_putcharMulti;
    gUartPL[PLUART_INDEX_4_FPGA].rx.getByte = _uart_pl_getchar;
    gUartPL[PLUART_INDEX_4_FPGA].rx.putByteIsr = _uart_pl_putcharISR;
    
    gUartPL[PLUART_INDEX_4_FPGA].tx.putByte = _uart_pl_putchar;
    gUartPL[PLUART_INDEX_4_FPGA].tx.putMultiByte = _uart_pl_putcharMulti;
    gUartPL[PLUART_INDEX_4_FPGA].tx.getByte = _uart_pl_getchar;
    gUartPL[PLUART_INDEX_4_FPGA].tx.putByteIsr = _uart_pl_putcharISR;
//    gUartMcu.pfSendByte = _uart_mcu_sendbyte;
//    gUartMcu.pfRecvByte = _uart_mcu_getchar;//
//    __HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
    gpfUartPutByte[PLUART_INDEX_4_FPGA] = _isr_putchar_fpga;

	//* State Machine Init
	StateMachInit();
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


int _putchar_shell(char a)
{
    return writeBufferNoMutex(&gUartShell.rx.cirbuf, a);
}

int _isr_putchar_fpga(char a)
{
    return writeBufferNoMutex(&gUartPL[PLUART_INDEX_4_FPGA].rx.cirbuf, a);
}

int _isr_putchar_bd21_1(char a)
{
    return writeBufferNoMutex(&gUartPL[PLUART_INDEX_2_BD21_1].rx.cirbuf, a);
}

int _isr_putchar_bd21_2(char a)
{
    return writeBufferNoMutex(&gUartPL[PLUART_INDEX_3_BD21_2].rx.cirbuf, a);
}

int _isr_putchar_pc(char a)
{
    return writeBufferNoMutex(&gUartPL[PLUART_INDEX_0_PC].rx.cirbuf, a);
}

int _isr_putchar_ublox(char a)
{
    return writeBufferNoMutex(&gUartPL[PLUART_INDEX_1_UBLOX].rx.cirbuf, a);
}


int _putchar(void* p,char a)
{
    return writeBufferNoMutex(p, a);
}


int _getchar(void* p,char *a)
{
    return readBufferMutex(p, a);
}

int _sendbyte(char a)
{
    
    // huart1.Instance->DR = a;
    // while(!__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC));
    return 0;
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

static int _uart_pl_sendbyte(char a)
{
    
    // huart4.Instance->DR = a;
    // while(!__HAL_UART_GET_FLAG(&huart4,UART_FLAG_TC));
    return 0;
}

#endif
#if 0

void AttConfigTask(void *argument)
{
		//int setpoint=18000;

		int dt=1;
		s16 mean1=0;
		s16 mean2=0;
		s16 mean3=0;
		s16 mean4=0;
		s32 mean5=0;
		s32 mean6=0;
		s32 mean7=0;
		s32 mean8=0;
		float tep=0;
		int Value[9] = { 0 };
		int mean[8] = { 0 };
		int KGR_4_7_change=0;
		unsigned int RawCnt;

		unsigned int tmpJsrdebuCnt = 0;
		unsigned int tmpbefpow = 0;
		unsigned int tmpaftpow = 0;
		float tmpjsr = 0;
		
		if(g_HwModuleVer==0)
			KGR_4_7_change=0;
		else if(g_HwModuleVer==7)
			KGR_4_7_change=0;
		else if((g_HwModuleVer==HW_KGR_B3_VER_4_54)||(g_HwModuleVer==HW_KGR_S_VER_4_54)||(g_HwModuleVer==HW_KGR_B1_VER_4_54))
			KGR_4_7_change=0;
		if(g_HwModuleVer == HW_KGR_B3_VER_4_54)
		{
			CSSet = 76;
		}

		if(g_HwModuleVer == HW_KGR_B1_VER_4_54)
		{
			CSSet = 74;
		}		

		if(g_HwModuleVer == HW_KGR_S_VER_4_54)
		{
			CSSet = 73;
			KGR_5 = 1;
		}
		
		mcu_to_fpga_write_kgr_reg(11,fip);
		mcu_to_fpga_write_kgr_reg(10, 1);//

		mcu_to_fpga_write_kgr_reg(32, ch-KGR_4_7_change);
		mcu_to_fpga_write_kgr_reg(152, 1|1<<4);

		mcu_to_fpga_write_kgr_reg(99, 1|0<<4|0<<8);
		mcu_to_fpga_write_kgr_reg(147,0x333);
		mcu_to_fpga_write_kgr_reg(141,0x800);
		if(g_HwModuleVer == HW_KGR_S_VER_4_54)
		{
			mcu_to_fpga_write_kgr_reg(141,0x1200);
		}
		if(KGR_5)
		{
			mcu_to_fpga_write_kgr_reg(7,0x01237);		
		}
		else
		{
			mcu_to_fpga_write_kgr_reg(7,0x2310);
		}

		mcu_to_fpga_write_kgr_reg(149,0x3);
		mcu_to_fpga_write_kgr_reg(151,0x1);


		//设置功率统计长度,默认是32768;
		stap_sjr_length_set(2);

		//
		//stap_sjr_refchn_set(4);

		//设置功率统计更新时间
		stap_sjr_refresh_time(10000);

		//sjr统计默认是打开状态
		stap_sjr_enable(1);
		

	    while (1)
	    {
	    	vTaskDelay(10);



	    	if(mcu_to_fpga_read_kgr_reg(2) == 1)
	    	{
	    		mcu_to_fpga_write_kgr_reg(1, 1);
	    		read_power(Value,mean);
	    		max_value=(int)find_max_value(Value);


	    		//ADC_DATA_display(&Value);
	    		if(Att_set==1)
	    		{

	    			max_value_PID=(int)(max_value/10);
	    			ic_data[0]=PID_Controller(setpoint,max_value_PID,dt);
		    		if(max_value_PID<300)
		    		{
		    			GNB= 0;
						gJamDetectFlag = 0;
						
		    		}		    		
		    		else
		    		{
		    			GNB=CSSet+ic_data[0]/4;
		    			gJamDetectFlag = 1;
		    		}

					tmpjsr = stap_sjr_get(&tmpbefpow,&tmpaftpow);
					if(g_HwModuleVer != HW_KGR_S_VER_4_54)
					{
						tmpjsr = tmpjsr + 15;
					
						if(tmpjsr > 30)
						{						
							gJamDetectFlag = 1;
						}
						else
						{
							tmpjsr = 0;
						}

						if(tmpjsr < 90)
						{
							GNB = tmpjsr;
						}						
					}

					

				
		    		Value[0]=Value[0]+BC_buff[0];
		    		Value[1]=Value[1]+BC_buff[1];
		    		Value[2]=Value[2]+BC_buff[2];
		    		Value[3]=Value[3]+BC_buff[3];
		    		if(Ch_set==1)
		    			max_ch=max_value%10;
		    		if(Ch_set==2)
		    			max_ch=find_min_value(Value);


	    			if((max_ch1!=max_ch)&&(Ch_set!=0)&&(abs(Value[max_ch1]-Value[max_ch])>YZData))
	    			{
	    				switch(max_ch)
						{
							case 0:if(KGR_5)ch=0x01237;else ch=0x0123;break;
							case 1:if(KGR_5)ch=0x12307;else ch=0x1230;break;
							case 2:if(KGR_5)ch=0x23107;else ch=0x2310;break;
							case 3:if(KGR_5)ch=0x32107;else ch=0x3210;break;
							case 7:ch=0x73210;break;
						}
	    				mcu_to_fpga_write_kgr_reg(7, ch);
	    				max_ch1=max_ch;
	    				dx_kprintf("Ch Change 0x%x\r\n", ch);
	    			}


	    		}
	    		else
	    		{
	    			ic_data[0]=Att_data;
	    		}
	    		if(ic_data[0]!=ic_data[1]){

	    			ATT_ALL_SET(ic_data[0]);
	    		}
	    		else
	    		{
	    			GXB=ic_data[0]/4-95+130;
	    		}
	    		if(PowerShowSet_flag){

	    			//boardTempGet(&tep);
	    			ch=mcu_to_fpga_read_kgr_reg(7);

	    			dx_kprintf("//////////////////////CHOPEWER:%d %d %d %d %d %d %d %d [%d %d %d %x %d %d %d %d %d %d %d %f %f]\r\n", Value[0], Value[1], Value[2], Value[3], Value[4], Value[5], Value[6], Value[7],setpoint,max_value,max_ch,ch,ic_data[0],Att_set,g_HwModuleVer,(abs(Value[max_ch1]-Value[max_ch])),(Value[max_ch1]-Value[max_ch]),max_ch1,max_ch,GNB,tep);

	    		}
	    			if(DCShowSet_flag)
	    		{
	    			dx_kprintf("//////////////////////DCPEWER:%d %d %d %d %d %d %d %d\r\n", mean[0], mean[1], mean[2], mean[3], mean[4], mean[5], mean[6], mean[7]);
	    			mean1=mcu_to_fpga_read_kgr_reg(3)%0x10000+mean[0];
	    			mean2=mcu_to_fpga_read_kgr_reg(3)/0x10000+mean[1];
	    			mean3=mcu_to_fpga_read_kgr_reg(4)%0x10000+mean[2];
	    			mean4=mcu_to_fpga_read_kgr_reg(4)/0x10000+mean[3];
	    			mean5=mcu_to_fpga_read_kgr_reg(5)%0x10000+mean[4];
	    			mean6=mcu_to_fpga_read_kgr_reg(5)/0x10000+mean[5];
	    			mean7=mcu_to_fpga_read_kgr_reg(6)%0x10000+mean[6];
	    			mean8=mcu_to_fpga_read_kgr_reg(6)/0x10000+mean[7];

	    			mcu_to_fpga_write_kgr_reg(3,((mean2&0xffff)<<16)|(mean1&0xffff));
	    			mcu_to_fpga_write_kgr_reg(4,((mean4&0xffff)<<16)|(mean3&0xffff));
	    			mcu_to_fpga_write_kgr_reg(5,((mean6&0xffff)<<16)|(mean5&0xffff));
	    			mcu_to_fpga_write_kgr_reg(6,((mean8&0xffff)<<16)|(mean7&0xffff));
	    		}
				//通过调试串口打印jsr值
				if (gDebugJSR_flag == 1)
				{
					if(tmpJsrdebuCnt > gDebugJsr_msTime)
					{
						tmpjsr = stap_sjr_get(&tmpbefpow,&tmpaftpow);
						dx_kprintf("current sjr [%f]--bef = %d aft = %d\r\n",tmpjsr,tmpbefpow,tmpaftpow);
						tmpJsrdebuCnt = 0;
					}
					tmpJsrdebuCnt++;
				}



	    		ic_data[1]=ic_data[0];
	    	}
	    }
}

void JamDetectTask(void *argument)
{

	unsigned int tmpJsrdebuCnt = 0;
	unsigned int tmpbefpow = 0;
	unsigned int tmpaftpow = 0;
	float tmpjsr = 0;
	int valbuffer[9] = {0};
	int maxValtemp = 0;
	int detectJamflag = 0;
	int detectNoJamCnt = 0;

	while (1)
	{
		vTaskDelay(1);
		{
				//通过调试串口打印jsr值
				if (gDebugJSR_flag >= 1)
				{
					if(tmpJsrdebuCnt > gDebugJsr_msTime)
					{
						tmpjsr = stap_sjr_get(&tmpbefpow,&tmpaftpow);
						dx_kprintf("current sjr [%f]--bef = %d aft = %d  max PowVal [%d]\r\n",tmpjsr,tmpbefpow,tmpaftpow,maxValtemp);
						tmpJsrdebuCnt = 0;
						
					}
					tmpJsrdebuCnt++;
					
				}
#if 1
				if(gJamDetectEnableFlag >= 1)
				{
					
					if(gJamDetectEnableFlag >= 2)
					{
						read_power(valbuffer,NULL);
			    		maxValtemp=(int)find_max_value(valbuffer);
						maxValtemp = maxValtemp/10;						
					}

					if(maxValtemp > DETCET_JAM_POWER_THRESHOLD)
					{
						if(detectJamflag == 0)
						{
							dx_kprintf("Module Detect  Jam !!!!!!------------<<<<\r\n",tmpjsr,tmpbefpow,tmpaftpow,maxValtemp);
							if(KGR_5 == 1)
							{
								mcu_to_fpga_write_kgr_reg(0x83,0xef00);
							}
							else
							{
								mcu_to_fpga_write_kgr_reg(0x83,0x8600);
							}
						}
						detectJamflag = 1;
						detectNoJamCnt = 0;
					}
					else
					{
						if(detectJamflag == 1)
						{
							if(detectNoJamCnt > 1000)
							{
								dx_kprintf("Module No Found Jam ------------<<<<\r\n",tmpjsr,tmpbefpow,tmpaftpow,maxValtemp);
								detectJamflag = 0;
								mcu_to_fpga_write_kgr_reg(0x83,0x470000);
							}

							detectNoJamCnt++;
						}
						
						
					}					
				}
#endif
		}
	}
}

#endif
