/**
 ****************************************************************************************
 *
 * @file user_otp_functions.h
 *
 * @brief user_otp_functions header file.
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
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

//    ______               _  ______           _            _____                
//    | ___ \             | | |  _  \         (_)          |_   _|               
//    | |_/ /___  __ _  __| | | | | |_____   ___  ___ ___    | |_   _ _ __   ___ 
//    |    // _ \/ _` |/ _` | | | | / _ \ \ / / |/ __/ _ \   | | | | | '_ \ / _ \
//    | |\ \  __/ (_| | (_| | | |/ /  __/\ V /| | (_|  __/   | | |_| | |_) |  __/
//    \_| \_\___|\__,_|\__,_| |___/ \___| \_/ |_|\___\___|   \_/\__, | .__/ \___|
//                                                               __/ | |         
//                                                              |___/|_|         
// Reads the device type of target SoC
uint32_t user_read_device_type(void);
//


//    ______               _  ____________             _     _                      __                       _____ ___________ 
//    | ___ \             | | | ___ \  _  \           | |   | |                    / _|                     |  _  |_   _| ___ \
//    | |_/ /___  __ _  __| | | |_/ / | | |   __ _  __| | __| |_ __ ___  ___ ___  | |_ _ __ ___  _ __ ___   | | | | | | | |_/ /
//    |    // _ \/ _` |/ _` | | ___ \ | | |  / _` |/ _` |/ _` | '__/ _ \/ __/ __| |  _| '__/ _ \| '_ ` _ \  | | | | | | |  __/ 
//    | |\ \  __/ (_| | (_| | | |_/ / |/ /  | (_| | (_| | (_| | | |  __/\__ \__ \ | | | | | (_) | | | | | | \ \_/ / | | | |    
//    \_| \_\___|\__,_|\__,_| \____/|___/    \__,_|\__,_|\__,_|_|  \___||___/___/ |_| |_|  \___/|_| |_| |_|  \___/  \_/ \_|    
// Reads the Bluetooth Device Address from OTP header        
struct bd_addr user_read_otp_bdaddr(void);
//


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

/// @} APP

