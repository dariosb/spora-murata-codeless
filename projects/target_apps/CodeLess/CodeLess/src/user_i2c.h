/**
 ****************************************************************************************
 *
 * @file user_i2c.h
 *
 * @brief user_i2c header file.
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

#ifndef _USER_I2C_H_
#define _USER_I2C_H_

#include "app_api.h"

/*---------------------------------------------------------------------------------------
| @brief      Checks that ports have been configured so that there is support for I2C.
|             There must be one I2C SCL and one I2C SDA port defined
|
| @return     True if both I2C SCL and I2C SDA has been configured
---------------------------------------------------------------------------------------*/
bool user_is_i2c_hw_valid(void);
//


/*---------------------------------------------------------------------------------------
| @brief      Initializes the I2C interface with the provide slave address
|
| @param[in]  slave_address, The I2C slave address to communicate with
|
| @return     void
---------------------------------------------------------------------------------------*/
void user_i2c_init(uint8_t slave_address);
//


/*---------------------------------------------------------------------------------------
| @brief      Scans the I2C bus for slave devices
|
| @param[in]  extended, if true: Response will include potential I2C slave device IDs
| @param[in]  remote, originator of the scan command
|
| @return     void
---------------------------------------------------------------------------------------*/
void user_i2c_scan(char* resp_str);
//


/*---------------------------------------------------------------------------------------
| @brief      Reads data from an I2C slave
|
| @param[in]  hw_address, HW address of I2C slave device
| @param[in]  reg_address, Register address to be read
| @param[in]  num_bytes, Number of bytes to read
| @param[in]  remote, originator of the read command
|
| @return     void
---------------------------------------------------------------------------------------*/
void user_i2c_read(uint16_t hw_address,uint16_t reg_address, uint8_t num_bytes, char* resp_str);
//


/*---------------------------------------------------------------------------------------
| @brief      Writes data to a register address of an I2C slave
|
| @param[in]  hw_address, HW address of I2C slave device
| @param[in]  reg_address, Register address to write to
| @param[in]  data, Data to write
| @param[in]  remote, originator of the read command
|
| @return     void
---------------------------------------------------------------------------------------*/
void user_i2c_write(uint16_t hw_address,uint16_t reg_address, uint8_t wr_data);
//


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

#endif //_USER_I2C_H_
/// @} APP

