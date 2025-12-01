/**
@file		IceErrCtrl.h 错误调试信息输出
@author		xIANG wEi waterknows@gmail.com
@date 		2007/8/1   16:19
@version	v1.0 Copyright (c) 2007 - All Rights Reserved
@warning 	适用于vxworks、wince、windows操作系统.特别注意，调试输出信息每次不可大于MAX_TRACE_STR_LEN字节，多者截断
默认情况下，输入的调试信息自动在最后加换行
根据所在的操作系统需配合下列中一个文件使用：
IceErrCtrlVxwork.c
IceErrCtrlWin32.cpp
IceErrCtrlCE.cpp
*********************************************************************/
#ifndef _ICEERRCTRL_H_WP_2010_04_28_
#define _ICEERRCTRL_H_WP_2010_04_28_

#ifdef __cplusplus
extern "C"{
#endif

#include "IceType.h"
void IceLogHexStr(const S8_CHAR* lpszFormat, const u8* pData, u32 nByteLen);
#define LOGBYTE01(nHeadInfo, pStr, nByteLen) IceLogHexStr(nHeadInfo, pStr, nByteLen);

#define VERIFY_LOG(a) if(!(a)){LOG_E(#a);}
#define VERIFY_LOG_RET(a) if(!(a)){LOG_E(#a);return;}
#define VERIFY_LOG_RETVAL(a,retVal) if(!(a)){LOG_E(#a); return retVal;}
#define VERIFY_S32(a) VERIFY_LOG_RETVAL((a)==ERR_NO_ERROR, DX_FALSE);

#define VERIFY_RET(a) if(!(a)){return;}
#define VERIFY_RETVAL(a,retVal) if(!(a)){return retVal;}
#define VERIFY_RET_ERR(a,retVal) if((a!=ERR_NO_ERROR)){return retVal;}

#define VERIFY_RSLT(a) \
{\
	s32 errVal = (a);\
	if(errVal != ERR_NO_ERROR)\
	{\
		LOG_E(#a);\
	}\
}
#define VERIFY_RSLT_RET(a) \
{\
	s32 errVal = (a);\
	if(errVal != ERR_NO_ERROR)\
	{\
		LOG_E(#a);\
		return;\
	}\
}
#define VERIFY_RSLT_RETVAL(a, retVal) \
{\
	s32 errVal = (a);\
	if(errVal != ERR_NO_ERROR)\
	{\
		LOG_E(#a);\
		return retVal;\
	}\
}
#define VERIFY_RSLT_RETSELF(a) \
{\
	s32 errVal = (a);\
	if(errVal != ERR_NO_ERROR)\
	{\
		LOG_E(#a);\
		return (errVal);\
	}\
}

#ifdef __cplusplus
}
#endif

#endif /* _ICEERRCTRL_H_WP_2010_04_28_ */

