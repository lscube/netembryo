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
 * */

#include <glib.h>
#include <glib/gprintf.h>
#include <netembryo/wsocket.h>
#include <netinet/in.h>
#include <netdb.h> // for getnameinfo()

char * get_remote_host(Sock *s)
{
	char str[128];
	int len = sizeof(str);

	if(s->remote_host != NULL)
		return s->remote_host;
	
	if((sock_ntop_host((struct sockaddr *)&(s->sock_stg),str,len)) != NULL)
		s->remote_host=g_strdup(str);	
		
	return s->remote_host;
}

char * get_local_host(Sock *s)
{
	char local_host[128]; /*Unix domain is largest*/

	return addr_ntop(s, local_host, sizeof(local_host));
}

inline int get_local_hostname(Sock *s, char *localhostname, size_t len) //return 0 if ok
{
	return getnameinfo((struct sockaddr *)&(s->sock_stg), sizeof(s->sock_stg), localhostname, len, NULL, 0, 0);
}

char * get_remote_port(Sock *s)
{
	int32_t port;
	
	if(s->remote_port != NULL)
		return s->remote_port;
	if((port = sock_get_port((struct sockaddr *)&(s->sock_stg))) < 0)
		return NULL;
	s->remote_port = g_strdup_printf("%d", port);

	return s->remote_port;
}

inline char * get_local_port(Sock *s)
{
	return s->local_port;
}

