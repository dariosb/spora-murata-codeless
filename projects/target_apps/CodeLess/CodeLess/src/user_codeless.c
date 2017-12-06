/**
 ****************************************************************************************
 *
 * @file user_codeless.c
 *
 * @brief codeless project source code.
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

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */


#include "user_codeless.h"

#include "app_api.h"
#include "arch_console.h"
#include "uart.h"
#include "wkupct_quadec.h"
#include "gpio.h"
#include "custs1_task.h"

#include "user_cmd_parser.h"
#include "user_cmd_interpreter.h"
#include "user_custs_config.h"
#include "user_custs1_def.h"
#include "user_random_bd_addr.h"
#include "user_periph_setup.h"
#include "user_gap.h"
#include "user_gatt.h"
			
/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

// This timer is used in order to activate sleepmode 10 seconds after bootup
timer_hnd sleep_timer; 


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/


//--------------------------------------------------------------------------------------------------------/
//     _____                             _   _               _____    _        _     _ _     _              _ 
//    /  __ \                           | | (_)             |  ___|  | |      | |   | (_)   | |            | |
//    | /  \/ ___  _ __  _ __   ___  ___| |_ _  ___  _ __   | |__ ___| |_ __ _| |__ | |_ ___| |__   ___  __| |
//    | |    / _ \| '_ \| '_ \ / _ \/ __| __| |/ _ \| '_ \  |  __/ __| __/ _` | '_ \| | / __| '_ \ / _ \/ _` |
//    | \__/\ (_) | | | | | | |  __/ (__| |_| | (_) | | | | | |__\__ \ || (_| | |_) | | \__ \ | | |  __/ (_| |
//     \____/\___/|_| |_|_| |_|\___|\___|\__|_|\___/|_| |_| \____/___/\__\__,_|_.__/|_|_|___/_| |_|\___|\__,_|
//                                                                                                            
//                                                                                                            
// This event is called when a Bluetooth connection is established   
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
  // Call the default handler for this event
	default_app_on_connection(connection_idx, param);	
  if(codeless_env.bt_role == GAP_CENTRAL_MST)
	{
  	arch_puts("Connected...");
				
		// Make a GATT characteristic discovery to identify the 16bit attribute handles of the three CodeLess characteristics
		codeless_env.gap_activity = DISCOVERING;
		user_gatt_discover_chars();
	}
	
  // Set any connection indicator output 	
	for(uint8_t i = 0; i<40; i++)
	{
	  if(codeless_env.port_configuration[i] == IO_FUNC_CONN_IND_HI)
			GPIO_ConfigurePin((GPIO_PORT)(i/10), (GPIO_PIN)(i%10), OUTPUT, PID_GPIO, true );
		if(codeless_env.port_configuration[i] == IO_FUNC_CONN_IND_LO)
			GPIO_ConfigurePin((GPIO_PORT)(i/10), (GPIO_PIN)(i%10), OUTPUT, PID_GPIO, false );
	}
}
//


//--------------------------------------------------------------------------------------------------------/
//     _____                             _   _              ______    _ _          _ 
//    /  __ \                           | | (_)             |  ___|  (_) |        | |
//    | /  \/ ___  _ __  _ __   ___  ___| |_ _  ___  _ __   | |_ __ _ _| | ___  __| |
//    | |    / _ \| '_ \| '_ \ / _ \/ __| __| |/ _ \| '_ \  |  _/ _` | | |/ _ \/ _` |
//    | \__/\ (_) | | | | | | |  __/ (__| |_| | (_) | | | | | || (_| | | |  __/ (_| |
//     \____/\___/|_| |_|_| |_|\___|\___|\__|_|\___/|_| |_| \_| \__,_|_|_|\___|\__,_|
//
// Connection attempt failed or was cancelled by user                                                                                   
void user_on_connect_failed()
{
	arch_puts("Connection Cancelled...\r\nOK\r\n");
	    codeless_env.gap_activity = IDLE;
}
//


