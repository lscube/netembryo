/* * 
 * * This file is part of NetEmbryo
 *
 * Copyright (C) 2008 by LScube team <team@streaming.polito.it>
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

/**
 * @file 
 * @brief Functions to generate protocol responses (HTTP and RTSP)
 */

#ifndef _NETEMBRYO_PROTOCOL_RESPONSES_H_
#define _NETEMBRYO_PROTOCOL_RESPONSES_H_

#include <netembryo/protocol_replies.h>
#include <glib.h>

/**
 * @brief List of protocols supported by the library
 *
 * @todo This right now only allows RTSP/1.0 but this is supposed to be generic,
 * so there might be more in the future.
 */
typedef enum {
    RTSP_1_0 //!< RTSP/1.0
} Protocol;

GString *protocol_response_new(const Protocol proto, const ProtocolReply reply);

void protocol_append_header(GString *response, const char *header);
void protocol_append_header_uint(GString *response, const char *header,
                                 const guint value);

#endif
