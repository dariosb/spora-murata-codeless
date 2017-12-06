/**
 ****************************************************************************************
 *
 * @file user_cmd_interpreter.h
 *
 * @brief user_cmd_interpreter header file.
 *
 * Copyright (C) 2017 Dialog Semiconductor. This computer program includes Confidential, 
 * Proprietary Information of Dialog Semiconductor. All Rights Reserved. This software 
 * is solely for use on authorized Dialog Semiconductor products and platforms. 
 * Recipient shall not transmit any software source code to any third party without
 * Dialog Semiconductor's prior written permission. 
 * 
 * TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT 
 * WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 *
 ****************************************************************************************
 */

#ifndef _USER_CMD_INTERPRETER_H_
#define _USER_CMD_INTERPRETER_H_
/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

// This enumeration type is used to keep track of current device Bluetooth activity state
typedef enum
{
	IDLE,
	ADVERTISING,
	SCANNING,
	DISCOVERING,
	INITIATING_CONNECTION,
	DISCONNECTING,
	CHANGING_ROLE,
} GAP_ACTIVITY_t;


// This enumeration type is used to keep track of where an AT command originated and where the response is to be generated
typedef enum 
{
	CMD_ROUTE_UNDEFINED,
	CMD_FROM_LOCAL_RESP_LOCAL,
	CMD_FROM_LOCAL_RESP_PEER,
	CMD_FROM_PEER_RESP_PEER,
} CMD_DIRECTION_t; 

// IO functionality
typedef enum 
{
	IO_FUNC_UNDEFINED,							// 0
	
	IO_FUNC_INPUT,									// 1
	IO_FUNC_INPUT_PULLUP,						// 2
	IO_FUNC_INPUT_PULLDOWN,					// 3
	
	IO_FUNC_OUTPUT,									// 4
	
	IO_FUNC_ANALOG_INPUT,						// 5
	IO_FUNC_ANALOG_INPUT_ATT3,			// 6
	
	IO_FUNC_I2C_CLOCK,							// 7
	IO_FUNC_I2C_DATA,								// 8

	IO_FUNC_CONN_IND_HI,						// 9
  IO_FUNC_CONN_IND_LO,						// 10
  
	
	// The settings below are not supported in this revision
	
	IO_FUNC_UART1_TX,								// 11
	IO_FUNC_UART1_RX,								// 12
	IO_FUNC_UART1_CTS,							// 13
	IO_FUNC_UART1_RTS,							// 14
	
	IO_FUNC_UART2_TX,								// 15
	IO_FUNC_UART2_RX,								// 16
	IO_FUNC_UART2_CTS,							// 17
	IO_FUNC_UART2_RTS,							// 18
	
	IO_FUNC_SPI_CLK,								// 19
	IO_FUNC_SPI_CS,									// 20
	IO_FUNC_SPI_MOSI,								// 21
	IO_FUNC_SPI_MISO,								// 22
	
	IO_FUNC_PWM1,										// 23
	IO_FUNC_PWM2,										// 24	
	IO_FUNC_PWM3,										// 25
	IO_FUNC_PWM4,										// 26

	IO_FUNC_NOT_AVAILABLE						// 27
	
} IO_FUNC_t; 


// Struct holding the CodeLess environment
struct codeless_env_t
{
	// Current command
	char current_cmd[160];
	
	// CodeLess UUIDs
	uint8_t service_uuid[16];
	uint8_t char_uuid_inbound[16];
	uint8_t char_uuid_outbound[16];
	uint8_t char_uuid_flow_ctrl[16];
	
	// Peer GATT database handles 
	uint16_t hdl_inbound_val;
	uint16_t hdl_outbound_val; 
	uint16_t hdl_flow_ctrl;
	
	// Device Bluetooth address
	struct bd_addr device_address;
	
	// IO Port arrays
	IO_FUNC_t port_configuration[40];
	bool port_io_state[40];
	
	// Timer handles
	timer_hnd tmr[4];
	
	// Adverising interval
	uint16_t adv_intv;
	
	// Advertising data
	uint8_t adv_data[28];
	uint8_t adv_data_len;
	
	// Scan response data
	uint8_t resp_data[28];
	uint8_t resp_data_len;
	
	bool echo;
	bool suppress_response;
	uint8_t bt_role;
	bool linked_to_codeless_device;
	CMD_DIRECTION_t command_route;
	GAP_ACTIVITY_t gap_activity;
	
};



/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

extern struct codeless_env_t codeless_env;


/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

//--------------------------------------------------------------------------------------------------------/
//      ___ _____   _____                                           _   _____      _                           _            
//     / _ \_   _| /  __ \                                         | | |_   _|    | |                         | |           
//    / /_\ \| |   | /  \/ ___  _ __ ___  _ __ ___   __ _ _ __   __| |   | | _ __ | |_ ___ _ __ _ __  _ __ ___| |_ ___ _ __ 
//    |  _  || |   | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |   | || '_ \| __/ _ \ '__| '_ \| '__/ _ \ __/ _ \ '__|
//    | | | || |   | \__/\ (_) | | | | | | | | | | | (_| | | | | (_| |  _| || | | | ||  __/ |  | |_) | | |  __/ ||  __/ |   
//    \_| |_/\_/    \____/\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|  \___/_| |_|\__\___|_|  | .__/|_|  \___|\__\___|_|   
//                                                                                             | |                          
//                                                                                             |_|                          
// Interprets and handles individual AT commands
void user_cmd_interpreter(uint32_t bytes);
//

//--------------------------------------------------------------------------------------------------------/
//    ______           _       
//    | ___ \         | |      
//    | |_/ /___ _ __ | |_   _ 
//    |    // _ \ '_ \| | | | |
//    | |\ \  __/ |_) | | |_| |
//    \_| \_\___| .__/|_|\__, |
//              | |       __/ |
//              |_|      |___/ 
// Replies back to the originator of a command
void user_reply(char* reply_string, bool success);
//	

//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

#endif //_USER_CMD_INTERPRETER_H_
/// @} APP