//--------------------------------------------------------------------------------------------------------/
//     _____                             _   _               _____                   _             _           _ 
//    /  __ \                           | | (_)             |_   _|                 (_)           | |         | |
//    | /  \/ ___  _ __  _ __   ___  ___| |_ _  ___  _ __     | | ___ _ __ _ __ ___  _ _ __   __ _| |_ ___  __| |
//    | |    / _ \| '_ \| '_ \ / _ \/ __| __| |/ _ \| '_ \    | |/ _ \ '__| '_ ` _ \| | '_ \ / _` | __/ _ \/ _` |
//    | \__/\ (_) | | | | | | |  __/ (__| |_| | (_) | | | |   | |  __/ |  | | | | | | | | | | (_| | ||  __/ (_| |
//     \____/\___/|_| |_|_| |_|\___|\___|\__|_|\___/|_| |_|   \_/\___|_|  |_| |_| |_|_|_| |_|\__,_|\__\___|\__,_|
//                                                                                                               
// This event is triggered when an existing Bluetooth connection is terminated                                                                                                              
void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
  ke_state_set(TASK_APP,APP_CONNECTABLE);
	if(codeless_env.bt_role == GAP_CENTRAL_MST)
	{
		codeless_env.gap_activity = IDLE;
	  arch_puts("\r\nDisconnected...\r\nOK\r\n");
	}
  else
		// Call the default handler for this event
	  default_app_on_disconnect(param);
  
	
	// No longer connected as master to a CodeLess slave 
	codeless_env.linked_to_codeless_device = false;
	
	// Set any connection indicator output 
	for(uint8_t i = 0; i<40; i++)
	{
	  if(codeless_env.port_configuration[i] == IO_FUNC_CONN_IND_HI)
			GPIO_ConfigurePin((GPIO_PORT)(i/10), (GPIO_PIN)(i%10), OUTPUT, PID_GPIO, false );
		if(codeless_env.port_configuration[i] == IO_FUNC_CONN_IND_LO)
			GPIO_ConfigurePin((GPIO_PORT)(i/10), (GPIO_PIN)(i%10), OUTPUT, PID_GPIO, true );		
	}
	
	
}
//

//--------------------------------------------------------------------------------------------------------/
//     _____      _ _     _____                      _    ___ _____ 
//    |_   _|    (_) |   /  ___|                    | |  / _ \_   _|
//      | | _ __  _| |_  \ `--. _ __ ___   __ _ _ __| |_/ /_\ \| |  
//      | || '_ \| | __|  `--. \ '_ ` _ \ / _` | '__| __|  _  || |  
//     _| || | | | | |_  /\__/ / | | | | | (_| | |  | |_| | | || |  
//     \___/_| |_|_|\__| \____/|_| |_| |_|\__,_|_|   \__\_| |_/\_/  
//                                                                  
// initialization of CodeLess application                                                                     
void user_codeless_init_system(void)
{
	// Inirtially, set All IOs to undefined:
	for(uint8_t i = 0; i<40; i++)
	{
	  codeless_env.port_configuration[i] = IO_FUNC_UNDEFINED;
		if( (i % 10 == 8) || (i % 10 == 9))
			codeless_env.port_configuration[i] = IO_FUNC_NOT_AVAILABLE;
	}
	// Set P0_4 to UART1 TX 
	codeless_env.port_configuration[4] = IO_FUNC_UART2_TX;
	
	// Set P0_5 to UART1 RX 
	codeless_env.port_configuration[5] = IO_FUNC_UART2_RX;
	
	// Set UART echo on
	codeless_env.echo = true;
}
//


