/**
 ****************************************************************************************
 *
 * @file user_custs1_def.h
 *
 * @brief Custom1 Server (CUSTS1) profile database declarations.
 *
 * Copyright (C) 2016. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com> and contributors.
 *
 * This file was auto generated 2/4/2017 9:13:54 AM
 * by DA1458x Custom GATT Service Constructor (v1.1b)
 ****************************************************************************************
 */

#ifndef _USER_CUSTS1_DEF_H_
#define _USER_CUSTS1_DEF_H_

/**
 ****************************************************************************************
 * @defgroup USER_CONFIG
 * @ingroup USER
 * @brief Custom1 Server (CUSTS1) profile database declarations.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "attm_db_128.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define DEF_CUST1_SVC_UUID_128   {0x83, 0x6E, 0xEC, 0x1B, 0xF9, 0xB7, 0x05, 0x9C, 0xDC, 0x40, 0x74, 0xE6, 0x04, 0x3B, 0x6D, 0x86}

#define DEF_CUST1_INBOUND_UUID_128  {0x25, 0x54, 0xE4, 0x4D, 0x59, 0x14, 0xD1, 0xB7, 0x1D, 0x41, 0xCD, 0xE8, 0xB9, 0x8F, 0x4F, 0x91}
#define DEF_CUST1_OUTBOUND_UUID_128  {0x04, 0xA4, 0x59, 0xDC, 0x06, 0x6B, 0x09, 0xAA, 0xBE, 0x4F, 0xB2, 0x50, 0xAA, 0x35, 0xB5, 0x3B}
#define DEF_CUST1_FLOW_CONTROL_UUID_128  {0x39, 0x56, 0x0D, 0xC1, 0x56, 0x18, 0x25, 0x9F, 0x45, 0x4A, 0xF9, 0xD4, 0x39, 0x8B, 0x04, 0xE2}


#define DEF_CUST1_INBOUND_LEN  160
#define DEF_CUST1_OUTBOUND_LEN  160
#define DEF_CUST1_FLOW_CONTROL_LEN  1



///Attribute Database Index Enumeration
enum
{
	CUST1_IDX_SVC = 0,

	CUST1_IDX_INBOUND_CHAR,
	CUST1_IDX_INBOUND_VAL,

	CUST1_IDX_OUTBOUND_CHAR,
	CUST1_IDX_OUTBOUND_VAL,

	CUST1_IDX_FLOW_CONTROL_CHAR,
	CUST1_IDX_FLOW_CONTROL_VAL,
	CUST1_IDX_FLOW_CONTROL_CFG,

	CUST1_IDX_NB
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern struct attm_desc_128 custs1_att_db[CUST1_IDX_NB];

/// @} USER_CONFIG

#endif // _USER_CUSTS1_DEF_H_

