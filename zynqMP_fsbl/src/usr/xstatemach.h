#ifndef _XSTATEMACH_H_
#define _XSTATEMACH_H_

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef struct 
{
    int state;      //状态
    void * usrData; //用户自定义数据
}T_STATEMACHAttr;

typedef struct 
{
    int (*  init)(void *);//进入状态，前置初始化
    int (*  handlers)(void *,unsigned char );//
    int (*  exit)(void *);
}T_STATEMACHFuncUnit;


typedef struct 
{
    T_STATEMACHAttr attr;           //状态
    T_STATEMACHFuncUnit* punit;     //状态条目   
}T_STATEMACHCtrl;





#ifdef __cplusplus
}
#endif


#endif
