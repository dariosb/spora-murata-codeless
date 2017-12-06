/**
 ****************************************************************************************
 *
 * @file user_cmd_parser.h
 *
 * @brief user_cmd_parser header file.
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

#include "app_api.h"

/*
 * DEFINITIONS
 ****************************************************************************************
 */

#define CMD_BUFFER_SIZE 		160
#define CMD_MAX_ARG_COUNT 		5
#define CMD_MAX_ARG_LENGTH 	100
#define CMD_MAX_TAG_SIZE 	   16

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

extern uint8_t rx_buffer[CMD_BUFFER_SIZE]; 
extern uint8_t command[CMD_MAX_TAG_SIZE];
extern uint8_t argument_array[CMD_MAX_ARG_COUNT][CMD_MAX_ARG_LENGTH];
extern uint8_t argument_count;
extern bool extended_cmd;


#ifndef _USER_CMD_PARSER_H_
#define _USER_CMD_PARSER_H_


/*
 * FUNCTION DECLARATIONS
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
void clean_command(uint32_t bytes);
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
// Populates argument array and extracts the command tag
void extract_args(void);
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
bool validate_at_cmd(void);
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
uint16_t ahtoi(char* str);
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
struct bd_addr strtobdaddr(char* str);
//


bool validate_adv_data(uint8_t* str, uint8_t* result, uint8_t* len);


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

#endif //_USER_CMD_PARSER_H_

/// @} APP

