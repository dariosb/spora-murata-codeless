/**
 ****************************************************************************************
 *
 * @file user_random_bd_addr.h
 *
 * @brief user_random_bd_addr header file.
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
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */


//--------------------------------------------------------------------------------------------------------/
//     ____                 _                   ____  ____       _       _     _                   
//    |  _ \ __ _ _ __   __| | ___  _ __ ___   | __ )|  _ \     / \   __| | __| |_ __ ___  ___ ___ 
//    | |_) / _` | '_ \ / _` |/ _ \| '_ ` _ \  |  _ \| | | |   / _ \ / _` |/ _` | '__/ _ \/ __/ __|
//    |  _ < (_| | | | | (_| | (_) | | | | | | | |_) | |_| |  / ___ \ (_| | (_| | | |  __/\__ \__ \
//    |_| \_\__,_|_| |_|\__,_|\___/|_| |_| |_| |____/|____/  /_/   \_\__,_|\__,_|_|  \___||___/___/
//                                                                                                 
// Reads device specific data from OTP in order to generate a unique random BD Address
struct bd_addr generate_random_bd_addr(void);
//


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

/// @} APP
