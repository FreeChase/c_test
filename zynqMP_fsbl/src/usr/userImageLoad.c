#include "../xfsbl_main.h"
#include "userFirmwareUpdate.h"
extern u8 ReadBuffer[];

extern T_AddrSwitchTable addrTable[eFirewareImage_Max];
/*****************************************************************************/
/**
 * This function validates the image header
 *
 * @param	FsblInstancePtr is pointer to the XFsbl Instance
 *
 * @return	returns the error codes described in xfsbl_error.h on any error
 * 			returns XFSBL_SUCCESS on success
 *
 ******************************************************************************/
// static u32 XFsbl_ValidateHeader(XFsblPs * FsblInstancePtr)
u32 XImage_ValidateHeader(XFsblPs * FsblInstancePtr,uint32_t u32ImageAddr)
{
	u32 Status;
	u32 MultiBootOffset;
	u32 BootHdrAttrb=0U;
	u32 FlashImageOffsetAddress;
	u32 EfuseCtrl;
	u32 ImageHeaderTableAddressOffset=0U;
	u32 FsblEncSts = 0U;
#ifdef XFSBL_SECURE
	u32 Size;
	u32 AcOffset=0U;
#endif
	/**
	 * Read the Multiboot Register
	 */
	MultiBootOffset = XFsbl_In32(CSU_CSU_MULTI_BOOT);
	XFsbl_Printf(DEBUG_INFO,"Multiboot Reg : 0x%0lx \n\r", MultiBootOffset);

	/**
	 *  Calculate the Flash Offset Address
	 *  For file system based devices, Flash Offset Address should be 0 always
	 */
    #if 0
	if (FsblInstancePtr->SecondaryBootDevice == 0U) {
		if (!((FsblInstancePtr->PrimaryBootDevice == XFSBL_SD0_BOOT_MODE)
				|| (FsblInstancePtr->PrimaryBootDevice == XFSBL_EMMC_BOOT_MODE)
				|| (FsblInstancePtr->PrimaryBootDevice == XFSBL_SD1_BOOT_MODE)
				|| (FsblInstancePtr->PrimaryBootDevice == XFSBL_SD1_LS_BOOT_MODE)
				|| (FsblInstancePtr->PrimaryBootDevice == XFSBL_USB_BOOT_MODE))) {
			FsblInstancePtr->ImageOffsetAddress = MultiBootOffset
					* XFSBL_IMAGE_SEARCH_OFFSET;
		}
	}
	else
	{
		if (!((FsblInstancePtr->SecondaryBootDevice == XFSBL_SD0_BOOT_MODE)
				|| (FsblInstancePtr->SecondaryBootDevice == XFSBL_EMMC_BOOT_MODE)
				|| (FsblInstancePtr->SecondaryBootDevice == XFSBL_SD1_BOOT_MODE)
				|| (FsblInstancePtr->SecondaryBootDevice == XFSBL_SD1_LS_BOOT_MODE)
				|| (FsblInstancePtr->SecondaryBootDevice == XFSBL_USB_BOOT_MODE))) {
			FsblInstancePtr->ImageOffsetAddress = MultiBootOffset
					* XFSBL_IMAGE_SEARCH_OFFSET;
		}
	}
    #endif
    FsblInstancePtr->ImageOffsetAddress = u32ImageAddr;
	FlashImageOffsetAddress = FsblInstancePtr->ImageOffsetAddress;

	/* Copy boot header to internal memory */
	Status = FsblInstancePtr->DeviceOps.DeviceCopy(FlashImageOffsetAddress,
	                   (PTRSIZE )ReadBuffer, XIH_BH_MAX_SIZE);
	if (XFSBL_SUCCESS != Status) {
			XFsbl_Printf(DEBUG_GENERAL,"Device Copy Failed \n\r");
			goto END;
	}
#ifdef XFSBL_SECURE
	/* copy IV to local variable */
	XFsbl_MemCpy(Iv, ReadBuffer + XIH_BH_IV_OFFSET, XIH_BH_IV_LENGTH);
#endif
	/**
	 * Read Boot Image attributes
	 */
	BootHdrAttrb = Xil_In32((UINTPTR)ReadBuffer +
					XIH_BH_IMAGE_ATTRB_OFFSET);
	FsblInstancePtr->BootHdrAttributes = BootHdrAttrb;

	/*
	 * Update PMU Global general storage register5 bit 3 with FSBL encryption
	 * status if either FSBL encryption status in boot header is true or
	 * ENC_ONLY eFuse bit is programmed.
	 *
	 * FSBL encryption information in boot header:
	 * If authenticate only bits 5:4 are set, boot image is only RSA signed
	 * though encryption status in BH is non-zero.
	 * Boot image is decrypted only when BH encryption status is not 0x0 and
	 * authenticate only bits value is other than 0x3
	 */
	if (((Xil_In32((UINTPTR)ReadBuffer + XIH_BH_ENC_STS_OFFSET) != 0x0U) &&
			((BootHdrAttrb & XIH_BH_IMAGE_ATTRB_AUTH_ONLY_MASK) !=
					XIH_BH_IMAGE_ATTRB_AUTH_ONLY_MASK)) ||
			((XFsbl_In32(EFUSE_SEC_CTRL) & EFUSE_SEC_CTRL_ENC_ONLY_MASK) !=
					0x0U)) {
		FsblEncSts = XFsbl_In32(PMU_GLOBAL_GLOB_GEN_STORAGE5) |
				XFSBL_FSBL_ENCRYPTED_MASK;
		XFsbl_Out32(PMU_GLOBAL_GLOB_GEN_STORAGE5, FsblEncSts);
	}

	/**
	 * Read the Image Header Table offset from
	 * Boot Header
	 */
	ImageHeaderTableAddressOffset = Xil_In32((UINTPTR)ReadBuffer +
					XIH_BH_IH_TABLE_OFFSET);

	XFsbl_Printf(DEBUG_INFO,"Image Header Table Offset 0x%0lx \n\r",
			ImageHeaderTableAddressOffset);

	/**
	 * Read Efuse bit and check Boot Header for Authentication
	 */
	EfuseCtrl = XFsbl_In32(EFUSE_SEC_CTRL);

	if (((EfuseCtrl & EFUSE_SEC_CTRL_RSA_EN_MASK) != 0x00)
		&& ((BootHdrAttrb & XIH_BH_IMAGE_ATTRB_RSA_MASK) ==
					XIH_BH_IMAGE_ATTRB_RSA_MASK)) {
		Status = XFSBL_ERROR_BH_AUTH_IS_NOTALLOWED;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_BH_AUTH_IS_NOTALLOWED"
					" when eFSUE RSA bit is set \n\r");
		goto END;
	}

	/* If authentication is enabled */
	if (((EfuseCtrl & EFUSE_SEC_CTRL_RSA_EN_MASK) != 0U) ||
	    ((BootHdrAttrb & XIH_BH_IMAGE_ATTRB_RSA_MASK)
		== XIH_BH_IMAGE_ATTRB_RSA_MASK)) {
		FsblInstancePtr->AuthEnabled = TRUE;
		XFsbl_Printf(DEBUG_INFO,"Authentication Enabled\r\n");
#ifdef XFSBL_SECURE

#else
                XFsbl_Printf(DEBUG_GENERAL,
			"XFSBL_ERROR_SECURE_NOT_ENABLED\r\n");
                Status = XFSBL_ERROR_SECURE_NOT_ENABLED;
                goto END;
#endif
	}
	else {
		/* Read Image Header and validate Image Header Table */
		Status = XFsbl_ReadImageHeader(&FsblInstancePtr->ImageHeader,
						&FsblInstancePtr->DeviceOps,
						FlashImageOffsetAddress,
						FsblInstancePtr->ProcessorID,
						ImageHeaderTableAddressOffset);
		if (XFSBL_SUCCESS != Status) {
			goto END;
		}
	}
END:
	return Status;
}
