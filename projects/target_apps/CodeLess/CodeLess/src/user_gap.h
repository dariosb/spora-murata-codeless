/**
 ****************************************************************************************
 *
 * @file user_gap.h
 *
 * @brief user_gap header file.
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

#ifndef _USER_GAP_H_
#define _USER_GAP_H_


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

//     _____ _                             ______ _____  ______      _      
//    /  __ \ |                            | ___ \_   _| | ___ \    | |     
//    | /  \/ |__   __ _ _ __   __ _  ___  | |_/ / | |   | |_/ /___ | | ___ 
//    | |   | '_ \ / _` | '_ \ / _` |/ _ \ | ___ \ | |   |    // _ \| |/ _ \
//    | \__/\ | | | (_| | | | | (_| |  __/ | |_/ / | |   | |\ \ (_) | |  __/
//     \____/_| |_|\__,_|_| |_|\__, |\___| \____/  \_/   \_| \_\___/|_|\___|
//                              __/ |                                       
//                             |___/                                        
// Changes the Bluetooth GAP role
void user_app_configuration_func(uint8_t role);
//


//     _____     _        _     _ _     _        ____                            _   _             
//    | ____|___| |_ __ _| |__ | (_)___| |__    / ___|___  _ __  _ __   ___  ___| |_(_) ___  _ __  
//    |  _| / __| __/ _` | '_ \| | / __| '_ \  | |   / _ \| '_ \| '_ \ / _ \/ __| __| |/ _ \| '_ \ 
//    | |___\__ \ || (_| | |_) | | \__ \ | | | | |__| (_) | | | | | | |  __/ (__| |_| | (_) | | | |
//    |_____|___/\__\__,_|_.__/|_|_|___/_| |_|  \____\___/|_| |_|_| |_|\___|\___|\__|_|\___/|_| |_|
//                                                                                                 
// We use this function to initiate a connection with a peripheral
void user_connect_to_peripheral(uint8_t peer_addr_type, uint8_t* addr, uint16_t intv);
//


//     ____  _             _     ____                  
//    / ___|| |_ __ _ _ __| |_  / ___|  ___ __ _ _ __  
//    \___ \| __/ _` | '__| __| \___ \ / __/ _` | '_ \ 
//     ___) | || (_| | |  | |_   ___) | (_| (_| | | | |
//    |____/ \__\__,_|_|   \__| |____/ \___\__,_|_| |_|
// Start a Bluetooth device scan                                                    
void user_scan_start(void);
//


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

#endif //_USER_GAP_H_
/// @} APP