//--------------------------------------------------------------------------------------------------------/
//        _       _                _   _          
//       / \   __| |_   _____ _ __| |_(_)___  ___ 
//      / _ \ / _` \ \ / / _ \ '__| __| / __|/ _ \
//     / ___ \ (_| |\ V /  __/ |  | |_| \__ \  __/
//    /_/   \_\__,_| \_/ \___|_|   \__|_|___/\___|
//                                                
void user_advertise()
{
  
  struct gapm_start_advertise_cmd *adv_cmd;               // Retained version
  struct gapm_start_advertise_cmd *cmd;                   // Version we send
    
  // Create an empty advertise start command
  cmd = app_advertise_start_msg_create();    
    
  // Get the one from the app_easy function
  adv_cmd = app_easy_gap_undirected_advertise_get_active();
    
  // Make a copy of the data
  memcpy(cmd, adv_cmd, sizeof(struct gapm_start_advertise_cmd));
  
  // Load the random Bluetooth Device Address
  cmd->op.addr_src = GAPM_PROVIDED_RND_ADDR;
  memcpy(&cmd->op.addr.addr,&codeless_env.device_address,6);
  
	// Specify the advertise data
	cmd->info.host.adv_data_len = codeless_env.adv_data_len;
  memcpy(&cmd->info.host.adv_data,&codeless_env.adv_data,codeless_env.adv_data_len); 	
	
	// Specify scan response data (if any)
	if(codeless_env.resp_data_len > 0)
	{
		cmd->info.host.scan_rsp_data_len = codeless_env.resp_data_len;
		memcpy(&cmd->info.host.scan_rsp_data,&codeless_env.resp_data,codeless_env.resp_data_len);
	}
	
	cmd->intv_min = codeless_env.adv_intv;
	cmd->intv_max = codeless_env.adv_intv;
  
	// Send message to GAP task
  ke_msg_send(cmd);
  
  // We are now connectable
  ke_state_set(TASK_APP, APP_CONNECTABLE);
	
	codeless_env.gap_activity = ADVERTISING;
}
//


//--------------------------------------------------------------------------------------------------------/
//      ___      _                _   _            _____ _              
//     / _ \    | |              | | (_)          /  ___| |             
//    / /_\ \ __| |_   _____ _ __| |_ _ ___  ___  \ `--.| |_ ___  _ __  
//    |  _  |/ _` \ \ / / _ \ '__| __| / __|/ _ \  `--. \ __/ _ \| '_ \ 
//    | | | | (_| |\ V /  __/ |  | |_| \__ \  __/ /\__/ / || (_) | |_) |
//    \_| |_/\__,_| \_/ \___|_|   \__|_|___/\___| \____/ \__\___/| .__/ 
//                                                               | |    
//                                                               |_|    
// Called when advertising stops
void user_on_adv_stop(const uint8_t status)
{ 
	codeless_env.gap_activity = IDLE;
	arch_puts("\r\nOK\r\n");
}
//


//--------------------------------------------------------------------------------------------------------/
//     _   _  ___  ______ _____   _____               _  ______              _               _ 
//    | | | |/ _ \ | ___ \_   _| /  __ \             | | | ___ \            (_)             | |
//    | | | / /_\ \| |_/ / | |   | /  \/_ __ ___   __| | | |_/ /___  ___ ___ ___   _____  __| |
//    | | | |  _  ||    /  | |   | |   | '_ ` _ \ / _` | |    // _ \/ __/ _ \ \ \ / / _ \/ _` |
//    | |_| | | | || |\ \  | |   | \__/\ | | | | | (_| | | |\ \  __/ (_|  __/ |\ V /  __/ (_| |
//     \___/\_| |_/\_| \_| \_/    \____/_| |_| |_|\__,_| \_| \_\___|\___\___|_| \_/ \___|\__,_|
//                                                                                             
// This event is triggered when a carriage-return character is received via UART                                                                                                       
void user_uart_cmd_received_callback(uint8_t status, uint32_t bytes)
{
	// Stop the sleep timer if it is still active
	if(sleep_timer != NULL)
	  app_easy_timer_cancel(sleep_timer);
	
	// Stop any ongoing GAP central activity
	if
	(
			   codeless_env.gap_activity == INITIATING_CONNECTION
			|| codeless_env.gap_activity == SCANNING
			|| codeless_env.gap_activity == DISCOVERING
	)
	{
		// Create a GAP activity cancel message 
		struct gapm_cancel_cmd *cmd = app_gapm_cancel_msg_create();
    // Send the message
    app_gapm_cancel_msg_send(cmd);
	}
	else  // Regular command
	{
  	// Remove carriage return at end of command 
  	rx_buffer[bytes -1] =0x00;
	
	  // Validated commands targeted at remote peer? 
	  if(rx_buffer[2] =='r' || rx_buffer[3] == 'r' )
	  {
  		if(ke_state_get(TASK_APP) == APP_CONNECTED)
		  {
  			codeless_env.command_route = CMD_FROM_LOCAL_RESP_PEER;
			  send_to_peer((char*)rx_buffer);
		  }		
		  else
  			arch_puts("\n\rERROR\n\r");
  	}
		else if(rx_buffer[0] =='|')
	  {
  		if(ke_state_get(TASK_APP) == APP_CONNECTED)
		  {
  			codeless_env.command_route = CMD_ROUTE_UNDEFINED;
			  send_to_peer((char*)rx_buffer);
				arch_puts("\r\n");
		  }		
		  else
  			user_reply("",false);
  	}
  	else
  	{
	    // Else interpret locally
		  codeless_env.command_route = CMD_FROM_LOCAL_RESP_LOCAL;	
      memcpy(&codeless_env.current_cmd, &rx_buffer, bytes);		
	    user_cmd_interpreter(bytes);
	  }
	}
	// Prepare for next AT command
	uart2_read(rx_buffer, CMD_BUFFER_SIZE, user_uart_cmd_received_callback, codeless_env.echo);
}
//


