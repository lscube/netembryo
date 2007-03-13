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
 *  part of code is taken from NeMeSI source code
 * */

#ifndef __WSOCKET_H
#define __WSOCKET_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifdef HAVE_SCTP_FENICE
#include <netinet/sctp.h>
#define MAX_SCTP_STREAMS 15
#endif

#if HAVE_SSL
#include <openssl/ssl.h>
#endif

#ifndef IN_IS_ADDR_MULTICAST
#define IN_IS_ADDR_MULTICAST(a)	((((in_addr_t)(a)) & 0xf0000000) == 0xe0000000)
#endif

#if IPV6
#ifndef IN6_IS_ADDR_MULTICAST
#define IN6_IS_ADDR_MULTICAST(a) (((__const uint8_t *) (a))[0] == 0xff)
#endif
#endif //IPV6

#ifdef WORDS_BIGENDIAN
#define ntohl24(x) (x)
#else
#define ntohl24(x) (((x&0xff) << 16) | (x&0xff00) | ((x&0xff0000)>>16)) 
#endif

#ifndef HAVE_STRUCT_SOCKADDR_STORAGE
/* Structure large enough to hold any socket address (with the historical exception of 
AF_UNIX). 128 bytes reserved.  */
#if ULONG_MAX > 0xffffffff
# define __ss_aligntype __uint64_t
#else
# define __ss_aligntype __uint32_t
#endif
#define _SS_SIZE        128
#define _SS_PADSIZE     (_SS_SIZE - (2 * sizeof (__ss_aligntype)))

struct sockaddr_storage
{
    sa_family_t ss_family;      /* Address family */
    __ss_aligntype __ss_align;  /* Force desired alignment.  */
    char __ss_padding[_SS_PADSIZE];
};
#endif // HAVE_STRUCT_SOCKADDR_STORAGE

/** flags definition*/
typedef enum {
/** ssl flags */
	USE_SSL = 0x1,
	USE_TLS = 0x3, /* set also USE_SSL */
/** multicast flags */
	IS_MULTICAST = 0x4
} sock_flags;

/** socket type definition */
typedef enum {
/** socket fd not valid */
	SOCK_NONE,
/** IP based protcols */
	TCP,
	UDP,
	SCTP,
/** Local socket */
	LOCAL
} sock_type;

/** NOTE:
 *	struct ip_mreq {
 *		struct in_addr imr_multiaddr;
 *		struct in_addr imr_interface;
 *	}
 *
 *	struct ipv6_mreq {
 *		struct in6_addr	ipv6mr_multiaddr;
 *		unsigned int ipv6mr_interface;
 *	}
 **/

#if IPV6
struct ipv6_mreq_in6 {
	struct ipv6_mreq NETmreq6;
	struct in6_addr __imr_interface6;
};
#endif

struct ip_mreq_in {
	struct ip_mreq NETmreq;
	unsigned int __ipv4mr_interface;
};

#if 0
union ADDR {
	struct in_addr in;
	struct in6_addr in6;
};
#endif
union ADDR {
#if IPV6
	struct ipv6_mreq_in6 mreq_in6; /*struct in6_addr ipv6mr_multiaddr; struct in6_addr imr_interface6 ; unsigned int ipv6mr_interface; */
#endif //IPV6
	struct ip_mreq_in mreq_in; /*struct in_addr ipv4mr_multiaddr; struct in_addr imr_interface4; unsigned int ipv4mr_interface;*/
};
#if IPV6
	#define imr_interface6 __imr_interface6
	#define ipv6_interface NETmreq6.ipv6mr_interface
	#define ipv6_multiaddr NETmreq6.ipv6mr_multiaddr
#endif //IPV6
	#define ipv4_interface __ipv4mr_interface
	#define imr_interface4 NETmreq.imr_interface
	#define ipv4_multiaddr NETmreq.imr_multiaddr

/** Developer HowTo:
 *
 * union ADDR
 * 		struct ipv6_mreq_in6 mreq_in6
 * 			struct in6_addr ipv6_multiaddr	// IPv6 class D multicast address. defined =  NETmreq6.ipv6mr_multiaddr
 * 			struct in6_addr imr_interface6	// IPv6 address of local interface.
 * 			unsigned int ipv6_interface	// interface index, or 0
 * 			struct ipv6_mreq NETmreq6
 * 		 struct ip_mreq_in mreq_in
 * 	 		struct in_addr ipv4_multiaddr 	// IPv4 class D multicast address. defined = NETmreq.imr_multiaddr
 * 	 		struct in_addr imr_interface4	// IPv4 address of local interface. defined = NETmreq.imr_interface
 * 	 		unsigned int ipv4_interface	// interface index, or 0
 * 	 		struct ip_mreq NETmreq
 */

