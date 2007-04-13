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

int Sock_read(Sock *s, void *buffer, int nbytes, void *protodata, int flags)
{

	socklen_t sa_len = sizeof(struct sockaddr_storage);

	if(!s)
		return -1;

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
			return recvfrom(s->fd, buffer, nbytes, flags,
				(struct sockaddr *) protodata, &sa_len);
			break;
		case TCP:
			return recv(s->fd, buffer, nbytes, flags);
			break;
		case SCTP:
#ifdef HAVE_LIBSCTP
			if (!protodata) {
				return -1;
			}
			return sctp_recvmsg(s->fd, buffer, nbytes, NULL,
				 0, (struct sctp_sndrcvinfo *) protodata, &flags);
			/* flags is discarted: usage may include detection of
			 * incomplete packet due to small buffer or detection of
			 * notification data (this last should be probably
			 * handled inside netembryo).
			 */
#endif
			break;
		case LOCAL:
			return recv(s->fd, buffer, nbytes, flags);
			break;
		default:
			break;
		}
#if HAVE_SSL
	}
#endif

	return -1;
}
