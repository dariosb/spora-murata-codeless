/**
 ****************************************************************************************
 *
 * @file app_bass_task.h
 *
 * @brief Battery server application task header file.
 *
 * Copyright (C) 2012. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */

#ifndef APP_BASS_TASK_H_
#define APP_BASS_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Battery Service Application.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwble_config.h"

#if (BLE_BAS_SERVER)

#include "bass_task.h"
#include "ke_msg.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Process handler for the Application BASS messages.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance (probably unused)
 * @param[in] src_id  ID of the sending task instance
 * @param[in] msg_ret Result of the message handler
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
enum process_event_response app_bass_process_handler(ke_msg_id_t const msgid,
                                                     void const *param,
                                                     ke_task_id_t const dest_id,
                                                     ke_task_id_t const src_id,
                                                     enum ke_msg_status_tag *msg_ret);

#endif // BLE_BAS_SERVER

/// @} APP

#endif // APP_BASS_TASK_H_
