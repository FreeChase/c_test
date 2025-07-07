#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdint.h"
#include <stdio.h>
#include <string.h>

typedef struct 
{
    int itemMax;        
    int itemUnitMax;
    char * pdst;
    int itemNumGet;
}T_ParamsInfo;


uint8_t my_crc(uint8_t * pDat, uint32_t datLen)
{
    uint32_t i = 0;
    uint8_t crc = 0;

    if (pDat == NULL)
    {
        return 0;
    }

    for ( i = 0; i < datLen; i++)
    {
        crc ^= pDat[i];
    }
    
    return crc;
}

char *my_strtok_r(char *s, char * delim,char ** save_ptr)
{
    char * token;
    if(s == NULL) s = *save_ptr;

    s += strspn(s,delim);
    if(*s == '\0')
        return NULL;

    token = s;
    s = strpbrk(token,delim);
    if(s==NULL)
        *save_ptr=strchr(token,'\0');
    else
    {
        *s = '\0';
        *save_ptr = s + 1;
    }
    return token;
}

int isNumeric(const char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] != '-' && str[i] != '.' && (str[i] < '0' || str[i] > '9')) {
            return 0;
        }
    }
    return 1;
}

int parseDataProtocol(char *data , T_ParamsInfo * ptParamsInfo)
{
    char *psp;
    char *token = my_strtok_r(data, ",", &psp);
    // char pDst[50][50];
    int param_count = 0;

    if(ptParamsInfo == NULL)
    {
        //��ʾfail��Ϣ
        return -1;
    }

    while (token != NULL)
    {
        // strcpy(pDst[param_count], token);
        strcpy(ptParamsInfo->pdst + param_count*ptParamsInfo->itemUnitMax , token);
        param_count++;

        token = my_strtok_r(NULL, ",", &psp);
    }

    printf("����: ");
    for (int i = 0; i < param_count; i++)
    {
        printf("\r\nindex %d ",i);
        if (isNumeric(ptParamsInfo->pdst+i*ptParamsInfo->itemUnitMax))
        {
            if (strchr(ptParamsInfo->pdst+i*ptParamsInfo->itemUnitMax, '.'))
            {
                printf(" f ");
                printf("%f ", atof(ptParamsInfo->pdst+i*ptParamsInfo->itemUnitMax));
            }
            else
            {
                printf(" i ");
                printf("%d ", atoi(ptParamsInfo->pdst+i*ptParamsInfo->itemUnitMax));
            }
        }
        else
        {
            printf(" s ");
            printf("%s ", ptParamsInfo->pdst+i*ptParamsInfo->itemUnitMax);
        }
    }
    printf("\n");

    return 0;
}

int main() {
    // ʾ������
    char data[] = "$TSTMODE,print,1,2,3.14,,Hello,world,*FC";
    char data2[] = "BDCNO,,0,";
    char data3[] = "BDCOV,0,25,0,";
    uint8_t crcValue;
    T_ParamsInfo paramInfo;

    memset((char*)&paramInfo,0,sizeof(paramInfo));
    paramInfo.itemMax = 50;
    paramInfo.itemUnitMax = 64;
    paramInfo.pdst = malloc(paramInfo.itemMax*(paramInfo.itemUnitMax+1));

    parseDataProtocol(data,&paramInfo);

    crcValue = my_crc(data3,strlen(data3));
    printf("crc 0x%x\r\n",crcValue);
    return 0;
}