typedef struct {
	int fd;	//! stores socket file descriptor
	struct sockaddr_storage local_stg;	//! from getsockname
	struct sockaddr_storage remote_stg;	//! from getpeername or forced
	sock_type socktype;
	union ADDR addr;
	/** flags */
	sock_flags flags;
	/** human readable datas */
	char *remote_host;
	char *local_host;
	in_port_t remote_port;	//! stored in host order
	in_port_t local_port;	//! stored in host order
#if HAVE_SSL
	SSL *ssl;
#endif
} Sock;

#define WSOCK_ERRORPROTONOSUPPORT -5	
#define WSOCK_ERRORIOCTL	-4	
#define WSOCK_ERRORINTERFACE	-3	
#define WSOCK_ERROR	-2	
#define WSOCK_ERRFAMILYUNKNOWN	-1
#define WSOCK_OK 0
#define WSOCK_ERRSIZE	1
#define WSOCK_ERRFAMILY	2
#define WSOCK_ERRADDR	3
#define WSOCK_ERRPORT	4

/** low level wrappers */
int sockfd_to_family(int sockfd);
int gethostinfo(struct addrinfo **res, char *host, char *serv, struct addrinfo *hints);
int sock_connect(char *host, char *port, int *sock, sock_type socktype);
int sock_bind(char *host, char *port, int *sock, sock_type socktype);
int sock_accept(int sock);
int sock_listen(int s, int backlog);
int sock_close(int s);

/** host & port wrappers */
/* return the address in human readable string format */
const char *sock_ntop_host(const struct sockaddr *sa, char *str, size_t len);
/* return the port in network byte order (use ntohs to change it) */
int32_t sock_get_port(const struct sockaddr *sa);

/** multicast*/
int16_t is_multicast(union ADDR *addr, sa_family_t family);
int16_t is_multicast_address(const struct sockaddr *sa, sa_family_t family);
int mcast_join (int sockfd, const struct sockaddr *sa/*, socklen_t salen*/, const char *ifname, unsigned int ifindex, union ADDR *addr);
int mcast_leave(int sockfd, const struct sockaddr *sa/*, socklen_t salen*/);

#if HAVE_SSL
/** ssl wrappers */
SSL_CTX *create_ssl_ctx(void);
SSL *get_ssl_connection(int);
int sock_SSL_connect(SSL **, int);
int sock_SSL_accept(SSL **, int);
int sock_SSL_read(SSL *, void *, int);
int sock_SSL_write(SSL *, void *, int);
int sock_SSL_close(SSL *);
#endif

/** log facilities */
/* store pointer to external log function */
extern void (*net_log)(int, const char*, ...);
/* levels to be implemented by log function */
#define NET_LOG_FATAL 0 
#define NET_LOG_ERR 1
#define NET_LOG_WARN 2 
#define NET_LOG_INFO 3 
#define NET_LOG_DEBUG 4 
#define NET_LOG_VERBOSE 5 

/** ------------------------------- INTERFACE -------------------------------
 * TODO: write API specs
 */
Sock * Sock_connect(char *host, char *port, Sock *binded, sock_type socktype, sock_flags ssl_flag);
/* usually host is NULL for unicast. For multicast it is the multicast address.
* Change it (ifi_xxx, see Stevens Chap.16) */
Sock * Sock_bind(char *host, char *port, sock_type socktype, sock_flags ssl_flag);
/* returns pointer to a new Sock structure (generated using POSIX accept) */
Sock * Sock_accept(Sock *); 
int Sock_create_ssl_connection(Sock *s);
int Sock_listen(Sock *n, int backlog);
int Sock_read(Sock *, void *buffer, int nbytes, void *protodata, int flags); // protodata is sock_type dependant
int Sock_write(Sock *, void *buffer, int nbytes, void *protodata, int flags);
int Sock_close(Sock *);
void Sock_init(void (*)(int, const char*, ...));
int Sock_compare(Sock *, Sock *);
#define Sock_cmp Sock_compare
int Sock_socketpair(Sock *[]);
int Sock_set_dest(Sock *, struct sockaddr *);

/** low level access macro */
#define Sock_fd(A) ((A)->fd)
#define Sock_type(A) ((A)->socktype)

/** ioctl set properties for socket
 *	RETURN VALUE:
 *	Usually, on success zero is returned. A few ioctls use the return value
 *	as an output parameter and return a nonnegative value on success.
 *	On error, -1 is returned, and errno is set appropriately.
 */
int Sock_set_props(Sock *s, int request, int *on);

/*get_info.c*/
char * get_remote_host(Sock *);
char * get_local_host(Sock *);
inline int get_local_hostname(char *localhostname, size_t len); // return 0 if ok
in_port_t get_remote_port(Sock *);
in_port_t get_local_port(Sock *);
/*----------------------------------------------------------------------------------------------------------------------------*/
 
#endif
