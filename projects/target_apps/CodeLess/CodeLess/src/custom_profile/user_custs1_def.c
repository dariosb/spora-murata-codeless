/**
 ****************************************************************************************
 *
 * @file user_custs1_def.c
 *
 * @brief Custom1 Server (CUSTS1) profile database definitions.
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

/**
 ****************************************************************************************
 * @defgroup USER_CONFIG
 * @ingroup USER
 * @brief Custom1 Server (CUSTS1) profile database definitions.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "prf_types.h"
#include "attm_db_128.h"
#include "user_custs1_def.h"

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Full CUSTOM1 Database Description - Used to add attributes into the database

static const att_svc_desc128_t custs1_svc                        = DEF_CUST1_SVC_UUID_128;

static uint8_t CUST1_INBOUND_UUID_128[ATT_UUID_128_LEN]     	= DEF_CUST1_INBOUND_UUID_128;
static uint8_t CUST1_OUTBOUND_UUID_128[ATT_UUID_128_LEN]     	= DEF_CUST1_OUTBOUND_UUID_128;
static uint8_t CUST1_FLOW_CONTROL_UUID_128[ATT_UUID_128_LEN]     	= DEF_CUST1_FLOW_CONTROL_UUID_128;

static const struct att_char128_desc custs1_inbound_char =
{
	ATT_CHAR_PROP_RD | ATT_CHAR_PROP_WR,
	{0, 0},
	DEF_CUST1_INBOUND_UUID_128
};

static const struct att_char128_desc custs1_outbound_char =
{
	ATT_CHAR_PROP_RD | ATT_CHAR_PROP_WR,
	{0, 0},
	DEF_CUST1_OUTBOUND_UUID_128
};

static const struct att_char128_desc custs1_flow_control_char =
{
	ATT_CHAR_PROP_RD | ATT_CHAR_PROP_WR | ATT_CHAR_PROP_NTF,
	{0, 0},
	DEF_CUST1_FLOW_CONTROL_UUID_128
};

/*
 * CUSTS1 ATTRIBUTES
 ****************************************************************************************
 */

static const uint16_t att_decl_svc       = ATT_DECL_PRIMARY_SERVICE;
static const uint16_t att_decl_char      = ATT_DECL_CHARACTERISTIC;
static const uint16_t att_decl_cfg       = ATT_DESC_CLIENT_CHAR_CFG;

/// Full CUSTOM1 Database Description - Used to add attributes into the database
struct attm_desc_128 custs1_att_db[CUST1_IDX_NB] =
{

	// -----------  Attributes of Custom Service #1 -----------//
	[CUST1_IDX_SVC] =
	{
		(uint8_t*)&att_decl_svc,
		ATT_UUID_16_LEN,
		PERM(RD, ENABLE),
		sizeof(custs1_svc),
		sizeof(custs1_svc),
		(uint8_t*)&custs1_svc
	},

	// Inbound Characteristic Attribute
	[CUST1_IDX_INBOUND_CHAR] =
	{
		(uint8_t*)&att_decl_char,
		ATT_UUID_16_LEN,
		PERM(RD, ENABLE),
		sizeof(custs1_inbound_char),
		sizeof(custs1_inbound_char),
		(uint8_t*)&custs1_inbound_char
	},

	// Inbound Characteristic Value Attribute
	[CUST1_IDX_INBOUND_VAL] =
	{
		CUST1_INBOUND_UUID_128,
		ATT_UUID_128_LEN,
		PERM(WR,ENABLE) | PERM(RD,ENABLE),
		DEF_CUST1_INBOUND_LEN,
		0,
		NULL
	},

	// Outbound Characteristic Attribute
	[CUST1_IDX_OUTBOUND_CHAR] =
	{
		(uint8_t*)&att_decl_char,
		ATT_UUID_16_LEN,
		PERM(RD, ENABLE),
		sizeof(custs1_outbound_char),
		sizeof(custs1_outbound_char),
		(uint8_t*)&custs1_outbound_char
	},

	// Outbound Characteristic Value Attribute
	[CUST1_IDX_OUTBOUND_VAL] =
	{
		CUST1_OUTBOUND_UUID_128,
		ATT_UUID_128_LEN,
		PERM(WR,ENABLE) | PERM(RD,ENABLE),
		DEF_CUST1_OUTBOUND_LEN,
		0,
		NULL
	},

	// Flow Control Characteristic Attribute
	[CUST1_IDX_FLOW_CONTROL_CHAR] =
	{
		(uint8_t*)&att_decl_char,
		ATT_UUID_16_LEN,
		PERM(RD, ENABLE),
		sizeof(custs1_flow_control_char),
		sizeof(custs1_flow_control_char),
		(uint8_t*)&custs1_flow_control_char
	},

	// Flow Control Characteristic Value Attribute
	[CUST1_IDX_FLOW_CONTROL_VAL] =
	{
		CUST1_FLOW_CONTROL_UUID_128,
		ATT_UUID_128_LEN,
		PERM(WR,ENABLE) | PERM(RD,ENABLE) | PERM(NTF, ENABLE),
		DEF_CUST1_FLOW_CONTROL_LEN,
		0,
		NULL
	},

	// Flow Control Characteristic Client Configuration Attribute
	[CUST1_IDX_FLOW_CONTROL_CFG] =
	{
		(uint8_t*)&att_decl_cfg,
		ATT_UUID_16_LEN,
		PERM(RD, ENABLE) | PERM(WR, ENABLE),
		sizeof(uint16_t),
		0,
		NULL
	},

};

/// @} USER_CONFIG
