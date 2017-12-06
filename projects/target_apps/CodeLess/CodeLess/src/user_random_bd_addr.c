/**
 ****************************************************************************************
 *
 * @file user_random_bd_addr.c
 *
 * @brief user_random_bd_addr source file.
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
 
 #include "user_random_bd_addr.h"
 
 #include "co_bt.h"
 #include "app_mid.h"
 #include "app_api.h"
 #include "app.h"


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */



/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

//--------------------------------------------------------------------------------------------------------/
//     _   _           _       _____                 _   _             
//    | | | | __ _ ___| |__   |  ___|   _ _ __   ___| |_(_) ___  _ __  
//    | |_| |/ _` / __| '_ \  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \ 
//    |  _  | (_| \__ \ | | | |  _|| |_| | | | | (__| |_| | (_) | | | |
//    |_| |_|\__,_|___/_| |_| |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|
//                                                                     
/**
****************************************************************************************
* @brief A simple hash function. As both '+' and '^' preserve the uniqiness of a number
*        the combined effect of the hash function is simply to shuffle the data around
*        maintaining a one-to-one mapping. The best 'shuffle' is with a 32-bit number
*        but 16/14 bit is also OK.
*
*        Robert Jenkins' 32 bit integer hash function
*        https://gist.github.com/badboy/6267743
*
* @return void.
****************************************************************************************
*/
uint32_t hash( uint32_t a)
{
   a = (a+0x7ed55d16) + (a<<12);
   a = (a^0xc761c23c) ^ (a>>19);
   a = (a+0x165667b1) + (a<<5);
   a = (a+0xd3a2646c) ^ (a<<9);
   a = (a+0xfd7046c5) + (a<<3);
   a = (a^0xb55a4f09) ^ (a>>16);
   return a;
}

//--------------------------------------------------------------------------------------------------------/
//     ____                 _                   ____  ____       _       _     _                   
//    |  _ \ __ _ _ __   __| | ___  _ __ ___   | __ )|  _ \     / \   __| | __| |_ __ ___  ___ ___ 
//    | |_) / _` | '_ \ / _` |/ _ \| '_ ` _ \  |  _ \| | | |   / _ \ / _` |/ _` | '__/ _ \/ __/ __|
//    |  _ < (_| | | | | (_| | (_) | | | | | | | |_) | |_| |  / ___ \ (_| | (_| | | |  __/\__ \__ \
//    |_| \_\__,_|_| |_|\__,_|\___/|_| |_| |_| |____/|____/  /_/   \_\__,_|\__,_|_|  \___||___/___/
//                                                                                                 
// Reads device specific data from OTP in order to generate a unique random BD Address
struct bd_addr generate_random_bd_addr()
{
  uint32_t siteID, timestamp, trim; 
  
	#ifndef CFG_BOOT_FROM_OTP    
    int cnt = 100000;
    #define XPMC_MODE_MREAD   0x1
    SetBits16(CLK_AMBA_REG, OTP_ENABLE, 1);    // Enable OTP clock  
    while ((GetWord16(ANA_STATUS_REG) & LDO_OTP_OK) != LDO_OTP_OK && cnt--)
    // Set OTP in read mode 
    SetWord32(OTPC_MODE_REG, XPMC_MODE_MREAD);
  #endif //CFG_BOOT_FROM_OTP
    
  // Read OTP values
  timestamp = GetWord32(0x00047f64);             // Device time stamp
  siteID = GetWord8(0x00047f6a)&0x7;             // Get the Site ID (3-bits)
  timestamp ^= (siteID << 30);                   // Add test site to MSB
  trim = siteID>>2;                              // MSb of Site ID
  trim |= (GetWord8(0x00047f84)&0x3F)<<1;        // Bandgap trim (6-bits)
  trim |= (GetWord8(0x00047f7c)&0x7F)<<7;        // LNA trim (7-bits)
		
	SetBits16(CLK_AMBA_REG, OTP_ENABLE, 0);     // Disable OTP clock
	
  // Shuffle the bits to appear more random
  timestamp = hash(timestamp);
  trim = (hash(trim) & 0x3FFF) | 0xC000;
  
		
  // Assign unique address 
  uint8_t bd_temp[6];
	bd_temp[0] = timestamp&0xFF;
  bd_temp[1] = (timestamp>>8)&0xFF;
  bd_temp[2] = (timestamp>>16)&0xFF;
  bd_temp[3] = (timestamp>>24)&0xFF;
  bd_temp[4] = trim&0xFF;
  bd_temp[5] = trim>>8;
	
	// Cast to bd_addr struct
  struct bd_addr bdaddr_result;
	memcpy(&bdaddr_result,&bd_temp,6);
	
  return bdaddr_result;
}

//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              


/// @} APP
