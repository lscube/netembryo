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

#include <netembryo/url.h>

int Url_init(Url * url, char * urlname)
{
    char * protocol_begin, * hostname_begin, * port_begin, * path_begin;
    size_t protocol_len, hostname_len, port_len, path_len;

    memset(url, 0, sizeof(Url));

    hostname_begin = strstr(urlname, "://");
    if (hostname_begin == NULL) {
        hostname_begin = urlname;
        protocol_begin = NULL;
        protocol_len = 0;
    }
    else {
        protocol_len = (size_t)(hostname_begin - urlname);
        hostname_begin = hostname_begin + 3;
        protocol_begin = urlname;
    }

    hostname_len = strlen(urlname) - ((size_t)(hostname_begin - urlname));

    path_begin = strstr(hostname_begin, "/");
    if (path_begin == NULL) {
        path_len = 0;
    }
    else {
        ++path_begin;
        hostname_len = (size_t)(path_begin - hostname_begin - 1);
        path_len = strlen(urlname) - ((size_t)(path_begin - urlname));
    }

    port_begin = strstr(hostname_begin, ":");
    if ((port_begin == NULL) || ((port_begin > path_begin) && (path_begin != NULL))) {
        port_len = 0;
        port_begin = NULL;
    }
    else {
        ++port_begin;
        if (path_len)
            port_len = (size_t)(path_begin - port_begin - 1);
        else
            port_len = strlen(urlname) - ((size_t)(port_begin - urlname));
        hostname_len = (size_t)(port_begin - hostname_begin - 1);
    }

    if (protocol_len) {
        url->protocol = (char*)malloc(protocol_len+1);
        strncpy(url->protocol, protocol_begin, protocol_len);
        url->protocol[protocol_len] = '\0';
    }

    if (port_len) {
        url->port = (char*)malloc(port_len+1);
        strncpy(url->port, port_begin, port_len);
        url->port[port_len] = '\0';
    }

    if (path_len) {
        url->path = (char*)malloc(path_len+1);
        strncpy(url->path, path_begin, path_len);
        url->path[path_len] = '\0';
    }

    url->hostname = (char*)malloc(hostname_len+1);
    strncpy(url->hostname, hostname_begin, hostname_len);
    url->hostname[hostname_len] = '\0';

    return 0;
}

void Url_destroy(Url * url)
{
    free(url->protocol);
    free(url->hostname);
    free(url->port);
    free(url->path);
}



