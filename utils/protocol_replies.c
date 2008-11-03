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

const ProtocolReply RTSP_Continue = { 100, false, false, "Continue" };
const ProtocolReply RTSP_Ok = { 200, false, false, "OK" };
const ProtocolReply RTSP_Created = { 201, false, false, "Created" };
const ProtocolReply RTSP_Accepted = { 202, false, false, "Accepted" };
const ProtocolReply RTSP_BadRequest = { 400, true, false, "Bad Request" };
const ProtocolReply RTSP_Forbidden = { 403, true, false, "Forbidden" };
const ProtocolReply RTSP_NotFound = { 404, true, false, "Not Found" };
const ProtocolReply RTSP_NotAcceptable = { 406, true, false, "Not Acceptable" };
const ProtocolReply RTSP_UnsupportedMedia = { 415, true, false, "Unsupported Media Type" };
const ProtocolReply RTSP_ParameterNotUnderstood = { 415, true, false, "Parameter Not Understood" };
const ProtocolReply RTSP_NotEnoughBandwidth = { 453, true, false, "Not Enough Bandwith" };
const ProtocolReply RTSP_SessionNotFound = { 454, true, false, "Session Not Found" };
const ProtocolReply RTSP_InvalidMethodInState = { 455, true, false, "Method Not Valid In This State" };
const ProtocolReply RTSP_HeaderFieldNotValidforResource = { 456, true, false, "Header Field Not Valid for Resource" };
const ProtocolReply RTSP_InvalidRange = { 457, true, false, "Invalid Range" };
const ProtocolReply RTSP_UnsupportedTransport = { 461, true, false, "Unsupported Transport" };
const ProtocolReply RTSP_InternalServerError = { 500, true, false, "Internal Server Error" };
const ProtocolReply RTSP_NotImplemented = { 501, true, false, "Not Implemented" };
const ProtocolReply RTSP_ServiceUnavailable = { 503, true, false, "Service Unavailable" };
const ProtocolReply RTSP_VersionNotSupported = { 505, true, false, "RTSP Version Not Supported" };
const ProtocolReply RTSP_OptionNotSupported = { 551, true, false, "Option not supported" };

/**
 * @brief Build a custom reply structure
 * @param code The code for the reply (has to be less than 
 * @param error True if the reply is an error reply
 * @param message The message to assign to the new reply
 *
 * Note that all replies should then pass through @ref reply_free to ensure the
 * dynamically allocated strings are properly freed afterward.
 */
ProtocolReply reply_build_custom(uint16_t code, bool error,
                                 const char *message)
{
    /* Thanks to C99 this is very very quick, but still we might want to have
     * further sanity checks for the reply, like ensuring the code is not over
     * the value of 999, at least in debug.
     */
    ProtocolReply reply = {
        .code = code,
        .error = error,
        .custom = true,
        .message = strdup(message ? message : "")
    };

    /* Ensure three-digits code */
    assert(100 >= code && code <= 999);

    if ( code < 100 || code > 999 ) {
        /** @todo Here we should log an error just for safety */
        reply.code = 500;
    }

    return reply;
}

/**
 * @brief Retrieves a standard RTSP reply given its code.
 * @param code The code of the RTSP reply to return.
 * @return A ProtocolReply structure for the given standard code reply.
 *
 * This function will abort if an unknown code is provided, for now.
 */
ProtocolReply reply_get_rtsp(uint16_t code)
{
    switch (code)
    {
        case 100:
            return RTSP_Continue;
        case 200:
            return RTSP_Ok;
        case 201:
            return RTSP_Created;
        case 202:
            return RTSP_Accepted;
        case 400:
            return RTSP_BadRequest;
        case 403:
            return RTSP_Forbidden;
        case 404:
            return RTSP_NotFound;
        case 406:
            return RTSP_NotAcceptable;
        case 415:
            return RTSP_UnsupportedMedia;
        case 451:
            return RTSP_ParameterNotUnderstood;
        case 453:
            return RTSP_NotEnoughBandwidth;
        case 454:
            return RTSP_SessionNotFound;
        case 455:
            return RTSP_InvalidMethodInState;
        case 456:
            return RTSP_HeaderFieldNotValidforResource;
        case 457:
            return RTSP_InvalidRange;
        case 461:
            return RTSP_UnsupportedTransport;
        case 500:
            return RTSP_InternalServerError;
        case 501:
            return RTSP_NotImplemented;
        case 503:
            return RTSP_ServiceUnavailable;
        case 505:
            return RTSP_VersionNotSupported;
        case 551:
            return RTSP_OptionNotSupported;
        default:
            /* Since we don't know this error code, return an ISE instead.
             * But since this is an error condition, assert out so
             * that we kill ourselves during debug instead.
             */
            assert(false);
            return RTSP_InternalServerError;
    }
}

/**
 * @brief Free a reply, only for custom replies.
 * @param reply Reply to free up.
 *
 * This function is called to ensure that the memory allocated for the custom
 * replies is freed. Default replies have no allocated memory so don't need to
 * be freed at all.
 */
void reply_free(ProtocolReply reply)
{
    /* The cast here is good because we allocated it if it's custom! */
    if ( reply.custom )
        free((char*)reply.message);
}
