/**
 ****************************************************************************************
 *
 * @file user_cmd_parser.c
 *
 * @brief user_cmd_parser source file.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_cmd_parser.h"

#include "arch_console.h"
#include "uart.h"

#include "user_otp_functions.h"

#include "stdlib.h"


/*
 * DEFINES
 ****************************************************************************************
 */

 
/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

uint8_t rx_buffer[CMD_BUFFER_SIZE]; 
uint8_t command[CMD_MAX_TAG_SIZE];
uint8_t argument_array[CMD_MAX_ARG_COUNT][CMD_MAX_ARG_LENGTH];
uint8_t argument_count = 0;

bool extended_cmd = false;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

//--------------------------------------------------------------------------------------------------------/
//     _____ _                    _____                                           _ 
//    /  __ \ |                  /  __ \                                         | |
//    | /  \/ | ___  __ _ _ __   | /  \/ ___  _ __ ___  _ __ ___   __ _ _ __   __| |
//    | |   | |/ _ \/ _` | '_ \  | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
//    | \__/\ |  __/ (_| | | | | | \__/\ (_) | | | | | | | | | | | (_| | | | | (_| |
//     \____/_|\___|\__,_|_| |_|  \____/\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
//                                                                                  
// Removes back-space and carriage return characters. Zero terminates command string                                                                                  
void clean_command(uint32_t bytes)
{
	// Removes backspace characters and carriage return. Null terminates the command string.
	uint8_t index = 0;
	for(uint8_t i=0;i<bytes;i++)
	{
		if(rx_buffer[i]==0x08)
			index--;
		else
			rx_buffer[index++]=rx_buffer[i];	
	}
	rx_buffer[index-1] = 0x00; // Null terminate the string
}
//


//--------------------------------------------------------------------------------------------------------/
//     _____     _                  _      ___                                       _       
//    |  ___|   | |                | |    / _ \                                     | |      
//    | |____  _| |_ _ __ __ _  ___| |_  / /_\ \_ __ __ _ _   _ _ __ ___   ___ _ __ | |_ ___ 
//    |  __\ \/ / __| '__/ _` |/ __| __| |  _  | '__/ _` | | | | '_ ` _ \ / _ \ '_ \| __/ __|
//    | |___>  <| |_| | | (_| | (__| |_  | | | | | | (_| | |_| | | | | | |  __/ | | | |_\__ \
//    \____/_/\_\\__|_|  \__,_|\___|\__| \_| |_/_|  \__, |\__,_|_| |_| |_|\___|_| |_|\__|___/
//                                                   __/ |                                   
//                                                  |___/                                    
// Populates argument array and extracts the command tag (the part of a command to the left of '=')
void extract_args()
{
	// Initialize the argument array
	memset(argument_array,0,sizeof(argument_array));
	argument_count = 0;
	uint8_t idx = 0;
	
	// The command tag will be the first part of the command
	bool command_tag_state = true;
	
	// Iterate through entire command string in the UART receive buffer
	for(uint8_t i=0;i<160;i++)
	{
		if(command_tag_state)
		{
			if(rx_buffer[i] == '=')
			{
				// Command tag ends here
				command_tag_state = false;
				command[idx]=0x00;
				idx = 0;
			}
			else
				// Record command tag
				command[idx++]=rx_buffer[i];
		}
		else
		{
			if(rx_buffer[i] == 0x00)
			{
				// No more arguments
				argument_count++;
			  break;
			}
			else if(rx_buffer[i] == ',')
			{
				// Next argument
				argument_array[argument_count][idx]=0x00;
				argument_count++;
				idx = 0;
			}
			else
				// Record current argument
				argument_array[argument_count][idx++] = rx_buffer[i];
		}
	}
	// Clear UART receive buffer (preparing for next command
	memset(rx_buffer,0,sizeof(rx_buffer));
}
//


//--------------------------------------------------------------------------------------------------------/
//     _   _       _ _     _       _          ___ _____   _____               _ 
//    | | | |     | (_)   | |     | |        / _ \_   _| /  __ \             | |
//    | | | | __ _| |_  __| | __ _| |_ ___  / /_\ \| |   | /  \/_ __ ___   __| |
//    | | | |/ _` | | |/ _` |/ _` | __/ _ \ |  _  || |   | |   | '_ ` _ \ / _` |
//    \ \_/ / (_| | | | (_| | (_| | ||  __/ | | | || |   | \__/\ | | | | | (_| |
//     \___/ \__,_|_|_|\__,_|\__,_|\__\___| \_| |_/\_/    \____/_| |_| |_|\__,_|
//                                                                              
// Validates the format of the AT command. Returns true if the format is valid.
// Qualifies the command as either a basic AT command or an extended command.
// Determines if command is intended for local device or remote Bluetooth peer.                                                                               
bool validate_at_cmd()
{
  extended_cmd = false;
	bool result = true;
	
	uint8_t temp_command[CMD_MAX_TAG_SIZE];
	// Ensure that end reult will be null terminated string
	memset(&temp_command,0,CMD_MAX_TAG_SIZE);
	
	// Index of resulting string
	uint8_t idx = 0;
	
	// Iterate through the command characters
	for(uint8_t i=0; i<CMD_MAX_TAG_SIZE; i++)
	{
		char c = command[i];
		// First two letters must be 'A' and 'T'.
		if(((i==0)&&(c!='A')) || ((i==1)&&(c!='T')))
		{
			result = false;
			break;
		}
		if((i==2)||(i==3))
		{
			// Extended AT command?
			if(c == '+')
				extended_cmd=true;
			else if(c != 'r')
				// Record anything else
				temp_command[idx++] = (uint8_t)c;
		}
		if(i>3)
			// Record all other characters
			temp_command[idx++] = (uint8_t)c;
	}
	
	// Empty string is a valid AT command and is identical to 'AT<CR>' 
	if(command[0]==0x00)
		result = true;
		
	// Replace command with validated command tag (ATr+ was removed)
	memcpy(&command,&temp_command,CMD_MAX_TAG_SIZE);
	
	return result;
}
//