//--------------------------------------------------------------------------------------------------------/
//     _____                    _         _____                                           _     
//    |  ___|                  | |       /  __ \                                         | |    
//    | |____  _____  ___ _   _| |_ ___  | /  \/ ___  _ __ ___  _ __ ___   __ _ _ __   __| |___ 
//    |  __\ \/ / _ \/ __| | | | __/ _ \ | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` / __|
//    | |___>  <  __/ (__| |_| | ||  __/ | \__/\ (_) | | | | | | | | | | | (_| | | | | (_| \__ \
//    \____/_/\_\___|\___|\__,_|\__\___|  \____/\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|___/
// Executesa list of semicolon separated commands                                                                                                                                                                                           
void user_execute_commands(char* cmd)
{
	uint8_t len_cmd = strlen(cmd);
	uint8_t idx = 0;
	uint8_t ptr = 0;
	uint8_t tmp_cmd[CMD_BUFFER_SIZE];
	memset(tmp_cmd,0,CMD_BUFFER_SIZE);
	
	// Iterate through entire command string 
	while (true) 
	{
  	// Last byte or separator?
		if((cmd[idx] == 0x00) || (cmd[idx] == ';'))
		{
			// We have a full command to interpret
			memcpy(&rx_buffer,&tmp_cmd,ptr);
	    user_uart_cmd_received_callback(0, 1 + ptr);
		  
			// Prepare fornext command
			ptr = 0;
			memset(tmp_cmd,0,100);
	  }
		else
		{
			// Record one byt
			tmp_cmd[ptr++] = cmd[idx];
		}			
		// End of commands?
		if(cmd[idx] == 0x00)
			break;
		// Next 
		idx++;
	}
}
//


