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

int sock_udp_read(int fd, void *buffer, int nbytes, struct sockaddr_storage *from,  socklen_t from_len, int *remote_port)
{
	int n;

	n = recvfrom(fd, buffer, nbytes, 0, (struct sockaddr *)(from), &from_len);
	/*n = read(fd, buffer, nbytes);*/

	if (n > 0) {
		*remote_port = ntohs(((struct sockaddr_in *)(from))->sin_port);
	}
	
	return n;
}

