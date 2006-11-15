/* *
 *
 *
 *  This file is part of NetEmbryo
 *
 *  NetEmbryo -- default network wrapper
 *
 *  Copyright (C) 2006 by
 *
 *      - Dario Gallucci	<dario.gallucci@gmail.com>
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

int Sock_socketpair(Sock *pair[]) {

	int sdpair[2], i, res;

	if ((res = socketpair(AF_UNIX, SOCK_DGRAM, 0, sdpair)) < 0) {
		net_log(NET_LOG_ERR, "Sock_socketpair() failure.\n");
		return res;
	}
	
	if (!(pair[0] = g_new0(Sock, 1))) {
		net_log(NET_LOG_FATAL, "Unable to allocate first Sock struct in Sock_socketpair().\n");
		close (sdpair[0]);
		close (sdpair[1]);
		return -1;
	}
	if (!(pair[1] = g_new0(Sock, 1))) {
		net_log(NET_LOG_FATAL, "Unable to allocate second Sock struct in Sock_socketpair().\n");
		close (sdpair[0]);
		close (sdpair[1]);
		g_free(pair[0]);
		return -1;
	}

	for (i = 0; i < 2; i++) {
		pair[i]->fd = sdpair[i];
		pair[i]->socktype = LOCAL;
	}
	
	return res;
}
