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

int Sock_write(Sock *s, void *buffer, int nbytes)
{
	int n = 0;
	socklen_t from_len = sizeof(struct sockaddr_storage);

#if HAVE_SSL
	if(s->flags & USE_SSL)
		n = sock_SSL_write(s->ssl,buffer,nbytes);
	else { 
#endif		
		if(s->socktype == TCP)	
			n = sock_tcp_write(s->fd,buffer,nbytes);
		else if ( s->socktype == UDP )
			n = sock_udp_write(s->fd,buffer,nbytes, &(s->sock_stg), from_len);
#if HAVE_SSL
	}
#endif		if(s->socktype == TCP)	
	
	return n;
}
