#ifndef FRMFUNC_H_
#define FRMFUNC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "IceType.h"

//#define U8_BIT_LEN  (8)
//#define U16_BIT_LEN	(16)
//#define U32_BIT_LEN (32)
#define F64_BIT_LEN (64)
#define MAX_FRAME_LEN (4096) /* 帧头+帧ID+数据长度+校验+帧尾共6字节 */
#define MAX_FRAME_BITLEN (MAX_FRAME_LEN*8)

#define FRM_HEAD_POS (0)
#define FRM_DATA_POS (4) /* 基0 */
#define FRM_HEAD_VALUE	 (0x08)
//结束
#define FRM_TAIL_VALUE	 (0x77)

bool_t FrmGetU8Data(u8 *pDes, const u8 *pData, u32 *pBytePos);

bool_t FrmSetU8Data(u8 *pDes, u8 u8Data, u32 *pBytePos);

bool_t FrmGetF64Data(f64 *pDes, const u8 *pData, u32 *pBytePos);

bool_t FrmSetF64Data(u8 *pDes, f64 f64Data, u32 *pBytePos);

bool_t FrmSetU64Data(u8 *pDes, u64 u64Data, u32 *pBytePos);

bool_t FrmGetU64Data(u64 *pDes, const u8 *pData, u32 *pBytePos);

bool_t FrmGetS16Data(s16 *pDes, const u8 *pData, u32 *pBytePos);

bool_t FrmSetS16Data(u8 *pDes, s16 s16Data, u32 *pBytePos);

bool_t FrmGetU16Data(u16 *pDes, const u8 *pData, u32 *pBytePos);

bool_t FrmSetU16Data(u8 *pDes, u16 u16Data, u32 *pBytePos);

bool_t FrmGetS32Data(s32* pDes, const u8* pData, u32* pBytePos);

bool_t FrmGetS64Data(s64* pDes, const u8* pData, u32* pBytePos);

bool_t FrmSetS32Data(u8 *pDes, s32 s32Data, u32 *pBytePos);

bool_t FrmGetU32Data(u32 *pDes, const u8 *pData, u32 *pBytePos);

bool_t FrmSetU32Data(u8 *pDes, u32 u32Data, u32 *pBytePos);

bool_t FrmSetF32Data(u8 *pDes, f32 f32Data, u32 *pBytePos);

bool_t FrmGetF32Data(f32 *pDes, const u8 *pData, u32 *pBytePos);

bool_t FrmCalcCrc(u8 *pCrc, const u8 *pData, u32 u32DataLen);

#ifdef __cplusplus
}
#endif

#endif