//--------------------------------------------------------------------------------------------------------/
//      ____ _______   ______               _     _                     _ _           
//     |  _ \__   __| |  ____|             | |   | |                   | | |          
//     | |_) | | |    | |____   _____ _ __ | |_  | |__   __ _ _ __   __| | | ___ _ __ 
//     |  _ <  | |    |  __\ \ / / _ \ '_ \| __| | '_ \ / _` | '_ \ / _` | |/ _ \ '__|
//     | |_) | | |    | |___\ V /  __/ | | | |_  | | | | (_| | | | | (_| | |  __/ |   
//     |____/  |_|    |______\_/ \___|_| |_|\__| |_| |_|\__,_|_| |_|\__,_|_|\___|_|                                                                                                                                            
//
// Handles events from the stack not handled elsewhere
void user_catch_rest_hndl(ke_msg_id_t const msgid, void const *param,ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
  switch(msgid)
  {  
    case CUSTS1_VAL_WRITE_IND:
    {
      // BT Smart data received via GATT write
			struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);
      switch (msg_param->handle)
      {
        // Was it a command?
				case CUST1_IDX_INBOUND_VAL:
        {
					// Clear the buffer and copy received data 
					memset(rx_buffer,0,CMD_BUFFER_SIZE);
					memcpy(&rx_buffer, &msg_param->value[0],msg_param->length);
					
					// Incoming response?
					if(codeless_env.command_route == CMD_FROM_LOCAL_RESP_PEER)
					{
						// Print response
						arch_printf("\r\n%s\r\n",rx_buffer);
						codeless_env.command_route = CMD_ROUTE_UNDEFINED;
					}
					else if(rx_buffer[0] =='|')
					{
						// Piped data from peer
						  arch_printf("%s\r\n",rx_buffer);
					}
					else
					{
						// Valid command (fast check: must contain 'r' and must start with 'AT')
						if((rx_buffer[2] =='r' || rx_buffer[3] == 'r')&&(rx_buffer[0] == 'A')&&(rx_buffer[1] == 'T') )
						{
							codeless_env.command_route = CMD_FROM_PEER_RESP_PEER;
							user_cmd_interpreter(msg_param->length+1);
						}
						else
							user_reply("",false);
					}
					break;
        }							
        default:
          break;
      }
    }
		case GATTC_DISC_CHAR_IND:
    {
		  struct gattc_disc_char_ind const* msg_param = (struct gattc_disc_char_ind const*)param;
			
			/*
			uint8_t uuid[16];
			memcpy(&uuid,&msg_param->uuid,msg_param->uuid_len);
			arch_printf(" ,Attr hdl=0x%04X, ",(uint16_t)msg_param->attr_hdl);
			arch_printf("UUID=0x%02X",uuid[msg_param->uuid_len-1]);
					
			for(uint8_t i = msg_param->uuid_len - 1 ; i>0 ;i--)
			  arch_printf("%02X",uuid[i-1]);
			
			arch_printf(" Val hdl=0x%02X, ",msg_param->pointer_hdl);
			arch_printf("Prop=0x%02X: ",msg_param->prop);
					
			if(msg_param->prop & 0x80) 
				arch_puts("Ext Prop, "); 
			if(msg_param->prop & 0x40) 
				arch_puts("Auth Signed WR, "); 
			if(msg_param->prop & 0x20) 
				arch_puts("IND, "); 
			if(msg_param->prop & 0x10) 
				arch_puts("NTF, "); 
			if(msg_param->prop & 0x08) 
				arch_puts("WR, "); 
			if(msg_param->prop & 0x04) 
			  arch_puts("WR no Resp, "); 
			if(msg_param->prop & 0x02) 
				arch_puts("RD, "); 
			if(msg_param->prop & 0x01) 
				arch_puts("Broadcast, "); 
			arch_puts("\x8\x8. \r\n");
			*/
			
			// Store CodeLess characteristics' 16bit handles in CodeLess environment  
			if(memcmp(&msg_param->uuid, codeless_env.char_uuid_inbound , 16) == 0)
				codeless_env.hdl_inbound_val = msg_param->pointer_hdl;
			if(memcmp(&msg_param->uuid, codeless_env.char_uuid_outbound , 16) == 0)
				codeless_env.hdl_outbound_val = msg_param->pointer_hdl;
			if(memcmp(&msg_param->uuid, codeless_env.char_uuid_flow_ctrl , 16) == 0)
			{
				// Store handle to client configuration of flow control characteristic
				codeless_env.hdl_flow_ctrl = msg_param->pointer_hdl; // Client configuration attribute 
				
				// Are we now linked as master to a CodeLess slave
				if(codeless_env.bt_role == GAP_CENTRAL_MST)
					codeless_env.linked_to_codeless_device = true;
				
				// Set client configuration notification flag
				struct gattc_write_cmd *req = KE_MSG_ALLOC_DYN
	      (
					GATTC_WRITE_CMD,
					KE_BUILD_ID(TASK_GATTC, 0),
					TASK_APP, 
					gattc_write_cmd,
					2
				);
				uint16_t data = 0x0001;
		    req->auto_execute = true;
				req->cursor = 0;
				req->handle = codeless_env.hdl_flow_ctrl +1 ; // Client configuration attribute
				req->length = 2;
				req->offset = 0;
				req->req_type = GATTC_WRITE;
				memcpy(&req->value,&data,2);
				ke_msg_send(req);
			}	
    }			
    break;
		case GATTC_CMP_EVT:
		{
			if(codeless_env.gap_activity == DISCOVERING)
			{
				codeless_env.gap_activity = IDLE;
				arch_puts("\n\rOK\n\r");
			}
		}
		break;
		case GATTC_EVENT_IND:
		{
		  struct gattc_event_ind const* msg_param = (struct gattc_event_ind const*)param;
			
			//arch_puts("Notification data:\r\n ");
			//arch_printf("hdl = 0x%04X, data: ",msg_param->handle);
			//for(uint8_t i = 0; i<msg_param->length;i++)
			//	arch_printf("0x%02X ",msg_param->value[i]);
			//arch_puts("\r\n");
  		// Only notifications from the flow-control handle are accepted
			
			if((msg_param->handle == codeless_env.hdl_flow_ctrl )&&(msg_param->type == GATTC_NOTIFY))
			{		
			  struct gattc_read_cmd *req = KE_MSG_ALLOC_DYN
			  (
  				GATTC_READ_CMD,
				  KE_BUILD_ID(TASK_GATTC, 0),
				  TASK_APP, 
				  gattc_read_cmd,
				  2
			  );
			  req->nb = 1;
			  req->req_type = GATTC_READ;
			  req->req.simple.offset = 0;
			  req->req.simple.length = 0;
			  req->req.simple.handle = codeless_env.hdl_outbound_val;			
			  ke_msg_send(req);		
			}
		}	
	  break;
			
		case GATTC_READ_IND:
		{
			
			struct gattc_read_ind const* msg = (struct gattc_read_ind const*)param;
			//char tempStr[25]= {0};
			//memcpy(&tempStr, &msg->value, msg->length);
			//arch_printf("Read Response:\"%s\" from handle: 0x%04X\r\n",tempStr,msg->handle);
			// Valid command (fast check: must contain 'r' and must start with 'AT')
			memset(rx_buffer,0,CMD_BUFFER_SIZE);
			memcpy(&rx_buffer, &msg->value[0], msg->length);			
			if((rx_buffer[2] =='r' || rx_buffer[3] == 'r')&&(rx_buffer[0] == 'A')&&(rx_buffer[1] == 'T') )
			{
				codeless_env.command_route = CMD_FROM_PEER_RESP_PEER;
				user_cmd_interpreter(msg->length+1);
			}
			else
			{
				// This was a reply arriving from the peer so print to serial port console
				arch_printf("\r\n%s\r\n",(char*)rx_buffer);
			}
		}
		break;
  }  
}
//