//     _   _             _          _____      _                       
//    | | | |           | |        |_   _|    | |                      
//    | |_| | _____  __ | |_ ___     | | _ __ | |_ ___  __ _  ___ _ __ 
//    |  _  |/ _ \ \/ / | __/ _ \    | || '_ \| __/ _ \/ _` |/ _ \ '__|
//    | | | |  __/>  <  | || (_) |  _| || | | | ||  __/ (_| |  __/ |   
//    \_| |_/\___/_/\_\  \__\___/   \___/_| |_|\__\___|\__, |\___|_|   
//                                                      __/ |          
//                                                     |___/           
//  Converts hexadecimal string to integer  
uint16_t ahtoi(char* str)
{
	int16_t result;
	if ((strlen(str)>2) && (str[0]=='0') && (str[1] == 'x')) 
	  // Argument prepended with 0x to indicate hexadecimal
	  result = (uint16_t)strtol(str, NULL, 0);
	else 
		// Argument provided as decimal
		result = atoi(str);
	return result;
}
//


//     _____ _        _               _         ____________    ___      _     _                   
//    /  ___| |      (_)             | |        | ___ \  _  \  / _ \    | |   | |                  
//    \ `--.| |_ _ __ _ _ __   __ _  | |_ ___   | |_/ / | | | / /_\ \ __| | __| |_ __ ___  ___ ___ 
//     `--. \ __| '__| | '_ \ / _` | | __/ _ \  | ___ \ | | | |  _  |/ _` |/ _` | '__/ _ \/ __/ __|
//    /\__/ / |_| |  | | | | | (_| | | || (_) | | |_/ / |/ /  | | | | (_| | (_| | | |  __/\__ \__ \
//    \____/ \__|_|  |_|_| |_|\__, |  \__\___/  \____/|___/   \_| |_/\__,_|\__,_|_|  \___||___/___/
//                             __/ |                                                               
//                            |___/                                                                
//  Converts from a string to a Bluetooth Device Address
struct bd_addr strtobdaddr(char* str)
{
	struct bd_addr bdaddr;
	char c[6];
  char temp_str[3] = {0};
	for(uint8_t i = 0; i < 6; i++)
	{
		temp_str[0] = str[15 - i * 3];
		temp_str[1] = str[16 - i * 3];
		c[i] = (uint8_t)strtol(temp_str, NULL, 16);
	}
	memcpy(&bdaddr,&c,6);
	return bdaddr;
}	
//


//     _   _       _ _     _       _          ___      _        ______      _        
//    | | | |     | (_)   | |     | |        / _ \    | |       |  _  \    | |       
//    | | | | __ _| |_  __| | __ _| |_ ___  / /_\ \ __| |_   __ | | | |__ _| |_ __ _ 
//    | | | |/ _` | | |/ _` |/ _` | __/ _ \ |  _  |/ _` \ \ / / | | | / _` | __/ _` |
//    \ \_/ / (_| | | | (_| | (_| | ||  __/ | | | | (_| |\ V /  | |/ / (_| | || (_| |
//     \___/ \__,_|_|_|\__,_|\__,_|\__\___| \_| |_/\__,_| \_(_) |___/ \__,_|\__\__,_|
//                                                                                   
// Validates a provided advertisement string
bool validate_adv_data(uint8_t* str, uint8_t* result, uint8_t* length)
{
	uint8_t len = strlen((char*)str)/3 + 1;
	memcpy(length,&len,1);
	
  // Convert from Ascii hex to bytes
  char temp_str[3] = {0};
	for(uint8_t i = 0; i < len; i++)
	{
		temp_str[0] = str[i * 3];
		temp_str[1] = str[i * 3 + 1];
		result[i] = (uint8_t)strtol(temp_str, NULL, 16);
	}
	
	// Validate that string adheres to |Length|Type|Data| format 
  uint8_t ptr = 0;
	uint8_t lc = 0;
	while(ptr < len)
	{
		lc = result[ptr];
		ptr = ptr + lc + 1;
	}
	if( ptr == len) 
		return true;
	else
		return false;
}


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

/// @} APP
