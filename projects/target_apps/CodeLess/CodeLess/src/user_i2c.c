/**
 ****************************************************************************************
 *
 * @file user_i2c.c
 *
 * @brief user_i2c source file.
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

 #include "user_i2c.h"
 
 #include "user_cmd_interpreter.h"

 

/****************************************************************************************
* MACROS
****************************************************************************************/
// I2C helper macros
#define SEND_I2C_COMMAND(X)             SetWord16(I2C_DATA_CMD_REG, (X))
#define WAIT_WHILE_I2C_FIFO_IS_FULL()   while(!(GetWord16(I2C_STATUS_REG) & TFNF))
#define WAIT_UNTIL_I2C_FIFO_IS_EMPTY()  while(!(GetWord16(I2C_STATUS_REG) & TFE))
#define WAIT_UNTIL_NO_MASTER_ACTIVITY() while(GetWord16(I2C_STATUS_REG) & MST_ACTIVITY)
#define WAIT_FOR_RECEIVED_BYTE()        while(!GetWord16(I2C_RXFLR_REG))


/*---------------------------------------------------------------------------------------
| @brief      Checks that ports have been configured so that there is support for I2C.
|             There must be one I2C SCL and one I2C SDA port defined
|
| @return     True if both I2C SCL and I2C SDA has been configured
---------------------------------------------------------------------------------------*/
bool user_is_i2c_hw_valid()
{
  // Default to false
  bool status_scl = false;  
  bool status_sda = false;
  // Run through all IO pins in the pin configuration array
  for(uint8_t i = 0; i<40 ; i++)
  {
    // Is there a pin configured for I2C Clock?
    if(codeless_env.port_configuration[i] == IO_FUNC_I2C_CLOCK)
      status_scl = true;
    // Is there a pin configured for I2C Data?
    if(codeless_env.port_configuration[i] == IO_FUNC_I2C_DATA)
      status_sda = true;
  }
  // Return true if a pin was defined as I2C Clock AND a pin is set to I2C Data
  return (status_scl & status_sda);
}
//


/*---------------------------------------------------------------------------------------
| @brief      Initializes the I2C interface with the provide slave address
|
| @param[in]  slave_address, The I2C slave address to communicate with
|
| @return     void
---------------------------------------------------------------------------------------*/
void user_i2c_init(uint8_t slave_address)
{
  // Initialize the I2C with the address provided as argument
  // TODO: Support 10bit addressing
  SetBits16(CLK_PER_REG, I2C_ENABLE, 1);                                       // Enable  clock for I2C 
  SetWord16(I2C_ENABLE_REG, 0x0);                                              // Disable the I2C controller  
  SetWord16(I2C_CON_REG, I2C_MASTER_MODE | I2C_SLAVE_DISABLE |I2C_RESTART_EN); // Slave is disabled
  SetBits16(I2C_CON_REG, I2C_SPEED, 1);                                        // Set speed. Standard speed = 1, fast = 2
  SetBits16(I2C_CON_REG, I2C_10BITADDR_MASTER, 0);                             // Set addressing mode. 7bit = 0, 10bit= 1
  SetWord16(I2C_TAR_REG, slave_address & 0xFF);                                // Set Slave device address
  SetWord16(I2C_ENABLE_REG, 0x1);                                              // Enable the I2C controller
  while(GetWord16(I2C_STATUS_REG) & 0x20);                                     // Wait for I2C master FSM to be IDLE
}  
//


/*---------------------------------------------------------------------------------------
| @brief      Scans the I2C bus for slave devices
|
| @param[in]  extended, if true: Response will include potential I2C slave device IDs
| @param[in]  remote, originator of the scan command
|
| @return     void
---------------------------------------------------------------------------------------*/
void user_i2c_scan(char* resp_str)
{
  
  // Run through all I2C addresses (7bit addressing only!)  
  for(uint8_t i = 0x01; i < 0x7F; i++)
  {
    // Set next slave address
    user_i2c_init(i);
    
    SEND_I2C_COMMAND(0x00 & 0x3FF);                 // Transmit Address on bus
    WAIT_WHILE_I2C_FIFO_IS_FULL();                  // Wait if I2C Tx FIFO is full
    SEND_I2C_COMMAND(0x0100 & 0x3FF);               // Send read register 0x00
    WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
    WAIT_UNTIL_NO_MASTER_ACTIVITY();                // Make sure Master has finished
    
    // Did we receive any data?
    if(GetWord16(I2C_RXFLR_REG) != 0)
    {
      // Read the received data (device_id)
      uint8_t device_id =0xFF & GetWord16(I2C_DATA_CMD_REG);
      // Report address and register 0x00 (device_id)
      sprintf(resp_str,"%s0x%02X:0x%02X,",resp_str,i,device_id);
    }
  }
  
	// Disable I2C
  SetWord16(I2C_ENABLE_REG, 0x0);         // Disable the I2C controller  
  SetBits16(CLK_PER_REG, I2C_ENABLE, 0);  // Disable clock for I2C
 
}
//


/*---------------------------------------------------------------------------------------
| @brief Send I2C slave memory address
|
| @param[in] address of the I2C slave memory
---------------------------------------------------------------------------------------*/
void user_i2c_send_address(uint8_t address_to_send)
{   
    SEND_I2C_COMMAND(address_to_send & 0xFF);                   // Set address LSB, write access    
}
//


/*---------------------------------------------------------------------------------------
| @brief      Reads single byte from I2C slave.
|
| @param[in]  address, Memory address to read the byte from.
|
| @return     Read byte.
---------------------------------------------------------------------------------------*/
int8_t user_i2c_read_byte(uint8_t address)
{
    user_i2c_send_address(address & 0x3FF);  
    WAIT_WHILE_I2C_FIFO_IS_FULL();                  // Wait if Tx FIFO is full
    SEND_I2C_COMMAND(0x0100 & 0x3FF);               // Set R/W bit to 1 (read access)
    WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until I2C Tx FIFO empty
	  WAIT_UNTIL_NO_MASTER_ACTIVITY();                // Make sure master has finished  
	  return (0xFF & GetWord16(I2C_DATA_CMD_REG));    // Get received byte
}
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
void user_i2c_read(uint16_t hw_address,uint16_t reg_address, uint8_t num_bytes, char* resp_str)
{
	user_i2c_init(hw_address);
	for(uint8_t i=0;i<num_bytes;i++)
	{
	  uint8_t data = user_i2c_read_byte(reg_address+i);
	 
		if(i==0)
			sprintf(resp_str,"0x%02X",data);
		else
			sprintf(resp_str,"%s,0x%02X",resp_str,data);
	}
}
//


/*---------------------------------------------------------------------------------------
| @brief      Writes single byte to I2C slave.
|
| @param[in]  address, Memory address to write the byte to.
| @param[in]  wr_data, Data to write.
|
| @return     void.
---------------------------------------------------------------------------------------*/
void user_i2c_write_byte(uint16_t address, uint8_t wr_data)
{
  user_i2c_send_address(address);       
  WAIT_WHILE_I2C_FIFO_IS_FULL();                  // Wait if I2C Tx FIFO is full
  SEND_I2C_COMMAND(wr_data & 0xFF);               // Send write data
  WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty    
  WAIT_UNTIL_NO_MASTER_ACTIVITY();                // wait until no master activity 
}
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
void user_i2c_write(uint16_t hw_address,uint16_t reg_address, uint8_t wr_data)
{
	user_i2c_init(hw_address);
	user_i2c_write_byte(reg_address,wr_data);
}
//



//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

/// @} APP
