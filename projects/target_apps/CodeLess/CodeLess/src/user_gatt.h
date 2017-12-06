/**
 ****************************************************************************************
 *
 * @file user_gatt.h
 *
 * @brief user_gatt header file.
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

#ifndef _USER_GATT_H_
#define _USER_GATT_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

//     _____                _   _                               
//    /  ___|              | | | |                              
//    \ `--.  ___ _ __   __| | | |_ ___    _ __   ___  ___ _ __ 
//     `--. \/ _ \ '_ \ / _` | | __/ _ \  | '_ \ / _ \/ _ \ '__|
//    /\__/ /  __/ | | | (_| | | || (_) | | |_) |  __/  __/ |   
//    \____/ \___|_| |_|\__,_|  \__\___/  | .__/ \___|\___|_|   
//                                        | |                   
//                                        |_|                   
// Sends a command or response to a Bluetooth peer   
void send_to_peer(char* str_to_send);
//


//    ______ _                               _____ _                          _            _     _   _          
//    |  _  (_)                             /  __ \ |                        | |          (_)   | | (_)         
//    | | | |_ ___  ___ _____   _____ _ __  | /  \/ |__   __ _ _ __ __ _  ___| |_ ___ _ __ _ ___| |_ _  ___ ___ 
//    | | | | / __|/ __/ _ \ \ / / _ \ '__| | |   | '_ \ / _` | '__/ _` |/ __| __/ _ \ '__| / __| __| |/ __/ __|
//    | |/ /| \__ \ (_| (_) \ V /  __/ |    | \__/\ | | | (_| | | | (_| | (__| ||  __/ |  | \__ \ |_| | (__\__ \
//    |___/ |_|___/\___\___/ \_/ \___|_|     \____/_| |_|\__,_|_|  \__,_|\___|\__\___|_|  |_|___/\__|_|\___|___/
// Performs a GATT characteristics discovery   
void user_gatt_discover_chars(void);
//


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

#endif //_USER_GATT_H_
/// @} APP

