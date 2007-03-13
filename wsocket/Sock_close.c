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

int Sock_close(Sock *s)
{
	int res;

	if (!s)
		return -1;

	if(s->flags & IS_MULTICAST) {
		if (s->remote_host)
			mcast_leave(s->fd,(struct sockaddr *) &(s->remote_stg));
		else
			mcast_leave(s->fd,(struct sockaddr *) &(s->local_stg));
	}

#if HAVE_SSL
	if(s->flags & USE_SSL)
		sock_SSL_close(s->ssl);
#endif

	res = sock_close(s->fd);

	if (s->remote_host) free(s->remote_host);
	if (s->local_host) free(s->local_host);
    free(s);

	return res;
}
