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
 * 
 * */


#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <netembryo/wsocket.h>

#ifndef IN_IS_ADDR_MULTICAST
#define IN_IS_ADDR_MULTICAST(a)	((((in_addr_t)(a)) & 0xf0000000) == 0xe0000000)
#endif

#ifndef IN6_IS_ADDR_MULTICAST
#define IN6_IS_ADDR_MULTICAST(a) ((a)->s6_addr[0] == 0xff)
#endif

/*return values:
 * 0 --> false
 * 1 --> true
 * -1 -> unknown
 * */
int16_t is_multicast(union ADDR *addr, sa_family_t family)
{
	switch (family) {
		case AF_INET: {
			struct in_addr *in;
			in = &(addr->mreq_in.imr_interface4);
			return IN_IS_ADDR_MULTICAST(ntohl(in->s_addr));
		}
#ifdef  IPV6
		case AF_INET6: {
			struct in6_addr *in6;
			in6 = &(addr->mreq_in6.imr_interface6);
			return IN6_IS_ADDR_MULTICAST(in6);
		}
#endif
#ifdef  AF_UNIX
		case AF_UNIX:
			return -1;
#endif
#ifdef  HAVE_SOCKADDR_DL_STRUCT
		case AF_LINK: 
			return -1;
#endif
		default:
			return -1;
	}
	
	return -1;
}

