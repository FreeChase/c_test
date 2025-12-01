#ifndef __STRPARSE_H__
#define __STRPARSE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdint.h"
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int itemMax;
    int itemUnitMax;
    char * pdst;
    int itemNumGet;
}T_ParamsInfo;


int parseDataProtocol(char *data , T_ParamsInfo * ptParamsInfo);



#ifdef __cplusplus
}
#endif

#endif 

