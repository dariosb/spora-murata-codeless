/**
 ****************************************************************************************
 *
 * @file user_gatt.c
 *
 * @brief user_gatt source file.
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

#include "user_gatt.h"

#include "custs1_task.h"
#include "user_custs_config.h"
#include "user_custs1_def.h"

#include "user_cmd_interpreter.h"


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
void send_to_peer(char* str_to_send)
{
  
	// Write response string to Bluetooth peer  
	if(codeless_env.linked_to_codeless_device)
	{
		struct gattc_write_cmd *req = KE_MSG_ALLOC_DYN
	  (
			GATTC_WRITE_CMD,
			KE_BUILD_ID(TASK_GATTC, 0),
			TASK_APP, 
			gattc_write_cmd,
			strlen(str_to_send)
		);
		req->auto_execute = true;
		req->cursor = 0;
		req->handle = codeless_env.hdl_inbound_val;
		req->length = strlen(str_to_send);
		req->offset = 0;
		req->req_type = GATTC_WRITE;
		memcpy(&req->value, str_to_send, strlen(str_to_send));
		ke_msg_send(req);
	}
	else
	{
	  struct custs1_val_set_req* req = KE_MSG_ALLOC_DYN
	  (
  		CUSTS1_VAL_SET_REQ,
      TASK_CUSTS1,
      TASK_APP,
      custs1_val_set_req,
      strlen(str_to_send)
	  );
    req->conhdl = app_env->conhdl;
    req->handle = CUST1_IDX_OUTBOUND_VAL;
    
  	req->length = strlen(str_to_send);
    memcpy(req->value, str_to_send, strlen(str_to_send));
    ke_msg_send(req);
  	
    if(codeless_env.bt_role != GAP_CENTRAL_MST)
  	{		
    // Notify Bluetooth peer
	    struct custs1_val_ntf_req* req2 = KE_MSG_ALLOC_DYN
	    (
    	  CUSTS1_VAL_NTF_REQ,
	      TASK_CUSTS1,
		    TASK_APP,
		    custs1_val_ntf_req,
		    1
	    );
	    req2->conhdl = app_env->conhdl;
	    req2->handle = CUST1_IDX_FLOW_CONTROL_VAL;
	    req2->length = 1;
	    req2->value[0] = 0x01;
	    ke_msg_send(req2);
    }
	}		
}
//


//    ______ _                               _____ _                          _            _     _   _          
//    |  _  (_)                             /  __ \ |                        | |          (_)   | | (_)         
//    | | | |_ ___  ___ _____   _____ _ __  | /  \/ |__   __ _ _ __ __ _  ___| |_ ___ _ __ _ ___| |_ _  ___ ___ 
//    | | | | / __|/ __/ _ \ \ / / _ \ '__| | |   | '_ \ / _` | '__/ _` |/ __| __/ _ \ '__| / __| __| |/ __/ __|
//    | |/ /| \__ \ (_| (_) \ V /  __/ |    | \__/\ | | | (_| | | | (_| | (__| ||  __/ |  | \__ \ |_| | (__\__ \
//    |___/ |_|___/\___\___/ \_/ \___|_|     \____/_| |_|\__,_|_|  \__,_|\___|\__\___|_|  |_|___/\__|_|\___|___/
// Performs a GATT characteristics discovery   
void user_gatt_discover_chars()
{
	struct gattc_disc_cmd *req = KE_MSG_ALLOC_DYN
			(
			  GATTC_DISC_CMD,
			  KE_BUILD_ID(TASK_GATTC,0),
			  TASK_APP,
			  gattc_disc_cmd,
			  2
			);
	req->req_type = GATTC_DISC_ALL_CHAR;
	memset(&req->uuid,0,2);
	
	req->uuid_len = 2;
	req->start_hdl = 0x0001;
	req->end_hdl = 0x1FFF;
	ke_msg_send(req);
}
//


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

/// @} APP
