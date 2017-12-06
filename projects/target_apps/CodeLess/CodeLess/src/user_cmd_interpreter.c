/**
 ****************************************************************************************
 *
 * @file user_cmd_interpreter.c
 *
 * @brief user_cmd_interpreter source file.
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

#include "user_cmd_interpreter.h"

#include "arch_console.h"
#include "uart.h"
#include "gpio.h"
#include "adc.h"
#include "battery.h"

#include "user_custs1_def.h"
#include "custs1_task.h"
#include "user_custs_config.h"

#include "stdlib.h"

#include "user_cmd_parser.h"
#include "user_otp_functions.h"
#include "user_i2c.h"
#include "user_gap.h"
#include "user_gatt.h"
#include "user_periph_setup.h"


/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
 
// The global environment for CodeLess
struct codeless_env_t codeless_env __attribute__((section("retention_mem_area0"),zero_init)); //@RETENTION MEMORY


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

// Functions declared in 'user_codeless.c'
extern void user_advertise(void);
extern void user_codeless_init_system(void);
extern void user_prepare_sleep(void);
extern void user_execute_commands(char* cmd);


char memcmd[4][100];
extern int user_uart_cmd_received_callback(uint8_t status, uint32_t bytes);

//--------------------------------------------------------------------------------------------------------/
//     _____                                        _____                                           _ 
//    /  __ \                                      /  __ \                                         | |
//    | /  \/ ___  _ __ ___  _ __   __ _ _ __ ___  | /  \/ ___  _ __ ___  _ __ ___   __ _ _ __   __| |
//    | |    / _ \| '_ ` _ \| '_ \ / _` | '__/ _ \ | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
//    | \__/\ (_) | | | | | | |_) | (_| | | |  __/ | \__/\ (_) | | | | | | | | | | | (_| | | | | (_| |
//     \____/\___/|_| |_| |_| .__/ \__,_|_|  \___|  \____/\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
//                          | |                                                                       
//                          |_|                                                                       
// Returns true if the received command matches cmd_tag and if the number of arguments is between min_args and max_args
bool user_compare_cmd(char* cmd_tag, uint8_t min_args, uint8_t max_args)
{
	bool result = false;
	if(strcmp((const char*)command,cmd_tag) == 0)
		if((argument_count >= min_args)&&( argument_count <= max_args))
			result = true;
	return result;
}
//


//--------------------------------------------------------------------------------------------------------/
//    ______           _       
//    | ___ \         | |      
//    | |_/ /___ _ __ | |_   _ 
//    |    // _ \ '_ \| | | | |
//    | |\ \  __/ |_) | | |_| |
//    \_| \_\___| .__/|_|\__, |
//              | |       __/ |
//              |_|      |___/ 
// Replies back to the originator of a command (Appends 'OK' if success is true; reports 'ERROR' if false)
void user_reply(char* reply_string, bool success)
{
	if(!codeless_env.suppress_response)
	{
	if(success)
	{
		// Append 'OK' to response
		if(strlen(reply_string)>0)
			sprintf(reply_string,"%s\r\nOK",reply_string);
		else
			reply_string = "OK";
	}
	else
	{
		// Report 'ERROR'
		sprintf(reply_string,"ERROR");
	}
	
	// Reply to BT Peer if the command originated there (only if connected)
	if((codeless_env.command_route == CMD_FROM_PEER_RESP_PEER) && (ke_state_get(TASK_APP)==APP_CONNECTED))
	{
		send_to_peer(reply_string);
	}
	else if(codeless_env.command_route == CMD_FROM_LOCAL_RESP_LOCAL)
	{
		// Reply locally via serial port
		arch_puts("\r\n");
		arch_puts(reply_string);
		arch_puts("\r\n");
		arch_printf_process();
	}
	codeless_env.command_route = CMD_ROUTE_UNDEFINED;
  }
}
//



//--------------------------------------------------------------------------------------------------------/
//     _____ _                       _____       _ _ _                _        
//    |_   _(_)                     /  __ \     | | | |              | |       
//      | |  _ _ __ ___   ___ _ __  | /  \/ __ _| | | |__   __ _  ___| | _____ 
//      | | | | '_ ` _ \ / _ \ '__| | |    / _` | | | '_ \ / _` |/ __| |/ / __|
//      | | | | | | | | |  __/ |    | \__/\ (_| | | | |_) | (_| | (__|   <\__ \
//      \_/ |_|_| |_| |_|\___|_|     \____/\__,_|_|_|_.__/ \__,_|\___|_|\_\___/
//                                                                             
void throw_command0()
{
  codeless_env.suppress_response = true;
	user_execute_commands(memcmd[0]);
	codeless_env.suppress_response = false;
}

void throw_command1()
{
  codeless_env.suppress_response = true;
	user_execute_commands(memcmd[1]);
  codeless_env.suppress_response = false;
}

void throw_command2()
{
	codeless_env.suppress_response = true;
  user_execute_commands(memcmd[2]);
	codeless_env.suppress_response = false;
}

void throw_command3()
{
	codeless_env.suppress_response = true;
  user_execute_commands(memcmd[3]);
	codeless_env.suppress_response = false;
}
//


//--------------------------------------------------------------------------------------------------------/
//      ___ _____   _____                                           _   _____      _                           _            
//     / _ \_   _| /  __ \                                         | | |_   _|    | |                         | |           
//    / /_\ \| |   | /  \/ ___  _ __ ___  _ __ ___   __ _ _ __   __| |   | | _ __ | |_ ___ _ __ _ __  _ __ ___| |_ ___ _ __ 
//    |  _  || |   | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |   | || '_ \| __/ _ \ '__| '_ \| '__/ _ \ __/ _ \ '__|
//    | | | || |   | \__/\ (_) | | | | | | | | | | | (_| | | | | (_| |  _| || | | | ||  __/ |  | |_) | | |  __/ ||  __/ |   
//    \_| |_/\_/    \____/\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|  \___/_| |_|\__\___|_|  | .__/|_|  \___|\__\___|_|   
//                                                                                             | |                          
//                                                                                             |_|                          
// Interprets and handles individual AT commands
void user_cmd_interpreter(uint32_t bytes)
{
			
	// Pre-processing of command
	clean_command(bytes);
	
	// Extract command arguments
	extract_args();
	
	// Initialize the reply string
	char tempstr[CMD_BUFFER_SIZE];
	memset(tempstr,0,sizeof(tempstr));
	
	if(validate_at_cmd())
	{
		if(!extended_cmd)
		{
	    //--------------- Non-Extended AT commands (No '+' character in AT command)----------------//
			
			//-----------------------------------------------------------------------------------------//
		  //            AT
		  //-----------------------------------------------------------------------------------------//
		  // Returns OK
			if(user_compare_cmd("",0,0))
			{
				user_reply(tempstr, true);
			}
	
			
			//-----------------------------------------------------------------------------------------//
		  //            ATI
		  //-----------------------------------------------------------------------------------------//
		  // Lists SW release and HW information about the device
			else if(user_compare_cmd("I",0,0))
	    {
				// Read BD address from OTP (if any) 
				struct bd_addr address = user_read_otp_bdaddr();
				uint8_t addr[6];
				memcpy(&addr,&address,6);
				// Display the CodeLess revision, the SoC type and the OUI part of OTP BD address	
				sprintf
				(
				  tempstr,"CodeLess V2.3.20170609\r\nType: 0x%08X\r\nOUI: %02X:%02X:%02X",
					user_read_device_type(),addr[5],addr[4],addr[3]
				);
				// OUI look-up: https://standards.ieee.org/develop/regauth/oui/oui.csv
				user_reply(tempstr, true);
			}

			
			//-----------------------------------------------------------------------------------------//
		  //            ATE=<0:off/1:on>
		  //-----------------------------------------------------------------------------------------//
		  // Turns echo on or off (0:off / 1:on)
			else if(user_compare_cmd("E",1,1))
		  {
				// UART Echo on or off
			  if(ahtoi((char*)argument_array[0]) == 0)
					codeless_env.echo = false;
				else
					codeless_env.echo = true;
				user_reply(tempstr, true);
		  }
		
			
			//-----------------------------------------------------------------------------------------//
		  //            ATZ
		  //-----------------------------------------------------------------------------------------//
		  // Re-initializes the system
			else if(user_compare_cmd("Z",0,0))
		  {
			  user_codeless_init_system(); 
				user_reply(tempstr, true);
		  }
			else
				user_reply(tempstr,false);
		}
		
		
		//------------------- Extended AT commands (A '+' character in AT command) ------------------//
			
		
		//-----------------------------------------------------------------------------------------//
		//            AT+CURSOR
		//-----------------------------------------------------------------------------------------//
		// Places a time stamp cursor in SmartSnippets' power profiler 
		else if(user_compare_cmd("CURSOR",0,0))
		{
  	  if (DEVELOPMENT_DEBUG)
			{
				// During debugging we can use this function
				arch_set_pxact_gpio();
			}
			else
      {	
				// When debugging is disabled, we can set the cursor as follows:
			  uint32_t i;
        SetWord16(P13_MODE_REG, PID_GPIO|OUTPUT);
        SetWord16(P1_SET_DATA_REG, 0x8);
        for (i = 0; i < 150; i++); //20 is almost 7.6usec of time.
        SetWord16(P1_RESET_DATA_REG, 0x8);
			}
			user_reply(tempstr, true);		 		
		}	

		
    //-----------------------------------------------------------------------------------------//
		//            AT+BDADDR
		//-----------------------------------------------------------------------------------------//
		// Displays the Bluetooth device address
		else if(user_compare_cmd("BDADDR",0,0))
		{
			uint8_t addr[6];
			memcpy(&addr,&codeless_env.device_address,6);
			sprintf(tempstr,"%02X:%02X:%02X:%02X:%02X:%02X",addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
			user_reply(tempstr, true);
		}		

		
		//-----------------------------------------------------------------------------------------//
		//            AT+CMDSTORE=<index:0-3>,<semicolon separated command strings>
		//-----------------------------------------------------------------------------------------//
		// Stores a single or multiple commands (semicolon separated)
		else if(user_compare_cmd("CMDSTORE",2,50))
		{
			uint8_t index = ahtoi((char*)argument_array[0]);
			if(index < 4)
			{
			  memset(memcmd[index],0,100);
				uint8_t i;
				for(i = 0; i < bytes; i++)
				  if(codeless_env.current_cmd[i] == ',')
						break;
					
			  memcpy(&memcmd[index],&codeless_env.current_cmd[i+1],bytes - i -1);
			  user_reply(tempstr, true);
			}
			else
				user_reply(tempstr, false);
		}		
		
		
		//-----------------------------------------------------------------------------------------//
		//            AT+CMDPLAY=<index:0-3>
		//-----------------------------------------------------------------------------------------//
		// Executes a stored command command string
		else if(user_compare_cmd("CMDPLAY",1,1))
		{
			 uint8_t index = ahtoi((char*)argument_array[0]);
			 if(index < 4)
			 {
				 user_execute_commands(memcmd[ahtoi((char*)argument_array[0])]);
			 }
			 else
			   user_reply(tempstr, false);
		}		
		
		
		//-----------------------------------------------------------------------------------------//
		//            AT+CMD=<index:0-3>
		//-----------------------------------------------------------------------------------------//
		// Displays a stored command string
		else if(user_compare_cmd("CMD",1,1))
		{
			uint8_t index = ahtoi((char*)argument_array[0]);
			if(index < 4)
			{
				sprintf(tempstr,"%s",memcmd[ahtoi((char*)argument_array[0])]);
				user_reply(tempstr, true);
			}
			else
			  user_reply(tempstr, false);
		}		
		
		
		//-----------------------------------------------------------------------------------------//
		//            AT+TMRSTART=<timer index: 0-3>,<command index: 0-3>,<timeout in tens of ms>
		//-----------------------------------------------------------------------------------------//
		// Starts a timer
		else if(user_compare_cmd("TMRSTART",3,3))
		{
			
			uint8_t timer_id = ahtoi((char*)argument_array[0]);
			uint8_t index = ahtoi((char*)argument_array[1]);
			
			if(index == 0)
				codeless_env.tmr[timer_id] = app_easy_timer(ahtoi((char*)argument_array[2]),throw_command0);
			else if(index == 1)
				codeless_env.tmr[timer_id] = app_easy_timer(ahtoi((char*)argument_array[2]),throw_command1);
			else if(index == 2)
				codeless_env.tmr[timer_id] = app_easy_timer(ahtoi((char*)argument_array[2]),throw_command2);
			else if(index == 3)
				codeless_env.tmr[timer_id] = app_easy_timer(ahtoi((char*)argument_array[2]),throw_command3);
			else
				user_reply(tempstr, false);
			if(index < 4)
				user_reply(tempstr, true);
		}		
		
		
		//-----------------------------------------------------------------------------------------//
		//            AT+TMRSTOP=<timer index: 0-3>
		//-----------------------------------------------------------------------------------------//
		// Stops a timer
		else if(user_compare_cmd("TMRSTOP",1,1))
		{
			uint8_t index = ahtoi((char*)argument_array[0]);
			if(index < 4)
			{
				if(codeless_env.tmr[index] > 0)
					app_easy_timer_cancel(codeless_env.tmr[index]);
				user_reply(tempstr, true);
			}
			else
				user_reply(tempstr, false);
			
		}		
		
    //-----------------------------------------------------------------------------------------//
		//            AT+SLEEP=<0:awake/1:sleep>
		//-----------------------------------------------------------------------------------------//
		// Controls sleep mode (0:awake / 1:extended sleep
		else if(user_compare_cmd("SLEEP",1,1))
		{
			if(ahtoi((char*)argument_array[0]) == 0)
			  arch_disable_sleep();
			else
				user_prepare_sleep();
			user_reply(tempstr, true);
		}		

		
		//-----------------------------------------------------------------------------------------//
		//            AT+IOCFG[=<pin#>,<IO functionality>]
		//-----------------------------------------------------------------------------------------//
		// Sets the functionality of a specified IO pin. Lists all pins if no arguments provided
		else if(user_compare_cmd("IOCFG",0,2))
		{
			bool port_cfg_failed = false;
			if (argument_count == 1)
				user_reply(tempstr,false); // Must have zero or two arguments
      else
			{				
	  		if(argument_count == 0)
			  {
  				// List all IO configuration settings
	  		  for(uint8_t i=0; i<40; i++)
		  	  {				
			  	  if(codeless_env.port_configuration[i] != IO_FUNC_NOT_AVAILABLE)
				      sprintf(tempstr,"%s%02d ",tempstr,(uint8_t)codeless_env.port_configuration[i]);
				    if((i % 10 == 9) && (i < 30) ) 
					    sprintf(tempstr,"%s\r\n",tempstr);
			    }
					user_reply(tempstr, true);
			  }
			  else // argument_count = 2
			  {
  				// Set specific IO configuration
					
					// Extract the port and desired functionality from arguments
					uint8_t port = ahtoi((char*)argument_array[0]);
					uint8_t func = ahtoi((char*)argument_array[1]);
					
					// Validate port and functionality
					if
					(	   (codeless_env.port_configuration[port] != IO_FUNC_NOT_AVAILABLE)
					  && (port < 40)
					  && ((IO_FUNC_t)func < IO_FUNC_NOT_AVAILABLE)
					  && (port != UART2_TX_GPIO_PORT * 10 + UART2_TX_GPIO_PIN) // Functionality cannot be changed - must remain UART TX
					  && (port != UART2_RX_GPIO_PORT * 10 + UART2_RX_GPIO_PIN) // Functionality cannot be changed - must remain UART RX
					)
					{
					  // Resolve port and pin numbers from argument
            uint8_t port_number = port / 10;
            uint8_t pin_number = port % 10;
            // Set port configuration accordingly
            if(func == IO_FUNC_INPUT)
              // Standard input port
              GPIO_ConfigurePin((GPIO_PORT)port_number, (GPIO_PIN)pin_number, INPUT, PID_GPIO, false );
            else if(func == IO_FUNC_INPUT_PULLUP)
              // Input port with pull-up resistor
              GPIO_ConfigurePin((GPIO_PORT)port_number, (GPIO_PIN)pin_number, INPUT_PULLUP, PID_GPIO, false );
            else if(func == IO_FUNC_INPUT_PULLDOWN)
              // Input port with pull-down resistor
              GPIO_ConfigurePin((GPIO_PORT)port_number, (GPIO_PIN)pin_number, INPUT_PULLDOWN, PID_GPIO, false );
            else if(func == IO_FUNC_OUTPUT)
              // Output port
              GPIO_ConfigurePin((GPIO_PORT)port_number, (GPIO_PIN)pin_number, OUTPUT, PID_GPIO, false);  
            else if((func == IO_FUNC_ANALOG_INPUT || func == IO_FUNC_ANALOG_INPUT_ATT3) && (port_number == 0) && (pin_number <= 3))
              // Analog input port
              GPIO_ConfigurePin((GPIO_PORT)port_number, (GPIO_PIN)pin_number, INPUT, PID_ADC, false );            
            else if(func == IO_FUNC_I2C_CLOCK)
            {
							// Remove any existing I2C clock pins
							for(uint8_t i = 0; i<40; i++)
								if(codeless_env.port_configuration[i] == IO_FUNC_I2C_CLOCK) 
									codeless_env.port_configuration[i] = IO_FUNC_UNDEFINED;
              // I2C clock output port
							GPIO_ConfigurePin((GPIO_PORT)port_number, (GPIO_PIN)pin_number, OUTPUT, PID_I2C_SCL, false );
            }
            else if(func == IO_FUNC_I2C_DATA)
            {
              // Remove any existing I2C data pins
							for(uint8_t i = 0; i<40; i++)
								if(codeless_env.port_configuration[i] == IO_FUNC_I2C_DATA) 
									codeless_env.port_configuration[i] = IO_FUNC_UNDEFINED;
							// I2C data port
              GPIO_ConfigurePin((GPIO_PORT)port_number, (GPIO_PIN)pin_number, OUTPUT, PID_I2C_SDA, false );  
            }
						else if(func == IO_FUNC_CONN_IND_HI)
						{
							// Connection indicator, high during a connection 
							codeless_env.port_configuration[port] = IO_FUNC_CONN_IND_HI;
							if(ke_state_get(TASK_APP)==APP_CONNECTED)
							  GPIO_ConfigurePin((GPIO_PORT)port_number, (GPIO_PIN)pin_number, OUTPUT, PID_GPIO, true );
							else
							  GPIO_ConfigurePin((GPIO_PORT)port_number, (GPIO_PIN)pin_number, OUTPUT, PID_GPIO, false );
						}
						else if(func == IO_FUNC_CONN_IND_LO)
						{
							// Connection indicator, low during a connection
							codeless_env.port_configuration[port] = IO_FUNC_CONN_IND_LO;
							if(ke_state_get(TASK_APP)==APP_CONNECTED)
							  GPIO_ConfigurePin((GPIO_PORT)port_number, (GPIO_PIN)pin_number, OUTPUT, PID_GPIO, false );
							else
							  GPIO_ConfigurePin((GPIO_PORT)port_number, (GPIO_PIN)pin_number, OUTPUT, PID_GPIO, true );
						}
						else
						{
							port_cfg_failed = true;
							user_reply(tempstr,false);
						}
						
						if(!port_cfg_failed)
						{
						  codeless_env.port_configuration[port] = (IO_FUNC_t)func;
						  user_reply(tempstr, true);	
						}
						
					}
					else
				    user_reply(tempstr,false); 
	  		}	
			}		 		
		}		
		
		
		//-----------------------------------------------------------------------------------------//
		//            AT+IO=<pin#>[,<0:low/1:high>]
		//-----------------------------------------------------------------------------------------//
		// Set Output pin (2 arguments) or read pin state (single argument)
		else if(user_compare_cmd("IO",1,2))
		{
			uint8_t port = ahtoi((char*)argument_array[0]);
			uint8_t port_number = port / 10;
      uint8_t pin_number = port % 10;
			
			if(argument_count == 1)
			{
				// Read (one argument means 'read')
				
				// Pin must be configured as input or output for read operation to work
				if(
					   (codeless_env.port_configuration[port] == IO_FUNC_INPUT)
					|| (codeless_env.port_configuration[port] == IO_FUNC_INPUT_PULLUP)
					|| (codeless_env.port_configuration[port] == IO_FUNC_INPUT_PULLDOWN)
				  || (codeless_env.port_configuration[port] == IO_FUNC_OUTPUT)

			  )
			  {
				  // Read pin status 
				  if(GPIO_GetPinStatus((GPIO_PORT)port_number,(GPIO_PIN)pin_number))
					  sprintf(tempstr,"%d",1);
				  else
					  sprintf(tempstr,"%d",0);
					user_reply(tempstr,true);
			  }
				else
				  user_reply(tempstr,false);
			} 		
			
			else //  argument_count = 2
			{
				// Write (two arguments means 'set port')
				uint8_t data = ahtoi((char*)argument_array[1]);
				
				// Pin must be configured as output and data must be 0 or 1
				if((codeless_env.port_configuration[port] == IO_FUNC_OUTPUT)&& (data == 0 || data == 1))
				{
          if(data == 0)
					{
            GPIO_SetInactive((GPIO_PORT)port_number,(GPIO_PIN)pin_number);	
						codeless_env.port_io_state[port] = false;
					}
					else
					{
						GPIO_SetActive((GPIO_PORT)port_number,(GPIO_PIN)pin_number);
						codeless_env.port_io_state[port] = true;
					}
          user_reply(tempstr, true);					
				}
			  else 
					user_reply(tempstr,false);
      }			
			
		}
		
		
		//-----------------------------------------------------------------------------------------//
		//            AT+ADC=<pin#>
		//-----------------------------------------------------------------------------------------//
		// Read an analog input (Only pin P0_0 - P0_3!)
		else if(user_compare_cmd("ADC",1,1))
		{
			uint8_t port = ahtoi((char*)argument_array[0]);
			if
			(      (port<4) 
				  && ((codeless_env.port_configuration[port] == IO_FUNC_ANALOG_INPUT_ATT3) || (codeless_env.port_configuration[port] == IO_FUNC_ANALOG_INPUT))
			)
			{		  
				// Use 1:3 attenuation if applicable
				if ((codeless_env.port_configuration[port] == IO_FUNC_ANALOG_INPUT))
				  adc_init(GP_ADC_SE, 0, 0);	
				else
				  adc_init(GP_ADC_SE, 0, GP_ADC_ATTN3X);
				
				// Open channel and take a dummy sample
				adc_enable_channel(port);
        adc_get_sample();
        
				sprintf(tempstr,"%d",adc_get_sample());
		  	
				// Disable the ADC
				adc_disable();
		  	user_reply(tempstr, true);
			}
			else
				user_reply(tempstr,false);
		}
		
		//-----------------------------------------------------------------------------------------//
		//            AT+BATT
		//-----------------------------------------------------------------------------------------//
		// Query the battery capacity in percentage
		else if(user_compare_cmd("BATT",0,0))
		{
		  // Read the battery capacity (TODO: Currently assuming coin cell CR2032) 
      sprintf(tempstr,"%d",battery_get_lvl(BATT_CR2032));
			user_reply(tempstr, true);
		}
		
		
		//-----------------------------------------------------------------------------------------//
		//            AT+I2CSCAN
		//-----------------------------------------------------------------------------------------//
		// Scan for I2C slaves 
		else if(user_compare_cmd("I2CSCAN",0,0))
		{
		  // Scan for I2C slaves only if hardware has been configured to support I2C        
      if(user_is_i2c_hw_valid())
      {
		  	user_i2c_scan(tempstr);
		    user_reply(tempstr, true);	
      }
		  else 
        // Hardware has not been configured to support I2C
        user_reply(tempstr,false);
	  }
		
		
		//-----------------------------------------------------------------------------------------//
		//            AT+I2CREAD=<slave address>,<slave register>[,<number of bytes>]
		//-----------------------------------------------------------------------------------------//
		// Read one or more I2C slave registers
		else if(user_compare_cmd("I2CREAD",2,3))
		{
		  // If no third argument is provided, read 1 byte only
			uint8_t num_bytes = 1;
			if(argument_count == 3)
				num_bytes = ahtoi((char*)argument_array[2]);
			// Check for valid hardware configuration
			if(user_is_i2c_hw_valid())
			{
			  user_i2c_read(ahtoi((char*)argument_array[0]),ahtoi((char*)argument_array[1]),num_bytes,tempstr);
				user_reply(tempstr, true);	
      }
			else
        // Hardware has not been configured to support I2C
        user_reply(tempstr,false);
		}
		
		
		//-----------------------------------------------------------------------------------------//
		//            AT+I2CWRITE=<slave address>,<slave register>,<value to write>
		//-----------------------------------------------------------------------------------------//
		// Write to a single I2C slave register		
		else if(user_compare_cmd("I2CWRITE",3,3))
		{
		  // Check for valid hardware configuration
			if(user_is_i2c_hw_valid())
			{
        user_i2c_write(ahtoi((char*)argument_array[0]),ahtoi((char*)argument_array[1]),ahtoi((char*)argument_array[2]));
				user_reply(tempstr, true);	
      }
			else
        user_reply(tempstr,false);
		}
		
		
		//-----------------------------------------------------------------------------------------//
		//            AT+PRINT=<string to write>
		//-----------------------------------------------------------------------------------------//
		// Print a string to local UART		
		else if(user_compare_cmd("PRINT",1,1))
		{
			arch_printf("\r\n%s\r\n",argument_array[0]);
			user_reply(tempstr, true);
		}

		
		//-----------------------------------------------------------------------------------------//
		//            AT+GAPSTATUS
		//-----------------------------------------------------------------------------------------//
		// Report the Bluetooth GAP role and conection status
		else if(user_compare_cmd("GAPSTATUS",0,0))
		{
			sprintf(tempstr,"%d,%d",(codeless_env.bt_role == GAP_CENTRAL_MST),(ke_state_get(TASK_APP) == APP_CONNECTED) );
			user_reply(tempstr, true);
		}
		
		//-----------------------------------------------------------------------------------------//
		//            AT+ADVDATA=<advertise data in hex format>
		//-----------------------------------------------------------------------------------------//
		// Set Bluetooth advertising data		
		else if(user_compare_cmd("ADVDATA",1,1))
		{
			uint8_t byte_array[32];
			uint8_t len;
			if((validate_adv_data(argument_array[0], byte_array, &len)) && (len<=28)) 
			{
				memcpy(&codeless_env.adv_data, &byte_array, len);
				codeless_env.adv_data_len = len;
				user_reply(tempstr, true);
			}
			else 
				user_reply(tempstr, false);
		}
		
		//-----------------------------------------------------------------------------------------//
		//            AT+ADVDATA
		//-----------------------------------------------------------------------------------------//
		// Get Bluetooth advertising data		
		else if(user_compare_cmd("ADVDATA",0,0))
		{
			for(uint8_t i=0; i < codeless_env.adv_data_len - 1 ; i++)
			{
			  sprintf(tempstr,"%s%02X:",tempstr,codeless_env.adv_data[i]);
			}
			sprintf(tempstr,"%s%02X",tempstr,codeless_env.adv_data[codeless_env.adv_data_len - 1]);
			user_reply(tempstr, true);
		}
		
		//-----------------------------------------------------------------------------------------//
		//            AT+ADVRESP=<scan response data in hex format>
		//-----------------------------------------------------------------------------------------//
		// Set Bluetooth advertising data		
		else if(user_compare_cmd("ADVRESP",1,1))
		{
			uint8_t byte_array[32];
			uint8_t len;
			if((validate_adv_data(argument_array[0], byte_array, &len)) && (len<=31)) 
			{
				memcpy(&codeless_env.resp_data, &byte_array, len);
				codeless_env.resp_data_len = len;
				user_reply(tempstr, true);
			}
			else 
				user_reply(tempstr, false);
		}
		
		//-----------------------------------------------------------------------------------------//
		//            AT+ADVRESP
		//-----------------------------------------------------------------------------------------//
		// Get Bluetooth advertising data		
		else if(user_compare_cmd("ADVRESP",0,0))
		{
			if( codeless_env.resp_data_len > 0)
			{
				for(uint8_t i=0; i < codeless_env.resp_data_len - 1 ; i++)
			    sprintf(tempstr,"%s%02X:",tempstr,codeless_env.resp_data[i]);
			  sprintf(tempstr,"%s%02X",tempstr,codeless_env.resp_data[codeless_env.resp_data_len - 1]);
			}
			user_reply(tempstr, true);
		}
		
		//-----------------------------------------------------------------------------------------//
		//            AT+ADVSTOP
		//-----------------------------------------------------------------------------------------//
		// Stop Bluetooth advertising		
		else if(user_compare_cmd("ADVSTOP",0,0))
		{
			if
			(
			     (codeless_env.bt_role == GAP_PERIPHERAL_SLV)
        && (codeless_env.gap_activity == ADVERTISING)
			  && (codeless_env.command_route == CMD_FROM_LOCAL_RESP_LOCAL)
				&& (ke_state_get(TASK_APP) != APP_CONNECTED)
			)
			{
				app_easy_gap_advertise_stop();
			}
			else 
				user_reply(tempstr, false);
		}

		
		//-----------------------------------------------------------------------------------------//
		//            AT+ADVSTART[=<advertising interval in miliseconds>]
		//-----------------------------------------------------------------------------------------//
		// Start Bluetooth advertising		
		else if(user_compare_cmd("ADVSTART",0,1))
		{
			if
			(
			     (codeless_env.bt_role == GAP_PERIPHERAL_SLV)
        && (codeless_env.gap_activity == IDLE)			
			  && (codeless_env.command_route == CMD_FROM_LOCAL_RESP_LOCAL)
			  && (ke_state_get(TASK_APP) != APP_CONNECTED)
			)
			{				
			  if(argument_count == 1)
				{
				uint16_t intv = ahtoi((char*)argument_array[0]);	
			  if(intv>=100 && intv<=3000)
				  // Set the advertising interval
			    codeless_env.adv_intv = intv *1.6;	
			  }
			  // Start advertising
				
			  user_advertise();
			  user_reply(tempstr, true);
			}
			else 
				user_reply(tempstr, false);
		}

		
		//-----------------------------------------------------------------------------------------//
		//            AT+CENTRAL
		//-----------------------------------------------------------------------------------------//
		// Put the device in central Bluetooth role		
		else if(user_compare_cmd("CENTRAL",0,0))
		{
			if
			(
			     (codeless_env.bt_role == GAP_PERIPHERAL_SLV)
        && (codeless_env.gap_activity == IDLE)
        && (codeless_env.command_route == CMD_FROM_LOCAL_RESP_LOCAL)
				&& (ke_state_get(TASK_APP) != APP_CONNECTED)
			)
			{
			  // Set GAP activity and central role
				codeless_env.gap_activity = CHANGING_ROLE;
			  user_app_configuration_func(GAP_CENTRAL_MST);
			}
			else
				user_reply(tempstr, false);
		}

		
		//-----------------------------------------------------------------------------------------//
		//            AT+PERIPHERAL
		//-----------------------------------------------------------------------------------------//
		// Put the device in peripheral Bluetooth role		
		else if(user_compare_cmd("PERIPHERAL",0,0))
		{
			if
			(
			     (codeless_env.bt_role == GAP_CENTRAL_MST)
        && (codeless_env.gap_activity == IDLE)
        && (codeless_env.command_route == CMD_FROM_LOCAL_RESP_LOCAL)
				&& (ke_state_get(TASK_APP) != APP_CONNECTED)		
			)
			{
				// Set peripheral role
			  user_app_configuration_func(GAP_PERIPHERAL_SLV);
			}
			else
				user_reply(tempstr, false);
		}

		
		//-----------------------------------------------------------------------------------------//
		//            AT+GAPSCAN
		//-----------------------------------------------------------------------------------------//
		// Start a Bluetooth scan		
		else if(user_compare_cmd("GAPSCAN",0,0))
		{
			if
			(
			     (codeless_env.bt_role == GAP_CENTRAL_MST)
        && (codeless_env.gap_activity == IDLE)
        && (codeless_env.command_route == CMD_FROM_LOCAL_RESP_LOCAL)
				&& (ke_state_get(TASK_APP) != APP_CONNECTED)			
			)
			{
				arch_printf("\r\nScanning...\r\n");
				codeless_env.gap_activity = SCANNING;
			  user_scan_start();
			}
			else 
				user_reply(tempstr, false);
		}

		
		//-----------------------------------------------------------------------------------------//
		//            AT+GAPCONNECT=<Slave BD Address>,<Address type P:public/R:random>
		//-----------------------------------------------------------------------------------------//
		// Initiate a Bluetooth connection		
		else if(user_compare_cmd("GAPCONNECT",2,2))
		{
			if
			(
			     (codeless_env.bt_role == GAP_CENTRAL_MST)
        && (codeless_env.gap_activity == IDLE)
        && (codeless_env.command_route == CMD_FROM_LOCAL_RESP_LOCAL)			
	  		&& (ke_state_get(TASK_APP) != APP_CONNECTED)								  // Valid App state?		
        && ((argument_array[1][0] == 'R')||(argument_array[1][0] == 'P'))
			  && ((uint8_t)strlen((char*)argument_array[0]) == 17) // BD Address must be 17 character string XX:XX:XX:XX:XX:XX
			)
			{
				uint8_t connect_bdaddr_type;
				arch_puts("\r\nConnecting...\r\n");
				if(argument_array[1][0] == 'R')
					connect_bdaddr_type = GAPM_PROVIDED_RND_ADDR;  // Peer is random address type
				else
					connect_bdaddr_type = GAPM_PUBLIC_ADDR;  // Peer is public address type
				struct bd_addr address = strtobdaddr((char*)argument_array[0]);
				uint8_t addr[6];
				memcpy(&addr,&address,6);
				user_connect_to_peripheral(connect_bdaddr_type, addr, 10);
				codeless_env.gap_activity = INITIATING_CONNECTION;
			}
			else 
			  user_reply(tempstr,false);
		}

		
		//-----------------------------------------------------------------------------------------//
		//            AT+GAPDISCONNECT
		//-----------------------------------------------------------------------------------------//
		// Disconnect from a Bluetooth peer
		else if(user_compare_cmd("GAPDISCONNECT",0,0))
		{
			if(ke_state_get(TASK_APP) == APP_CONNECTED) 
			{
				arch_printf("\r\nDisconnecting...");
				codeless_env.gap_activity = DISCONNECTING;
				app_disconnect_op(0, CO_ERROR_REMOTE_USER_TERM_CON);			
			}
			else
		    user_reply(tempstr,false);
		}

		
		//-----------------------------------------------------------------------------------------//
		//            AT+RANDOM
		//-----------------------------------------------------------------------------------------//
		// Produce a random 32bit number		
		else if(user_compare_cmd("RANDOM",0,0))
		{
		  // Respond with a 32bit random number 
      sprintf(tempstr,"0x%08X",rand());
      user_reply(tempstr,true);
		}
		
						
		//-----------------------------------------------------------------------------------------//
		//   Syntax Error, wrong context, or unknown command
		//-----------------------------------------------------------------------------------------//
		// Command not recognized, wrong context, or invalid argument count  
		else
			user_reply(tempstr,false);
		}
	
		
	// invalid AT command
	else
		user_reply(tempstr,false);
}
//


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

/// @} APP
