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

#include "protocol_replies.h"
#include <string.h>
#include <assert.h>

const ProtocolReply RTSP_Continue = { 100, false, "Continue" };
const ProtocolReply RTSP_Ok = { 200, false, "OK" };
const ProtocolReply RTSP_Created = { 201, false, "Created" };
const ProtocolReply RTSP_Accepted = { 202, false, "Accepted" };
const ProtocolReply RTSP_BadRequest = { 400, true, "Bad Request" };
const ProtocolReply RTSP_Forbidden = { 403, true, "Forbidden" };
const ProtocolReply RTSP_NotFound = { 404, true, "Not Found" };
const ProtocolReply RTSP_NotAcceptable = { 406, true, "Not Acceptable" };
const ProtocolReply RTSP_UnsupportedMedia = { 415, true, "Unsupported Media Type" };
const ProtocolReply RTSP_ParameterNotUnderstood = { 451, true, "Parameter Not Understood" };
const ProtocolReply RTSP_NotEnoughBandwidth = { 453, true, "Not Enough Bandwith" };
const ProtocolReply RTSP_SessionNotFound = { 454, true, "Session Not Found" };
const ProtocolReply RTSP_InvalidMethodInState = { 455, true, "Method Not Valid In This State" };
const ProtocolReply RTSP_HeaderFieldNotValidforResource = { 456, true, "Header Field Not Valid for Resource" };
const ProtocolReply RTSP_InvalidRange = { 457, true, "Invalid Range" };
const ProtocolReply RTSP_UnsupportedTransport = { 461, true, "Unsupported Transport" };
const ProtocolReply RTSP_InternalServerError = { 500, true, "Internal Server Error" };
const ProtocolReply RTSP_NotImplemented = { 501, true, "Not Implemented" };
const ProtocolReply RTSP_ServiceUnavailable = { 503, true, "Service Unavailable" };
const ProtocolReply RTSP_VersionNotSupported = { 505, true, "RTSP Version Not Supported" };
const ProtocolReply RTSP_OptionNotSupported = { 551, true, "Option not supported" };