//--------------------------------------------------------------------------------------------------------/                                                   |_|                                                          
//      ___      _        ______                      _    ______              _               _ 
//     / _ \    | |       | ___ \                    | |   | ___ \            (_)             | |
//    / /_\ \ __| |_   __ | |_/ /___ _ __   ___  _ __| |_  | |_/ /___  ___ ___ ___   _____  __| |
//    |  _  |/ _` \ \ / / |    // _ \ '_ \ / _ \| '__| __| |    // _ \/ __/ _ \ \ \ / / _ \/ _` |
//    | | | | (_| |\ V /  | |\ \  __/ |_) | (_) | |  | |_  | |\ \  __/ (_|  __/ |\ V /  __/ (_| |
//    \_| |_/\__,_| \_(_) \_| \_\___| .__/ \___/|_|   \__| \_| \_\___|\___\___|_| \_/ \___|\__,_|
//                                  | |                                                          
//                                  |_|                                                          
// Display advertising reports    
void user_on_adv_report_ind(struct gapm_adv_report_ind const * param)
{
  // Dump Scan report to UART  
	
  char address_type = 'R';
	if (param->report.adv_addr_type == 0) 
	  address_type = 'P';
		
	char codeless_service = ' ';
		
	if(param->report.data_len > 20) // Must have at least 21 bytes to be a CodeLess device
	{
    if(memcmp(&param->report.data[5], codeless_env.service_uuid, 16) == 0)
	    codeless_service = '*';
	}
		
	arch_printf
	(
	  "(%c) %02X:%02X:%02X:%02X:%02X:%02X,%c, Type: 0x%02X, RSSI:%d\r\n",
		codeless_service,          
	  param->report.adv_addr.addr[5],param->report.adv_addr.addr[4],
		param->report.adv_addr.addr[3],param->report.adv_addr.addr[2],
		param->report.adv_addr.addr[1],param->report.adv_addr.addr[0],
	  address_type, 
	  param->report.evt_type,
	  ((47*param->report.rssi)/100 - 112)
	);
  
}
//


