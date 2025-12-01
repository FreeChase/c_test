/******************************************************************************
 * Copyright (C) 2024 ZS Development Team, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @file dev_manager.h
 * @brief
 * @version 0.1
 * Change Logs:
 * Date           Author       Notes
 * 2024-12-19     Andy      first version
******************************************************************************/
#ifndef __DEV_MANAGER_H__
#define __DEV_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "dxdef.h"

void PreDevManagerInit(void);
void UartServiceInit(void);

void module_version_printf();

void module_versionAll_printf();



#ifdef __cplusplus
}
#endif

#endif 
// __DEV_MANAGER_H__
