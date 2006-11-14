/* * 
 *  $Id$
 *  
 *  This file is part of NetEmbryo 
 *
 * NetEmbryo -- default network wrapper 
 *
 *  Copyright (C) 2005 by
 *  	
 *	- Federico Ridolfo	<federico.ridolfo@polito.it>
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
 *  this piece of code is taken from NeMeSI source code
 * */


#include <netembryo/wsocket.h>

int sock_bind(char *host, char *port, int *sock, sock_type socktype)
{
	int n;
	struct addrinfo *res, *ressave;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(struct addrinfo));

	if (host == NULL)
		hints.ai_flags = AI_PASSIVE;
	else
		hints.ai_flags = AI_CANONNAME;
#ifdef IPV6
	hints.ai_family = AF_UNSPEC;
#else
	hints.ai_family = AF_INET;
#endif

	switch (socktype) {
	case SCTP:
#ifndef HAVE_SCTP_FENICE
		fnc_log(FNC_LOG_ERR, "SCTP protocol not compiled in\n");
		return WSOCK_ERROR;
		break;
#endif	// else go down to TCP case (SCTP and TCP are both SOCK_STREAM type)
	case TCP:
		hints.ai_socktype = SOCK_STREAM;
		break;
	case UDP:
		hints.ai_socktype = SOCK_DGRAM;
		break;
	default:
		fnc_log(FNC_LOG_ERR, "Unknown socket type specified\n");
		return WSOCK_ERROR;
		break;
	}

	if ((n = gethostinfo(&res, host, port, &hints)) != 0) {
		fnc_log(FNC_LOG_ERR, "%s\n", gai_strerror(n));	
		return WSOCK_ERRADDR;
	}
	
	ressave = res;

	do {
#ifdef HAVE_SCTP_FENICE
		if (socktype == SCTP)
			res->ai_protocol = IPPROTO_SCTP;
#endif // TODO: remove this code when SCTP will be supported from getaddrinfo()
		if ((*sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
			continue;

                if (bind(*sock, res->ai_addr, res->ai_addrlen) == 0)
                        break;

		if (close(*sock) < 0)
			return WSOCK_ERROR;

	} while ((res = res->ai_next) != NULL);

	freeaddrinfo(ressave);

	if ( !res )
		return WSOCK_ERROR;

	return 0;
}