//--------------------------------------------------------------------------------------------------------/
//     _____                   _____                       _      _           _ 
//    /  ___|                 /  __ \                     | |    | |         | |
//    \ `--.  ___ __ _ _ __   | /  \/ ___  _ __ ___  _ __ | | ___| |_ ___  __| |
//     `--. \/ __/ _` | '_ \  | |    / _ \| '_ ` _ \| '_ \| |/ _ \ __/ _ \/ _` |
//    /\__/ / (_| (_| | | | | | \__/\ (_) | | | | | | |_) | |  __/ ||  __/ (_| |
//    \____/ \___\__,_|_| |_|  \____/\___/|_| |_| |_| .__/|_|\___|\__\___|\__,_|
//                                                  | |                         
//                                                  |_|                         
// Bluetooth scan was cancelled or has completed    
void user_scan_completed(uint8_t status)
{
	arch_puts("Scan Completed...\r\nOK\r\n");
	codeless_env.gap_activity = IDLE;
}
//


//--------------------------------------------------------------------------------------------------------/
//     _    _       _                    
//    | |  | |     | |                   
//    | |  | | __ _| | _____ _   _ _ __  
//    | |/\| |/ _` | |/ / _ \ | | | '_ \ 
//    \  /\  / (_| |   <  __/ |_| | |_) |
//     \/  \/ \__,_|_|\_\___|\__,_| .__/ 
//                                | |    
//                                |_|    
// Called when device wakes up from Sleep   
void user_wakeup()
{
  // Disable interrupt
	wkupct_disable_irq();
	
	// If the peripheral domain is powered off, intitialize domain.
	if(!(GetWord16(SYS_STAT_REG) & PER_IS_UP))
		periph_init();
	
	// Prepare for first AT command via UART
	uart2_read(rx_buffer,CMD_BUFFER_SIZE,user_uart_cmd_received_callback,codeless_env.echo);
	
	// Disable sleep
	arch_disable_sleep(); 
	
	// Freeze the watchdog 
	wdg_freeze();

}
//

//--------------------------------------------------------------------------------------------------------/
//    ______                                _____ _                 
//    | ___ \                              /  ___| |                
//    | |_/ / __ ___ _ __   __ _ _ __ ___  \ `--.| | ___  ___ _ __  
//    |  __/ '__/ _ \ '_ \ / _` | '__/ _ \  `--. \ |/ _ \/ _ \ '_ \ 
//    | |  | | |  __/ |_) | (_| | | |  __/ /\__/ / |  __/  __/ |_) |
//    \_|  |_|  \___| .__/ \__,_|_|  \___| \____/|_|\___|\___| .__/ 
//                  | |                                      | |    
//                  |_|                                      |_|    
// Prepare to sleep. Set wakeup on UART RX activity (low transition)   
void user_prepare_sleep()
{
  // Set wakeup on UART RX
	wkupct_register_callback(user_wakeup);
	
	// Specify the IO pin, the polarity, the event count and the debouncing time for the counter IRQ
	wkupct_enable_irq
  (
    WKUPCT_PIN_SELECT(UART2_RX_GPIO_PORT, UART2_RX_GPIO_PIN),   // UART RX 
	  WKUPCT_PIN_POLARITY(UART2_RX_GPIO_PORT, UART2_RX_GPIO_PIN, WKUPCT_PIN_POLARITY_LOW), // polarity low
    1,  // Number of events
    0  // debouncing time = 0ms
  );
	
  arch_set_extended_sleep();  
}
//


