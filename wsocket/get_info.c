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

#include <netembryo/wsocket.h>
#include <netinet/in.h>
#include <netdb.h> // for getnameinfo()

char *get_remote_host(Sock *s)
{
	char str[128];

	if(!(s->remote_host)) {
		if(!sock_ntop_host((struct sockaddr *)&(s->remote_stg),str,sizeof(str)))
			memset(str, 0, sizeof(str));
		s->remote_host = strdup(str);
	}
	return s->remote_host;
}

char *get_local_host(Sock *s)
{
	char str[128]; /*Unix domain is largest*/

	if(!(s->local_host)) {
		if(!sock_ntop_host((struct sockaddr *)&(s->local_stg),str,sizeof(str)))
			memset(str, 0, sizeof(str));
		s->local_host = strdup(str);
		}
	return s->local_host;
}

inline int get_local_hostname(char *localhostname, size_t len) //return 0 if ok
{
	//return getnameinfo((struct sockaddr *)&(s->local_stg), sizeof(s->local_stg), localhostname, len, NULL, 0, 0);
	return gethostname(localhostname, len);
}

in_port_t get_remote_port(Sock *s)
{
	int32_t port;
	if(!(s->remote_port)) {
		if((port = sock_get_port((struct sockaddr *)&(s->remote_stg))) < 0)
			return 0;
		s->remote_port = ntohs(port);
	}
	return s->remote_port;
}

in_port_t get_local_port(Sock *s)
{
	int32_t port;
	
	if(!(s->local_port)) {
		if((port = sock_get_port((struct sockaddr *)&(s->local_stg))) < 0)
			return 0;
		s->local_port = ntohs(port);
	}
	return s->local_port;
}
