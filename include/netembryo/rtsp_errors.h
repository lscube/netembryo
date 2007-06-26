/* * 
 *  $Id$
 *  
 *  This file is part of NetEmbryo 
 *
 * NetEmbryo -- default network wrapper 
 *
 *  Copyright (C) 2007 by team@streaming.polito.it
 *
 *  NetEmbryo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NetEmbryo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NetEmbryo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

#ifndef _NETEMBRYO_RTSP_ERRORS_H_
#define _NETEMBRYO_RTSP_ERRORS_H_

#define RTSP_MAX_REPLY_MESSAGE_LEN 256

/** 
 * RTSP Error Notification data and functions
 * @defgroup rtsp_error RTSP Error Notification
 * @{
 */

/**
  * RTSP reply message
  */
typedef struct
{
    int reply_code; //!< RTSP code representation of the message
    char reply_str[RTSP_MAX_REPLY_MESSAGE_LEN]; //!< written representation of the message
} ReplyMessage;

/**
  * RTSP error description
  */
typedef struct
{
    ReplyMessage message; //!< RTSP standard error message
    int got_error; //!< can be: FALSE no error, TRUE generic error or have internal error id
} RTSP_Error;


extern RTSP_Error const RTSP_Ok;
extern RTSP_Error const RTSP_BadRequest;
extern RTSP_Error const RTSP_InternalServerError;
extern RTSP_Error const RTSP_Forbidden;
extern RTSP_Error const RTSP_OptionNotSupported;
extern RTSP_Error const RTSP_NotFound;
extern RTSP_Error const RTSP_SessionNotFound;
extern RTSP_Error const RTSP_InvalidRange;
extern RTSP_Error const RTSP_Fatal_ErrAlloc;

void set_RTSP_Error(RTSP_Error * err, int reply_code, char * message);
RTSP_Error const * get_RTSP_Error(int reply_code);
/**
 * @}
 */

#endif

