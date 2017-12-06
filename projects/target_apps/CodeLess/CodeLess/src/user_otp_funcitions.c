/**
 ****************************************************************************************
 *
 * @file user_otp_functions.c
 *
 * @brief user_otp_functions source file.
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

#include "user_otp_functions.h"

#include "arch_console.h"
#include "uart.h"


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

//--------------------------------------------------------------------------------------------------------/
//     _____ ___________  ______               _    ___       _   _            _       
//    |  _  |_   _| ___ \ | ___ \             | |  / _ \     | | (_)          | |      
//    | | | | | | | |_/ / | |_/ /___  __ _  __| | / /_\ \ ___| |_ ___   ____ _| |_ ___ 
//    | | | | | | |  __/  |    // _ \/ _` |/ _` | |  _  |/ __| __| \ \ / / _` | __/ _ \
//    \ \_/ / | | | |     | |\ \  __/ (_| | (_| | | | | | (__| |_| |\ V / (_| | ||  __/
//     \___/  \_/ \_|     \_| \_\___|\__,_|\__,_| \_| |_/\___|\__|_| \_/ \__,_|\__\___|
//                                                                                     
// Activate OTP for read operation                                                                                     
void user_otp_read_activate(void)
{
	int cnt = 100000;
  SetBits16(CLK_AMBA_REG, OTP_ENABLE, 1);    // Enable OTP clock  
  while ((GetWord16(ANA_STATUS_REG) & LDO_OTP_OK) != LDO_OTP_OK && cnt--)
  
	// set OTP in read mode 
  SetWord32(OTPC_MODE_REG, 0x1);
}
//


//--------------------------------------------------------------------------------------------------------/
//     _____ ___________  ______               _  ______                _   _            _       
//    |  _  |_   _| ___ \ | ___ \             | | |  _  \              | | (_)          | |      
//    | | | | | | | |_/ / | |_/ /___  __ _  __| | | | | |___  __ _  ___| |_ ___   ____ _| |_ ___ 
//    | | | | | | |  __/  |    // _ \/ _` |/ _` | | | | / _ \/ _` |/ __| __| \ \ / / _` | __/ _ \
//    \ \_/ / | | | |     | |\ \  __/ (_| | (_| | | |/ /  __/ (_| | (__| |_| |\ V / (_| | ||  __/
//     \___/  \_/ \_|     \_| \_\___|\__,_|\__,_| |___/ \___|\__,_|\___|\__|_| \_/ \__,_|\__\___|
//                                                                                               
// Deactivate OTP
void user_otp_read_deactivate(void)
{
	SetBits16(CLK_AMBA_REG, OTP_ENABLE, 0);     // Disable OTP clock
}
//


//--------------------------------------------------------------------------------------------------------/
//    ______               _  ______           _            _____                
//    | ___ \             | | |  _  \         (_)          |_   _|               
//    | |_/ /___  __ _  __| | | | | |_____   ___  ___ ___    | |_   _ _ __   ___ 
//    |    // _ \/ _` |/ _` | | | | / _ \ \ / / |/ __/ _ \   | | | | | '_ \ / _ \
//    | |\ \  __/ (_| | (_| | | |/ /  __/\ V /| | (_|  __/   | | |_| | |_) |  __/
//    \_| \_\___|\__,_|\__,_| |___/ \___| \_/ |_|\___\___|   \_/\__, | .__/ \___|
//                                                               __/ | |         
//                                                              |___/|_|         
// Read the SoC device type from OTP
uint32_t user_read_device_type()
{
	user_otp_read_activate();
	
	uint32_t data = GetWord32( 0x47F70);        // Device type
	
	user_otp_read_deactivate();
	
	return data;
}
//


//--------------------------------------------------------------------------------------------------------/
//    ______               _  ____________    ___      _     _                      __                       _____ ___________ 
//    | ___ \             | | | ___ \  _  \  / _ \    | |   | |                    / _|                     |  _  |_   _| ___ \
//    | |_/ /___  __ _  __| | | |_/ / | | | / /_\ \ __| | __| |_ __ ___  ___ ___  | |_ _ __ ___  _ __ ___   | | | | | | | |_/ /
//    |    // _ \/ _` |/ _` | | ___ \ | | | |  _  |/ _` |/ _` | '__/ _ \/ __/ __| |  _| '__/ _ \| '_ ` _ \  | | | | | | |  __/ 
//    | |\ \  __/ (_| | (_| | | |_/ / |/ /  | | | | (_| | (_| | | |  __/\__ \__ \ | | | | | (_) | | | | | | \ \_/ / | | | |    
//    \_| \_\___|\__,_|\__,_| \____/|___/   \_| |_/\__,_|\__,_|_|  \___||___/___/ |_| |_|  \___/|_| |_| |_|  \___/  \_/ \_|    
//                                                                                                                             
// Read BD address from OTP                                                                                                                             
struct bd_addr user_read_otp_bdaddr()
{
  struct bd_addr address;

	user_otp_read_activate();
		
	uint8_t *otp_bdaddr = (uint8_t *)0x47FD4;		// BD Address
	
	memcpy(&address, otp_bdaddr, 6);
	
	user_otp_read_deactivate();
	
	return address;
}
//


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

/// @} APP