//--------------------------------------------------------------------------------------------------------/
//    ______           _            _____  __          _____                       _      _       
//    |  _  \         (_)          /  __ \/ _|        /  __ \                     | |    | |      
//    | | | |_____   ___  ___ ___  | /  \/ |_ __ _    | /  \/ ___  _ __ ___  _ __ | | ___| |_ ___ 
//    | | | / _ \ \ / / |/ __/ _ \ | |   |  _/ _` |   | |    / _ \| '_ ` _ \| '_ \| |/ _ \ __/ _ \
//    | |/ /  __/\ V /| | (_|  __/ | \__/\ || (_| |_  | \__/\ (_) | | | | | | |_) | |  __/ ||  __/
//    |___/ \___| \_/ |_|\___\___|  \____/_| \__, (_)  \____/\___/|_| |_| |_| .__/|_|\___|\__\___|
//                                            __/ |                         | |                   
//                                           |___/                          |_|                      
// This event is triggered after the stack has been successfully initialized 
void user_on_dev_config_complete()
{
	
	// Disable sleep
	arch_disable_sleep();
	
	// Freeze the watchdog
	wdg_freeze();
		
	if(codeless_env.bt_role == GAP_NO_ROLE)
	{
	  		   
	  // Set all dafaults
	  user_codeless_init_system();
	
	  // Indicate the status via serial port
  	arch_puts("\r\nREADY\r\n");
	
		// Call the default handler for this event
	  default_app_on_set_dev_config_complete();
		
		// Start a timer to enter sleep 10 seconds after bootup, unless a '@' or <CR> character is received via UART
	  sleep_timer = app_easy_timer(1000, user_prepare_sleep);
	
		// Store the Bluetooth role 
		codeless_env.bt_role = GAP_PERIPHERAL_SLV;
		
	  // Prepare for first AT command via UART
	  uart2_read(rx_buffer,CMD_BUFFER_SIZE,user_uart_cmd_received_callback,codeless_env.echo);
	}
	else 
	{
		// Call the default handler for this event
	  app_db_init_start();
		codeless_env.gap_activity = IDLE;
		arch_puts("\r\nOK\r\n");
	}
}
// 


//--------------------------------------------------------------------------------------------------------/
//     _____      _ _   _       _ _          _   _             
//    |_   _|    (_) | (_)     | (_)        | | (_)            
//      | | _ __  _| |_ _  __ _| |_ ______ _| |_ _  ___  _ __  
//      | || '_ \| | __| |/ _` | | |_  / _` | __| |/ _ \| '_ \ 
//     _| || | | | | |_| | (_| | | |/ / (_| | |_| | (_) | | | |
//     \___/_| |_|_|\__|_|\__,_|_|_/___\__,_|\__|_|\___/|_| |_|
// Initialization, runs only once                                                             
void user_app_on_init(void)
{
	default_app_on_init();
	
	// Set intial Bluetooth role
	codeless_env.bt_role = GAP_NO_ROLE;
	
	// Not currently connected to a CodeLess device as master 
	codeless_env.linked_to_codeless_device = false;
	
	// Generate a Random Bluetooth Device Address (non-IEEE)
	codeless_env.device_address = generate_random_bd_addr();
	
	// Store the CodeLess service UUID in the environment
	uint8_t tmp_uuid[16] = DEF_CUST1_SVC_UUID_128;
	memcpy(&codeless_env.service_uuid, &tmp_uuid, 16);
		
	// Store the CodeLess characteristic UUIDs in the environment 
	uint8_t uuida[16] = DEF_CUST1_INBOUND_UUID_128;
	memcpy(&codeless_env.char_uuid_inbound, &uuida, 16);
	uint8_t uuidb[16] = DEF_CUST1_OUTBOUND_UUID_128;
	memcpy(&codeless_env.char_uuid_outbound, &uuidb, 16);
	uint8_t uuidc[16] = DEF_CUST1_FLOW_CONTROL_UUID_128;
	memcpy(&codeless_env.char_uuid_flow_ctrl, &uuidc, 16);

  // Set the application state
  codeless_env.gap_activity = IDLE;
	codeless_env.adv_intv = user_adv_conf.intv_min;
	
	codeless_env.suppress_response = false;
	codeless_env.echo = true;
	
	// Specify the advertise data
	uint8_t advertisement_data[] = 
  { 
	  // Complete list of 128bit service UUIDs
		17, GAP_AD_TYPE_COMPLETE_LIST_128_BIT_UUID,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	  
		// Bluetooth Device Name
		6 , GAP_AD_TYPE_COMPLETE_NAME,               'S', 'p', 'o', 'r', 'a'
	};
	
	// Insert the CodeLess service uuid into advertisement data (must reverse order):
	for(uint8_t i = 0;i<16;i++)
	  memcpy(&advertisement_data[2+i], &codeless_env.service_uuid[15-i], 1);
	
	memcpy(&codeless_env.service_uuid,&advertisement_data[2],16);
	
	memcpy(&codeless_env.adv_data,&advertisement_data,sizeof(advertisement_data));
	codeless_env.adv_data_len = sizeof(advertisement_data);
	
	// No scan response data 
	codeless_env.resp_data_len = 0;
}
//


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

/// @} APP
