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

#include "protocol_responses.h"

#include <assert.h>

/** End of line macro */
#define EL "\r\n"

/**
 * @brief Array of protocol strings for the supported protocols.
 *
 * This array contains the protocol string in the form NAME/VERSION for the
 * protocols supported by the library.
 *
 * It is indexed through the values of the Protocol enum.
 */
static const char protocols_strings[][16] = {
    [RTSP_1_0] = "RTSP/1.0"
};

/**
 * @brief Table of default response text for protocols
 *
 * This table contains the default response for a given code for each protocol
 * supported.
 *
 * It is indexed first by protocol then by three-digits response code
 *
 * @todo Provide a size-optimised alternative.
 */
static const char *const responses[][1000] = {
    [RTSP_1_0] = {
        [100] = "Continue",
        [200] = "OK",
        [201] = "Created",
        [202] = "Accepted",
        [400] = "Bad Request",
        [403] = "Forbidden",
        [404] = "Not Found",
        [406] = "Not Acceptable",
        [415] = "Unsupported Media Type",
        [451] = "Parameter Not Understood",
        [453] = "Not Enough Bandwith",
        [454] = "Session Not Found",
        [455] = "Method Not Valid In This State",
        [456] = "Header Field Not Valid for Resource",
        [457] = "Invalid Range",
        [461] = "Unsupported Transport",
        [500] = "Internal Server Error",
        [501] = "Not Implemented",
        [503] = "Service Unavailable",
        [505] = "RTSP Version Not Supported",
        [551] = "Option not supported",
        [999] = NULL
    }
};

/**
 * @brief Create a new response for a protocol with a given reply
 *
 * @param proto Protocol to write the response for.
 * @param reply Protocol reply to use for code and message.
 */
GString *protocol_response_new(const Protocol proto,
                               const guint16 code)
{
    GString *response = g_string_new("");

    /* make sure the code is valid and we don't exceed our boundaries. */
    assert(100 <= code && code <= 999);

    g_string_printf(response,
                    "%s %d %s" EL,
                    protocols_strings[proto],
                    code,
                    responses[proto][code]);

    return response;
}

/**
 * @brief Append a full header to a response
 *
 * @param response Response to append the header to
 * @param header Full header (name and value) to append
 *
 * @important Don't add the final endline!
 */
void protocol_append_header(GString *response, const char *header)
{
    g_string_append(response, header);
    g_string_append(response, EL);
}

/**
 * @brief Append an unsigned integer header to a response
 *
 * @param response Response to append the header to
 * @param header The name of the header to append
 * @param value The value of the header to append
 */
void protocol_append_header_uint(GString *response, const char *header,
                                 const guint value)
{
    g_string_append_printf(response, "%s: %u" EL,
                           header, value);
}
