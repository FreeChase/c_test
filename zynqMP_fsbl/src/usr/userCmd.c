#include "userTask.h"
#include "xil_printf.h"
#include "strSplit/strparse.h"
#include "strSplit/paramparse.h"
#include "uartPowerUpSM.h"
int _flashread(void * p);
int _help(void * p);
int _md_extend(void * p);
void SwitchUpdateBin(void * p);
T_AsciiCmdTable cmdTable[] = {
    {"$help",_help,"print func table"},
    {"$flashread",_flashread,"read flash"},
    {"$md",_md_extend,"start_addr len  unitsize"},
    {"$updatebin",SwitchUpdateBin,"switch to updatebin"},
};



char strParseBuf[ASCII_PARSE_ITEM_UNIT_MAX * (ASCII_PARSE_ITEM_NUM +  1)]={0};
int AsciiDataDecode(T_Uart_AsciiData *pDataCtrl) {
   T_ParamsInfo paramInfo;

   memset((char*)&paramInfo,0,sizeof(paramInfo));
   paramInfo.itemMax = ASCII_PARSE_ITEM_NUM;
   paramInfo.itemUnitMax = ASCII_PARSE_ITEM_UNIT_MAX;
   paramInfo.pdst = strParseBuf;//malloc(paramInfo.itemMax*(paramInfo.itemUnitMax+1));

   parseDataProtocol(pDataCtrl->pdata,&paramInfo);

   for (size_t i = 0; i < ARRAY_SIZE(cmdTable); i++)
   {
        if(strcmp((char*)&paramInfo.pdst[0*paramInfo.itemUnitMax],(char*)cmdTable[i].pcmd) == 0)
        {
            if(cmdTable[i].pfunc)
            	cmdTable[i].pfunc((void*)&paramInfo);
        }
   }
   return 0;

}


int _help(void * p)
{
    for (size_t i = 0; i < ARRAY_SIZE(cmdTable); i++)
    {
        xil_printf("%s : %s\r\n",cmdTable[i].pcmd,cmdTable[i].pDescribe);
    }
}
u32 XFsbl_Qspi32Copy_2(u32 SrcAddress, uintptr_t DestAddress, u32 Length);
int _flashread(void * p)
{
    T_ParamsInfo * pparamInfo = (T_ParamsInfo *)p;
    xil_printf("flashread 0x%x 0x%x len %d\r\n",paramsParsePara( &pparamInfo->pdst[1*pparamInfo->itemUnitMax] ),\
                                    paramsParsePara( &pparamInfo->pdst[2*pparamInfo->itemUnitMax] ),\
                                    paramsParsePara( &pparamInfo->pdst[3*pparamInfo->itemUnitMax] ));

    XFsbl_Qspi32Copy_2(paramsParsePara( &pparamInfo->pdst[1*pparamInfo->itemUnitMax] ),\
                        paramsParsePara( &pparamInfo->pdst[2*pparamInfo->itemUnitMax] ),\
                        paramsParsePara( &pparamInfo->pdst[3*pparamInfo->itemUnitMax] ));
}

void _md(void *start_addr, size_t length, size_t unit_size);
int _md_extend(void * p)
{
    T_ParamsInfo * pparamInfo = (T_ParamsInfo *)p;
    _md(paramsParsePara( &pparamInfo->pdst[1*pparamInfo->itemUnitMax] ),\
    paramsParsePara( &pparamInfo->pdst[2*pparamInfo->itemUnitMax] ),\
    paramsParsePara( &pparamInfo->pdst[3*pparamInfo->itemUnitMax] ));
}


void SwitchUpdateBin(void * p)
{
    ShellUnlockInit();
    Event_Put(EVENT_CMD_UPDATEBIN);
}


void _md(void *start_addr, size_t length, size_t unit_size) {
    if (unit_size != 1 && unit_size != 2 && unit_size != 4 && unit_size != 8) {
        unit_size = 1;
        //return;
    }
    if(length>8192)
    {
        length = 8192;
    }

    size_t bytes_per_line = 16;
    uint8_t *mem = (uint8_t *)start_addr;

    xil_printf("%-10s", "Address");
    for (size_t i = 0; i < bytes_per_line; ++i) {
        xil_printf("0x%X ", i);
    }
    xil_printf("  ASCII\r\n");

    xil_printf("------------------");
    for (size_t i = 0; i < bytes_per_line; ++i) {
        xil_printf("-----");
    }
    xil_printf("\r\n");

    size_t addr = 0;
    while (addr < length) {
        xil_printf("%p  ", (void *)(mem + addr));

        for (size_t i = 0; i < bytes_per_line && addr < length; i += unit_size) {
            switch (unit_size) {
                case 1:
                    xil_printf("%02X  ", mem[addr + i]);
                    break;
                case 2:
                    xil_printf("%04X    ", *((uint16_t *)(mem + addr + i)));
                    break;
                case 4:
                    xil_printf("%08X        ", *((uint32_t *)(mem + addr + i)));
                    break;
                case 8:
                    xil_printf("%016llX   ", *((uint64_t *)(mem + addr + i)));
                    break;
                default:
                    break;
            }
        }


        xil_printf("  ");
        for (size_t i = 0; i < bytes_per_line && addr < length; ++i) {
            char c = (mem[addr + i] >= 32 && mem[addr + i] <= 126) ? mem[addr + i] : '.';
            xil_printf("%c", c);
        }

        xil_printf("\r\n");
        addr += bytes_per_line;
    }
}
