#include "stdint.h"
#include "stdbool.h"
#include "userFirmwareUpdate.h"
#include "xdriverPsQspi.h"
#include "userTask.h"

T_UpdateCtrl g_UpdateCtrl;

T_AddrSwitchTable addrTable[] ={
    {"zsfsbl",IMAGE_BASE_ADDR_FSBL},
    {"zsbit",IMAGE_BASE_ADDR_BIT},
    {"zsapp",IMAGE_BASE_ADDR_APP},
};

// 判断membuf是否都等于byte c
bool MemCmpToChar(const uint8_t* src, uint8_t c, uint32_t len) {
    if (0 == src || 0 == len) {
        return ;
    }

    for (uint32_t i=0; i<len; i++) {
        if (src[i] != c) {
            return false;
        }
    }
    return true;
}
//* if success, return true ,else return false
int FirewareUpdate_Init(uint8_t * pdat ,uint32_t u32dataSize)
{
    g_UpdateCtrl.attr.u32AddrStar = 0xffffffff;
    //* search addr
    for (size_t i = 0; i < ARRAY_SIZE(addrTable); i++)
    {
        if(strstr(pdat,addrTable[i].name))
        {
            g_UpdateCtrl.attr.u32AddrStar = addrTable[i].u32addr;
            break;
        }
    }
    //* check file name is valid?
    if(g_UpdateCtrl.attr.u32AddrStar == 0xffffffff)
    {
        g_UpdateCtrl.attr.state = eFirewareUpdate_Init;
        return false;
    }
    if( strlen(pdat) > sizeof(g_UpdateCtrl.attr.fileName))
    {
        return false;
    }
    strcpy(g_UpdateCtrl.attr.fileName, pdat);

    g_UpdateCtrl.attr.state = eFirewareUpdate_Update;
    g_UpdateCtrl.attr.result = eFirewareUpdate_Result_Updating;

    g_UpdateCtrl.attr.u32FileSize = u32dataSize;
    g_UpdateCtrl.attr.u32alreayWrite = 0;
    

    if (QspiPsuInstance_2.Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL)
    {
        g_UpdateCtrl.attr.u32PageSize = Flash_Config_Table[FCTIndex].PageSize * 2;
        // g_UpdateCtrl.attr.u32ErSize = 2 * 64 * 1024;
        g_UpdateCtrl.attr.u32ErSize = Flash_Config_Table[FCTIndex].SectSize * 2;
    }
    else
    {
        g_UpdateCtrl.attr.u32PageSize = Flash_Config_Table[FCTIndex].PageSize;
        g_UpdateCtrl.attr.u32ErSize = Flash_Config_Table[FCTIndex].SectSize ;

    }

    return true;
}


int FirewareUpdate_Update(uint8_t * pdat ,uint32_t u32dataSize)
{
    
    uint8_t i=0;
    uint8_t rBuf[64] = {0};
    uint8_t rtmpBuf[2048]={0};
    int32_t s32remain = 0;
    int32_t s32remainPage = 0;
    int32_t s32cacheSize = 0;

    uint32_t u32curWriteSize = 0;
    uint32_t u32leftSize = u32dataSize;
    int s32ret;
    uint32_t u32AddrBase=0;
    uint32_t u32AddrOffset=0;
    int ret_err;
    int result ;
    int timeMS = 0;
    uint32_t PageSize=0;
    bool bNoneEmptyFg = 0;

    uint32_t timeout = 2U;

    static int eraseFlag = 0;

    if(!u32dataSize || g_UpdateCtrl.attr.state != eFirewareUpdate_Update)
    {

        return -1;
    }

    //* get basic erase size and program size
    PageSize = g_UpdateCtrl.attr.u32PageSize;
    //* loop write, until u32leftSize equal to zero
    while ( u32leftSize )
    {
        //* erase block in first write
        if ((g_UpdateCtrl.attr.u32alreayWrite % g_UpdateCtrl.attr.u32ErSize == 0) )
        {

            u32AddrBase = g_UpdateCtrl.attr.u32AddrStar;
            u32AddrOffset = g_UpdateCtrl.attr.u32alreayWrite;
//*new block, judge the necessary of erase
// sfud_read(sfud_dev, u32AddrBase+u32AddrOffset, sizeof(rBuf), rBuf);
#if 0
            XFsbl_Qspi32Copy_2(u32AddrBase+u32AddrOffset, sizeof(rBuf),rBuf);
        
            if ( !MemCmpToChar((void *)rBuf, 0xFF, sizeof(rBuf))) 
            {   
                //*erase cur block
                sfud_erase(sfud_dev, u32AddrBase+u32AddrOffset, 4096U);
                LOG_SERIAL_OUTPUT("########erase 1 0x%x\r\n",u32AddrBase+u32AddrOffset);
            }
#endif
            //* tmp, erase directly
            XFsbl_Qspi32Erase_2(u32AddrBase + u32AddrOffset, g_UpdateCtrl.attr.u32ErSize);
            eraseFlag = 1;
        }

        /* code */
        s32remainPage = PageSize - (g_UpdateCtrl.attr.u32alreayWrite % PageSize); //* page 内剩余 size
        u32AddrBase = g_UpdateCtrl.attr.u32AddrStar;
        u32AddrOffset = g_UpdateCtrl.attr.u32alreayWrite;

        // TODO get s32cacheSize
        if(u32leftSize > s32remainPage)
        {
            //* align page size to write
            // sfud_write(sfud_dev, u32AddrBase+u32AddrOffset,s32cacheSize, pdat);
            XFsbl_Qspi32PageWrite_2(u32AddrBase + u32AddrOffset, &pdat[ u32dataSize - u32leftSize ], s32remainPage);
            g_UpdateCtrl.attr.u32alreayWrite += s32remainPage;
            u32leftSize -= s32remainPage;
        }
        else
        {
            //* write left size
            XFsbl_Qspi32PageWrite_2(u32AddrBase + u32AddrOffset, &pdat[ u32dataSize - u32leftSize ], u32leftSize);
            g_UpdateCtrl.attr.u32alreayWrite += u32leftSize;
            u32leftSize -= u32leftSize;
        }

    }

    return result;
}


int FirewareUpdate_Over(void)
{
    //* dosomething
    g_UpdateCtrl.attr.result = eFirewareUpdate_Result_Success;
}