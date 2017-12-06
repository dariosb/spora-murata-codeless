/**
 ****************************************************************************************
 *
 * @file user_gap.c
 *
 * @brief user_gap source file.
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

#include "user_gap.h"

#include "custs1_task.h"
#include "user_custs_config.h"
#include "user_custs1_def.h"

#include "user_cmd_interpreter.h"

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
void user_app_configuration_func(uint8_t role)
{
    struct gapm_set_dev_config_cmd* cmd = KE_MSG_ALLOC
		(
		  GAPM_SET_DEV_CONFIG_CMD,
      TASK_GAPM, TASK_APP,
		  gapm_set_dev_config_cmd
		);

    // Set device configuration
    cmd->operation = GAPM_SET_DEV_CONFIG;

    // Set role
    cmd->role = role;
    
		// Also retain the role in the environment
		codeless_env.bt_role = role;
    
		memset( cmd->irk.key, 0, sizeof(struct gap_sec_key));
    cmd->appearance = 0;
    cmd->appearance_write_perm = GAPM_WRITE_DISABLE;
    cmd->name_write_perm = GAPM_WRITE_DISABLE;

    ke_msg_send(cmd);

    return;
}
//

//     _____     _        _     _ _     _        ____                            _   _             
//    | ____|___| |_ __ _| |__ | (_)___| |__    / ___|___  _ __  _ __   ___  ___| |_(_) ___  _ __  
//    |  _| / __| __/ _` | '_ \| | / __| '_ \  | |   / _ \| '_ \| '_ \ / _ \/ __| __| |/ _ \| '_ \ 
//    | |___\__ \ || (_| | |_) | | \__ \ | | | | |__| (_) | | | | | | |  __/ (__| |_| | (_) | | | |
//    |_____|___/\__\__,_|_.__/|_|_|___/_| |_|  \____\___/|_| |_|_| |_|\___|\___|\__|_|\___/|_| |_|
//                                                                                                 
// We use this function to initiate a connection with a peripheral
void user_connect_to_peripheral(uint8_t peer_addr_type, uint8_t* addr, uint16_t intv)
{
	// Construct and send a connect message
  struct gapm_start_connection_cmd *msg;
  msg = (struct gapm_start_connection_cmd *) KE_MSG_ALLOC_DYN
  (
    GAPM_START_CONNECTION_CMD,
    TASK_GAPM,
    TASK_APP, 
    gapm_start_connection_cmd,
    sizeof(struct gap_bdaddr)
  );
  msg->nb_peers = 1;
  memcpy((void *) &msg->peers[0].addr, (void *)addr, BD_ADDR_LEN);
  msg->peers[0].addr_type = peer_addr_type;
  msg->con_intv_max = intv;
  msg->con_intv_min = intv; 
  msg->ce_len_min = 0x45;
  msg->ce_len_max = 0x45;
  msg->con_latency = 0;
  msg->op.addr_src = 0;
  msg->superv_to = 300;// 300 -> 3000 ms ;
  msg->scan_interval = 0x1800;
  msg->scan_window = 0x1700;
  msg->op.code = GAPM_CONNECTION_DIRECT;
  ke_msg_send((void *) msg);  
		
	ke_state_set(TASK_APP,APP_CONNECTABLE);
}


//     ____  _             _     ____                  
//    / ___|| |_ __ _ _ __| |_  / ___|  ___ __ _ _ __  
//    \___ \| __/ _` | '__| __| \___ \ / __/ _` | '_ \ 
//     ___) | || (_| | |  | |_   ___) | (_| (_| | | | |
//    |____/ \__\__,_|_|   \__| |____/ \___\__,_|_| |_|
//                                                     
void user_scan_start(void)
{
  // Comstruct and send a command to start scanning   
  struct gapm_start_scan_cmd* cmd = KE_MSG_ALLOC
  (
    GAPM_START_SCAN_CMD,
    TASK_GAPM,
    TASK_APP,
    gapm_start_scan_cmd
  );
  cmd->op.code = user_scan_conf.code;
  cmd->op.addr_src = user_scan_conf.addr_src;
  cmd->interval = user_scan_conf.interval;
  cmd->window = user_scan_conf.window;
  cmd->mode = user_scan_conf.mode;
  cmd->filt_policy = user_scan_conf.filt_policy;
  cmd->filter_duplic = user_scan_conf.filter_duplic;

    
  // Send the message
  ke_msg_send(cmd);
}
//


//     ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ 
//    |______|______|______|______|______|______|______|______|______|______|______|______|______|______|______|
//                                                                                                              

/// @} APP
