#ifndef _NETEMBRYO_UTILS_H_
#define _NETEMBRYO_UTILS_H_

/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2007 by team@streaming.polito.it
 *
 *  NeMeSI is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NeMeSI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NeMeSI; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

#include <string.h>

/** @defgroup NetEmbryo Url Management Interface
 *
 * @brief These module offers high level functions to parse and handle URLs
 *
 * @{ */

/**
 * Saved informations about a parsed url will be stored here
 */
typedef struct
{
    char * protocol; //!< The protocol specified in the url (http, rtsp, etc)
    char * hostname; //!< The hostname specified in the url (www.something.org, 192.168.0.1, etc)
    char * port; //!< The port specified in the url
    char * path; //!< The path of the specific object to access (/path/to/resource.ext)
} Url;

int Url_init(Url * url, char * urlname);
void Url_destroy(Url * url);

/**
 * @}
 */

#endif
