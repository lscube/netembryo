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

int Sock_read(Sock *s, void *buffer, int nbytes, void *protodata)
{

socklen_t sa_len = sizeof(struct sockaddr_storage);

#if HAVE_SSL
	if(s->flags & USE_SSL)
		n = sock_SSL_read(s->ssl,buffer,nbytes);
	else {
#endif
		switch(s->socktype) {
		case UDP:
			if (!protodata) {
				return -1;
			}
			return recvfrom(s->fd, buffer, nbytes, 0,
				(struct sockaddr *) protodata, &sa_len);
			break;
		case TCP:
			return read(s->fd, buffer, nbytes);
			break;
		case SCTP:
#ifdef HAVE_SCTP_FENICE
			if (!protodata) {
				return -1;
			}
			return sctp_recvmsg(transport->fd, buffer, nbytes, NULL,
				 0, (struct sctp_sndrcvinfo *) protodata, NULL);
#endif
			break;
		default:
			break;
		}
#if HAVE_SSL
	}
#endif

	return -1;
}
