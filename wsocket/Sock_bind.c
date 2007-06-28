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

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <netembryo/wsocket.h>

Sock * Sock_bind(char *host, char *port, sock_type socktype, sock_flags ssl_flag)
{

	Sock *s = NULL;
	int sockfd = -1;
	struct sockaddr *sa_p;
	socklen_t sa_len;
	char local_host[128];
	int32_t local_port;

#if HAVE_SSL
	if ((ssl_flag & USE_SSL) {
		if(socktype != TCP) {
			net_log(NET_LOG_ERR, "SSL can't work on this protocol.\n");
			return NULL;
		}
	}
#endif

	if (sock_bind(host, port, &sockfd, socktype)) {
		net_log(NET_LOG_ERR, "Error in low level sock_bind().\n");
		return NULL;
	}

    if (!(s = calloc(1, sizeof(Sock)))) {
		net_log(NET_LOG_FATAL, "Unable to allocate a Sock struct in Sock_bind().\n");
		sock_close(sockfd);
		return NULL;
	}

	s->fd = sockfd;
	s->socktype = socktype;

	s->flags = ssl_flag;

	sa_p = (struct sockaddr *)&(s->local_stg);
	sa_len = sizeof(struct sockaddr_storage);

	if(getsockname(s->fd, sa_p, &sa_len) < 0) {
		Sock_close(s);
		return NULL;
	}

	if(!sock_ntop_host(sa_p, local_host, sizeof(local_host)))
		memset(local_host, 0, sizeof(local_host));

    if (!(s->local_host = strdup(local_host))) {
        net_log(NET_LOG_FATAL, "Unable to allocate local host in Sock_bind().\n");
        Sock_close(s);
        return NULL;
    }

	local_port = sock_get_port(sa_p);

	if(local_port < 0) {
		net_log(NET_LOG_ERR, "Unable to get local port in Sock_bind().\n");
		Sock_close(s);
		return NULL;
	} else
		s->local_port = ntohs(local_port);

	net_log(NET_LOG_DEBUG, "Socket bound with addr=\"%s\" and port=\"%u\".\n", s->local_host, s->local_port);

	if(is_multicast_address(sa_p, s->local_stg.ss_family)) {
		if(mcast_join(s->fd, sa_p, NULL, 0, &(s->addr) )) {
			Sock_close(s);
			return NULL;
		}
		s->flags |= IS_MULTICAST;
	}

	return s;
}
