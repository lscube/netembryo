/* * 
 * * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#ifndef _NETEMBRYO_PROTOCOL_REPLIES_H_
#define _NETEMBRYO_PROTOCOL_REPLIES_H_

#include <stdint.h>
#include <stdbool.h>

/** 
 * RTSP Error Notification data and functions
 * @defgroup rtsp_error RTSP Error Notification
 * @{
 */

/**
 * @brief Structure representing a reply for clear-text protocols.
 *
 * This structure represents a reply that a server can send a client
 * as response to a method. It might be an error or it might not be.
 *
 * This data structure is not tied to RTSP, as it works as it is also
 * for HTTP.
 */
typedef struct {
  //! Three digits code for the reply
  uint16_t code;
  //! If true, this is an error reply
  bool error;
  //! Human-readable message for the reply
  const char *message;
} ProtocolReply;

/**
 * @defgroup rtsp_std_replies Standard RTSP replies
 * @{
 */
extern const ProtocolReply RTSP_Continue;
extern const ProtocolReply RTSP_Ok;
extern const ProtocolReply RTSP_Created;
extern const ProtocolReply RTSP_Accepted;
extern const ProtocolReply RTSP_BadRequest;
extern const ProtocolReply RTSP_Forbidden;
extern const ProtocolReply RTSP_NotFound;
extern const ProtocolReply RTSP_NotAcceptable;
extern const ProtocolReply RTSP_UnsupportedMedia;
extern const ProtocolReply RTSP_ParameterNotUnderstood;
extern const ProtocolReply RTSP_NotEnoughBandwidth;
extern const ProtocolReply RTSP_SessionNotFound;
extern const ProtocolReply RTSP_InvalidMethodInState;
extern const ProtocolReply RTSP_HeaderFieldNotValidforResource;
extern const ProtocolReply RTSP_InvalidRange;
extern const ProtocolReply RTSP_UnsupportedTransport;
extern const ProtocolReply RTSP_InternalServerError;
extern const ProtocolReply RTSP_NotImplemented;
extern const ProtocolReply RTSP_ServiceUnavailable;
extern const ProtocolReply RTSP_VersionNotSupported;
extern const ProtocolReply RTSP_OptionNotSupported;
/**
 * @}
 */

/**
 * @}
 */

#endif

