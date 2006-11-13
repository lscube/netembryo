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

int Sock_write(Sock *s, void *buffer, int nbytes, void *protodata, int flags)
{
#ifdef HAVE_SCTP_FENICE
	struct sctp_sndrcvinfo sinfo;
#endif

#if HAVE_SSL
	if(s->flags & USE_SSL)
		return sock_SSL_write(s->ssl, buffer, nbytes);
	else {
#endif		
		switch (s->socktype) {
		case TCP:
			return send(s->fd, buffer, nbytes, flags);
			break;
		case UDP:
			if (!protodata) {
				protodata = &(s->remote_stg);
			}
			return sendto(s->fd, buffer, nbytes, flags, (struct sockaddr *) 
					protodata, sizeof(struct sockaddr_storage));
			break;
		case SCTP:
#ifdef HAVE_SCTP_FENICE
			if (!protodata) {
				protodata = &sinfo;
				memset(protodata, 0, sizeof(struct sctp_sndrcvinfo));
			}
			return sctp_send(s->fd, buffer, nbytes, 
				(struct sctp_sndrcvinfo *) protodata, flags);
#endif
			break;
		case LOCAL:
			return send(s->fd, buffer, nbytes, flags);
			break;
		default:
			break;
		}
#if HAVE_SSL
	}
#endif		
	return -1;
}
